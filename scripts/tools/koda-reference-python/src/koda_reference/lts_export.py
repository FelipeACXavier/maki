"""Explicit reachable-state graph and Aldebaran AUT export for KODA.

This module is deliberately independent of the existing *trace* explorer.
States correspond to stable macrostep boundaries. A graph edge executes one
whole atomic macrostep, including all internal nondeterministic decisions.
The AUT event projection expands a macrostep into an uninterruptible chain of
observable labels; only stable states have outgoing macrostep alternatives.

The graph is complete only for its configured one-trigger environment driver
and only when no exploration limit was reached. It is NOT a general proof of
correctness or an equivalence-preserving abstraction of arbitrary KODA code.
"""
from __future__ import annotations

from collections import deque
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Iterator
import json

from .engine import ChoiceNeeded, Engine
from .model import ModelError


@dataclass(frozen=True)
class LtsEdge:
    source: int
    target: int
    operation: str
    input: str
    outputs: tuple[str, ...]


@dataclass
class LtsGraph:
    initial: int
    states: list[dict[str, Any]]
    edges: list[LtsEdge]
    complete: bool
    cutoff_reasons: list[str]
    expanded_states: int
    include_abort: bool
    include_reset: bool

    def report(self) -> dict[str, Any]:
        return {
            'schema': 'koda.reference.lts.v1',
            'initial': self.initial,
            'stable_states': len(self.states),
            'macrostep_transitions': len(self.edges),
            'expanded_states': self.expanded_states,
            'complete': self.complete,
            'cutoff_reasons': self.cutoff_reasons,
            'environment_driver': {
                'initial_task_trigger': 'exactly_once',
                'dispatch': 'any_pending_event',
                'optional_task_abort': 'at_most_once' if self.include_abort else 'disabled',
                'optional_task_reset': 'at_most_once' if self.include_reset else 'disabled',
                'retrigger': 'disabled',
            },
            'states': self.states,
            'macrosteps': [
                {'source': e.source, 'target': e.target, 'operation': e.operation,
                 'input': e.input, 'outputs': list(e.outputs)}
                for e in self.edges
            ],
        }


@dataclass
class _Vertex:
    engine: Engine
    started: bool = False
    abort_used: bool = False
    reset_used: bool = False

    def key(self) -> tuple:
        return (self.started, self.abort_used, self.reset_used,
                self.engine.semantic_key())


def _stable_engine(engine: Engine) -> Engine:
    """Discard exploration-only history, retaining the actual runtime state."""
    candidate = engine.clone()
    candidate.decisions = []
    candidate._choice_cursor = 0
    candidate.history = []
    candidate._index = 0
    candidate._outputs = []
    candidate.last_cutoff = False
    # Neither pending broadcasts nor re-entrant execution are stable states.
    candidate.semantic_key()
    return candidate


def _alternatives(base: Engine, method: str, *args: Any) -> Iterator[tuple[Engine, dict[str, Any]]]:
    """Enumerate every mock choice made during exactly one atomic operation.

    Resetting the choice plan for EACH edge is essential: persisted decision
    histories must never constrain outgoing transitions from merged states.
    """
    work: list[list[int]] = [[]]
    while work:
        plan = work.pop()
        candidate = base.clone()
        candidate.decisions = plan.copy()
        candidate._choice_cursor = 0
        try:
            step = getattr(candidate, method)(*args)
        except ChoiceNeeded as missing:
            if missing.position != len(candidate.decisions):
                raise ModelError('choice plan diverged while expanding the LTS')
            for n in reversed(range(missing.options)):
                work.append(candidate.decisions + [n])
        else:
            yield candidate, step


def _state_details(vertex: _Vertex) -> dict[str, Any]:
    engine = vertex.engine
    return {
        'root_phase': engine.state[engine.root.path].phase,
        'started': vertex.started,
        'abort_used': vertex.abort_used,
        'reset_used': vertex.reset_used,
        'runtime': {
            p: {'phase': st.phase, 'generation': st.generation,
                'attempts': st.attempts, 'current': st.current,
                'completed': sorted(st.completed), 'iteration': st.iteration,
                'waiting': st.waiting}
            for p, st in sorted(engine.state.items())
        },
        'pending': [
            {'source': source, 'generation': generation, 'kind': kind}
            for source, generation, kind in sorted(
                (p.source, p.generation, p.kind) for p in engine.pending
            )
        ],
    }


def explore_lts(engine: Engine, *, max_states: int = 5000,
                max_transitions: int = 50000,
                include_abort: bool = False,
                include_reset: bool = False) -> LtsGraph:
    """Enumerate the reachable LTS under a single-invocation environment driver.

    The initial vertex is the pre-trigger machine. The driver issues exactly
    one task.trigger and then permits any pending completion dispatch. With
    include_abort/reset enabled, at most one of each can be issued, in any
    reachable phase where the corresponding operation is legal. Those driver
    control bits are part of the explored state, preventing accidental loss
    of external-operation choices when runtime states merge.

    All bounds are hard limits. A graph cut off by a limit is ALWAYS marked
    incomplete, even if the AUT file is syntactically valid.
    """
    if max_states < 1 or max_transitions < 1:
        raise ValueError('max_states and max_transitions must be positive')
    if engine.history:
        raise ModelError('AUT exploration must start with a fresh Engine')
    original = _Vertex(_stable_engine(engine))
    vertices = [original]
    indices = {original.key(): 0}
    queue = deque([0])
    edges: list[LtsEdge] = []
    edge_keys: set[tuple[int, int, str, tuple[str, ...]]] = set()
    cutoff_reasons: list[str] = []
    expanded = 0
    limit_hit = False

    while queue and not limit_hit:
        source = queue.popleft()
        vertex = vertices[source]
        root_phase = vertex.engine.state[vertex.engine.root.path].phase

        operations: list[tuple[str, int | None]] = []
        if not vertex.started:
            operations.append(('trigger', None))
        else:
            # Independent pending completions may dispatch in any order.
            # Identical pending entries produce identical successor choices.
            seen_pending: set[tuple[str, int, str]] = set()
            for index, pending in enumerate(vertex.engine.pending):
                key = (pending.source, pending.generation, pending.kind)
                if key not in seen_pending:
                    seen_pending.add(key)
                    operations.append(('dispatch', index))
            if include_abort and not vertex.abort_used and root_phase in ('Running', 'Handling'):
                operations.append(('abort', None))
            if include_reset and not vertex.reset_used and root_phase == 'Error':
                operations.append(('reset', None))

        for operation, index in operations:
            for successor, step in _alternatives(vertex.engine, operation,
                                                  *([index] if index is not None else [])):
                state = _Vertex(
                    engine=_stable_engine(successor),
                    started=True,
                    abort_used=vertex.abort_used or operation == 'abort',
                    reset_used=vertex.reset_used or operation == 'reset',
                )
                key = state.key()
                target = indices.get(key)
                edge_key = ((source, target, step['input'], tuple(step['outputs']))
                            if target is not None else None)
                # Distinct mock scripts may generate precisely the same
                # observable edge and successor; do not duplicate that edge.
                if edge_key is not None and edge_key in edge_keys:
                    continue
                if len(edges) >= max_transitions:
                    cutoff_reasons.append(f'max_transitions={max_transitions}')
                    limit_hit = True
                    break
                if target is None:
                    if len(vertices) >= max_states:
                        cutoff_reasons.append(f'max_states={max_states}')
                        limit_hit = True
                        break
                    target = len(vertices)
                    indices[key] = target
                    vertices.append(state)
                    queue.append(target)
                edge_keys.add((source, target, step['input'], tuple(step['outputs'])))
                edges.append(LtsEdge(source, target, operation, step['input'],
                                     tuple(step['outputs'])))
            if limit_hit:
                break
        if not limit_hit:
            expanded += 1

    return LtsGraph(
        initial=0,
        states=[_state_details(v) for v in vertices],
        edges=edges,
        complete=not limit_hit,
        cutoff_reasons=cutoff_reasons,
        expanded_states=expanded,
        include_abort=include_abort,
        include_reset=include_reset,
    )


def _project(label: str, observe: str) -> str:
    if observe == 'all':
        return label
    if label.startswith('task.'):
        return label
    if observe == 'orchestration' and label.startswith(('child.', 'flow.')):
        return label
    return 'tau'


def _quote_aut(label: str) -> str:
    # Aldebaran AUT string-escape conventions; JSON produces compatible
    # quoting for ordinary action labels and escapes embedded quotes.
    return json.dumps(label, ensure_ascii=True)


def aut_text(graph: LtsGraph, *, observe: str = 'orchestration',
             macro_boundaries: bool = False) -> str:
    """Serialize AUT with stable vertices and atomic event chains.

    Each macrostep is expanded to a linear chain of emitted output events.
    Only stable vertices have branching outgoing transitions; the inserted
    chain states have exactly one outgoing transition. Internal/scheduler
    labels can be projected to tau, not removed. An optional begin/end marker
    makes macrostep boundaries observable for compatible backend adapters.
    """
    if observe not in ('task', 'orchestration', 'all'):
        raise ValueError('observe must be task, orchestration, or all')
    next_id = len(graph.states)
    transitions: list[tuple[int, str, int]] = []
    for edge in graph.edges:
        labels = [_project(value, observe) for value in edge.outputs]
        if macro_boundaries:
            labels = ['macro.begin', *labels, 'macro.end']
        if not labels:
            labels = ['tau']
        current = edge.source
        for i, label in enumerate(labels):
            if i == len(labels) - 1:
                target = edge.target
            else:
                target = next_id
                next_id += 1
            transitions.append((current, label, target))
            current = target
    lines = [f'des ({graph.initial},{len(transitions)},{next_id})']
    lines.extend(f'({source},{_quote_aut(label)},{target})'
                 for source, label, target in transitions)
    return '\n'.join(lines) + '\n'


def write_aut(graph: LtsGraph, path: Path, *, observe: str = 'orchestration',
              macro_boundaries: bool = False,
              report_path: Path | None = None) -> None:
    path.write_text(aut_text(graph, observe=observe,
                             macro_boundaries=macro_boundaries), encoding='utf-8')
    if report_path is not None:
        report = graph.report()
        report['aut'] = {'observe': observe,
                         'macro_boundaries': macro_boundaries,
                         'location': str(path)}
        report_path.write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8')
