"""Executable reference for Call, Sequence and Join (parallel).

Atomic macrosteps, deferred capability events, invocation generations, and
nondeterministic event dispatch. No production compiler modules are imported.
"""
from __future__ import annotations
from dataclasses import dataclass, field
from copy import deepcopy
from typing import Any
from .model import Node, ModelError
from .guards import eligible_branches


@dataclass
class Runtime:
    phase: str = "Idle"  # Idle, Running, Handling, Error
    generation: int = 0
    attempts: int = 0
    current: int = 0   # sequence's currently running child
    completed: set[int] = field(default_factory=set)  # join
    iteration: int = 0
    waiting: str = ''  # cleanup/timeout handling mode


@dataclass(frozen=True)
class Pending:
    source: str
    generation: int
    kind: str


class ChoiceNeeded(Exception):
    def __init__(self, position: int, options: int):
        self.position, self.options = position, options


class Engine:
    def __init__(self, root: Node, capabilities: dict[str, dict]):
        self.root = root
        self.capabilities = deepcopy(capabilities)
        self.nodes: dict[str, Node] = {}
        self.parents: dict[str, tuple[str, int]] = {}
        def walk(n: Node) -> None:
            self.nodes[n.path] = n
            for i, child in enumerate(n.children):
                self.parents[child.path] = (n.path, i)
                walk(child)
        walk(root)
        self.state = {path: Runtime() for path in self.nodes}
        self.pending: list[Pending] = []
        self.history: list[dict[str, Any]] = []
        self._outputs: list[str] = []
        self._index = 0
        self._in_macrostep = False
        self.decisions: list[int] = []
        self._choice_cursor = 0
        self.last_cutoff = False
        # Defer re-entrant observer callbacks until the surrounding trigger
        # and its ancestors have finished entering Running.
        self._broadcasts: list[tuple[str,int]] = []

    def clone(self) -> "Engine":
        return deepcopy(self)

    def _emit(self, action: str) -> None:
        self._outputs.append(action)

    def _choose(self, options: list[Any]) -> Any:
        if self._choice_cursor >= len(self.decisions):
            if len(options) == 1:
                selected = 0
                self.decisions.append(0)
            else:
                raise ChoiceNeeded(self._choice_cursor, len(options))
        else:
            selected = self.decisions[self._choice_cursor]
            if selected >= len(options):
                raise ModelError('invalid choice plan')
        self._choice_cursor += 1
        return options[selected]

    def _mock(self, node: Node) -> dict:
        cfg = self.capabilities[node.capability]
        return cfg['events'][node.event] if node.event is not None else cfg

    def normalized_trace(self) -> list[dict[str, Any]]:
        """Stable JSON event format, preserving macrostep and intra-step order."""
        result = []
        for step in self.history:
            for order, label in enumerate(step['outputs']):
                parts = label.split('.')
                if parts[0] == 'task':
                    operation = parts[1]
                    category = 'invoke' if operation in ('trigger','abort','reset') else (
                        'reply' if operation in ('reply','abort_reply','reset_reply') else 'event')
                    event = {'scope':'task','kind':category,'operation':operation,
                             'target':'root','result':parts[2] if len(parts)>2 else None}
                elif parts[0] == 'child':
                    # child.<path>.operation[.result]; path can have periods
                    op_index = next((i for i, x in enumerate(parts) if i>0 and x in
                                     ('trigger','service','reply','abort','abort_reply','reset','reset_reply','event')), -1)
                    path = '.'.join(parts[1:op_index])
                    op = parts[op_index]
                    n = self.nodes[path]
                    event = {'scope':'capability','kind': 'event' if op == 'event' else
                             'reply' if op in ('reply','abort_reply','reset_reply') else 'invoke',
                             'operation':op,'target':path,
                             'symbol':f'{n.capability}.{n.event}' if n.event else n.capability,
                             **({'call_kind':'service'} if n.kind == 'service' else
                                {'call_kind':'abort_call'} if n.kind == 'abort_call' else {}),
                             'result':'.'.join(parts[op_index+1:]) or None}
                elif parts[0] == 'flow':
                    operation = next((x for x in ('trigger','reply','event') if x in parts),None)
                    pos = parts.index(operation)
                    event = {'scope':'flow',
                             'kind':'invoke' if operation=='trigger' else operation,
                             'operation':operation,'target':'.'.join(parts[1:pos]),
                             'symbol':parts[-1],
                             'result':parts[pos+1] if operation!='trigger' else None}
                elif parts[0] == 'environment':
                    event = {'scope':'scheduler','kind':'dispatch','label':label}
                else:
                    event = {'scope':'internal','kind':'internal','label':label}
                if 'target' in event and event['target'] in step.get('generations',{}):
                    event['generation'] = step['generations'][event['target']]
                result.append({'macrostep':step['macrostep'],'ordinal':order,**event})
        return result

    def _run(self, input_label: str, operation) -> dict[str, Any]:
        if self._in_macrostep:
            raise RuntimeError("external event cannot interleave an atomic macrostep")
        self._in_macrostep = True
        self._outputs = []
        try:
            operation()
            # Deliver shared-execution notifications at the end of the same
            # atomic macrostep, not recursively from the middle of a trigger.
            # Dezyne may order these earlier; hospital-level comparison uses
            # a task-facing abstraction until exact resource wiring is modeled.
            while self._broadcasts:
                path, generation = self._broadcasts.pop(0)
                state = self.state[path]
                if state.generation != generation or state.phase not in ("Running", "Handling"):
                    continue
                state.phase = "Idle"
                self._emit(f"child.{path}.event.ABORTED")
                self._bubble(path, "ABORTED")
            record = {"macrostep": self._index, "input": input_label,
                      "outputs": list(self._outputs),
                      "root_state": self.state[self.root.path].phase,
                      "generations": {p: x.generation for p,x in self.state.items()}}
            self.history.append(record)
            self._index += 1
            return record
        finally:
            self._in_macrostep = False

    def trigger(self) -> dict[str, Any]:
        def work():
            self._emit("task.trigger")
            result = self._trigger(self.root)
            self._emit(f"task.reply.{'DONE' if result == 'TERMINATE' else result}")
        return self._run("task.trigger", work)

    def abort(self) -> dict[str, Any]:
        def work():
            self._emit("task.abort")
            result = self._abort(self.root)
            self._emit(f"task.abort_reply.{result}")
        return self._run("task.abort", work)

    def reset(self) -> dict[str, Any]:
        def work():
            self._emit("task.reset")
            result = self._reset(self.root)
            self._emit(f"task.reset_reply.{result}")
        return self._run("task.reset", work)

    def dispatch(self, index: int = 0) -> dict[str, Any]:
        if not (0 <= index < len(self.pending)):
            raise ModelError("pending-event index out of range")
        # Choose and remove event before the macrostep starts. Its validity
        # is checked inside the macrostep against the current generation.
        event = self.pending[index]
        def work():
            self.pending.pop(index)
            self._emit(f"environment.dispatch.{event.source}.{event.kind}")
            state = self.state[event.source]
            if state.phase not in ("Running", "Handling") or state.generation != event.generation:
                self._emit(f"internal.discard_stale.{event.source}")
                return
            node = self.nodes[event.source]
            if event.kind == 'TIMEOUT':
                if node.kind != 'within' or state.current != 0 or state.phase != 'Running':
                    self._emit(f"internal.discard_stale.{event.source}")
                    return
                outcome = self._abort(node.children[0])
                if outcome == 'SUCCESS':
                    self._start_within_fallback(node)
                elif outcome == 'HANDLING':
                    state.phase = 'Handling'
                    state.waiting = 'timeout_abort'
                else:
                    state.phase = 'Error'
                    self._bubble(node.path, 'FAILURE')
                return
            if event.kind in ('REPEAT', 'RETRY'):
                expected_waiting = ('repeat_scheduled' if event.kind == 'REPEAT'
                                    else 'retry_scheduled')
                if node.kind != event.kind.lower() or state.waiting != expected_waiting:
                    self._emit(f"internal.discard_stale.{event.source}")
                    return
                # A deferred iteration gets its own atomic scheduler step.
                # In particular, a zero interval does NOT allow a restart in
                # the completion macrostep of the previous iteration.
                state.waiting = ''
                self._restart_loop(node)
                return
            if node.kind not in ('call', 'abort_call'):
                raise AssertionError(f"unexpected pending completion source: {node.kind}")
            state.phase = "Idle" if event.kind in ("SUCCESS", "ABORTED", "TERMINATED") else "Error"
            self._emit(f"child.{event.source}.event.{event.kind}")
            self._bubble(node.path, event.kind)
        return self._run(f"dispatch:{event.source}:{event.kind}", work)

    def _trigger(self, node: Node) -> str:
        st = self.state[node.path]
        if st.phase != "Idle":
            raise ModelError(f"{node.path}: trigger requires Idle, got {st.phase}")
        st.generation += 1
        st.current = 0
        st.completed.clear()
        st.iteration = 0
        st.waiting = ''
        if node.kind == 'success':
            st.phase = 'Idle'
            return 'TERMINATE'
        if node.kind == 'failure':
            st.phase = 'Error'
            return 'FAILURE'
        if node.kind == 'continue':
            st.phase = 'Idle'
            return 'DONE'
        if node.kind in ('on_abort', 'on_error'):
            result = self._trigger(node.children[0])
            st.phase = {'DONE':'Idle','ACCEPTED':'Running','FAILURE':'Error',
                        'TERMINATE':'Idle'}[result]
            return result
        if node.kind == 'abort_call':
            # A direct abort call is a new action that requests cancellation of
            # a shared capability execution. Dezyne's cabort_call reports
            # ACCEPTED then defers its successful completion to a later step.
            matching_calls = [n for n in self.nodes.values()
                              if n.kind == 'call' and n.capability == node.capability]
            active = [n for n in matching_calls if self.state[n.path].phase == 'Running']
            errored = any(self.state[n.path].phase == 'Error' for n in matching_calls)
            if active:
                # Active shared execution: its underlying abort can succeed or fail.
                choices = self._mock(active[0]).get('abort', ['SUCCESS', 'FAILURE'])
                if isinstance(choices, str):
                    choices = [choices]
                aborted = self._choose(choices)
            elif errored:
                # The capability armour is not available while in Error.
                aborted = 'FAILURE'
            else:
                # Idle capability armour: abort is a successful no-op.  Do not
                # invent a FAILURE here solely from the generic iabort protocol.
                aborted = 'SUCCESS'
            self._emit(f'child.{node.path}.trigger')
            if aborted == 'FAILURE':
                st.phase = 'Error'
                self._emit(f'child.{node.path}.reply.FAILURE')
                return 'FAILURE'
            # Notify each observer of *this execution* once. A future shared
            # resource model should use explicit execution IDs rather than
            # capability instance alone.
            for observer in active:
                self._broadcasts.append((observer.path, self.state[observer.path].generation))
            st.phase = 'Running'
            self._emit(f'child.{node.path}.reply.ACCEPTED')
            self.pending.append(Pending(node.path, st.generation, 'SUCCESS'))
            return 'ACCEPTED'
        if node.kind in ("call", "service"):
            st.attempts += 1
            cfg = self.capabilities[node.capability]
            cfg = self._mock(node)
            if 'outcomes' in cfg:
                script = self._choose(cfg['outcomes'])
            else:
                scripts = cfg['trigger']
                attempt = st.attempts - 1
                if attempt >= len(scripts):
                    raise ModelError(f"{node.capability}: no mock script for attempt {attempt+1}")
                script = scripts[attempt]
            result = script["reply"]
            operation = "service" if node.kind == "service" else "trigger"
            self._emit(f"child.{node.path}.{operation}")
            self._emit(f"child.{node.path}.reply.{result}")
            st.phase = {"DONE":"Idle", "ACCEPTED":"Running", "FAILURE":"Error"}[result]
            if node.kind == "service" and result not in ("DONE", "FAILURE"):
                raise ModelError(f"{node.path}: synchronous service cannot return {result}")
            if result == "ACCEPTED":
                # Environment event becomes pending, never dispatched in this step.
                self.pending.append(Pending(node.path, st.generation, script["event"]))
            return result
        if node.kind == 'flow_call':
            self._emit(f'flow.{node.path}.trigger.{node.flow}')
            outcome = self._trigger(node.children[0])
            self._emit(f'flow.{node.path}.reply.{outcome}.{node.flow}')
            st.phase = {'DONE':'Idle', 'ACCEPTED':'Running', 'FAILURE':'Error',
                        'TERMINATE':'Idle'}[outcome]
            return outcome
        if node.kind == 'within':
            result = self._trigger(node.children[0])
            st.phase = {'ACCEPTED':'Running','DONE':'Idle','FAILURE':'Error', 'TERMINATE':'Idle'}[result]
            if result == 'ACCEPTED':
                # Alarm competes with capability completion as an environment
                # event; real duration ordering needs a clocked scheduler.
                self.pending.append(Pending(node.path, st.generation, 'TIMEOUT'))
            return result
        if node.kind == 'repeat':
            # Repeat accepts the invocation now, but schedules its FIRST body
            # invocation for a distinct macrostep (also when interval_ms == 0).
            # Iteration is incremented by _restart_loop when the event runs.
            st.phase = 'Running'
            st.waiting = 'repeat_scheduled'
            self.pending.append(Pending(node.path, st.generation, 'REPEAT'))
            return 'ACCEPTED'
        if node.kind == 'retry':
            # Retry differs from Repeat: its first attempt is synchronous
            # with the enclosing trigger macrostep.
            st.iteration = 1
            result = self._trigger(node.children[0])
            if result == 'FAILURE':
                # A synchronous rejection is still a failed attempt. The first
                # retry MUST NOT finish immediately if attempts remain. Reset
                # now, then defer the next attempt to another macrostep.
                if self._schedule_retry_after_failure(node):
                    return 'ACCEPTED'
            st.phase = {'ACCEPTED':'Running','DONE':'Idle','FAILURE':'Error','TERMINATE':'Idle'}[result]
            return result
        if node.kind in ('choose', 'either'):
            # Preserve first-true semantics for known guards. Unknown world
            # values induce only feasible branches; no value is invented.
            candidates = (eligible_branches(node.guard_expressions)
                          if node.kind == 'choose' else list(range(len(node.children))))
            if not candidates:
                st.phase = 'Error'
                return 'FAILURE'
            chosen = self._choose(candidates)
            st.current = chosen
            result = self._trigger(node.children[chosen])
            st.phase = {'ACCEPTED':'Running','DONE':'Idle','FAILURE':'Error','TERMINATE':'Idle'}[result]
            return result
        if node.kind == "sequence":
            for i, ch in enumerate(node.children):
                st.current = i
                result = self._trigger(ch)
                if result == "DONE":
                    continue
                st.phase = {'ACCEPTED':'Running', 'FAILURE':'Error',
                            'TERMINATE':'Idle'}[result]
                return result
            st.phase = "Idle"
            return "DONE"
        if node.kind == "parallel":
            # Trigger ALL children, including when an earlier child fails.
            results = [self._trigger(ch) for ch in node.children]
            st.completed = {i for i, r in enumerate(results) if r == "DONE"}
            if "FAILURE" in results or "TERMINATE" in results:
                for i, child in enumerate(node.children):
                    if self.state[child.path].phase == "Running":
                        self._abort(child)
                st.phase = "Error" if "FAILURE" in results or any(
                    self.state[ch.path].phase == 'Error' for ch in node.children) else "Idle"
                return "FAILURE" if st.phase == "Error" else "TERMINATE"
            if len(st.completed) == len(node.children):
                st.phase = "Idle"
                return "DONE"
            st.phase = "Running"
            return "ACCEPTED"
        raise AssertionError(node.kind)

    def _bubble(self, path: str, kind: str) -> None:
        """Propagate an asynchronous child outcome to its ancestors."""
        if path == self.root.path:
            # Termination is a successful early exit, not normal continuation.
            self._emit(f"task.event.{'SUCCESS' if kind == 'TERMINATED' else kind}")
            return
        parent_path, idx = self.parents[path]
        node = self.nodes[parent_path]
        st = self.state[parent_path]
        if st.phase not in ('Running', 'Handling'):
            # This can happen for a late event; it must not revive a parent.
            self._emit(f"internal.ignored_parent_inactive.{parent_path}")
            return
        if node.kind == 'on_error':
            if idx == 0:
                if kind == 'FAILURE':
                    reset = self._reset(node.children[0])
                    if reset != 'SUCCESS':
                        st.phase = 'Error'
                        self._bubble(parent_path, 'FAILURE')
                        return
                    result = self._trigger(node.children[1])
                    if result == 'ACCEPTED':
                        st.phase = 'Handling'
                        return
                    st.phase = 'Error' if result == 'FAILURE' else 'Idle'
                    self._bubble(parent_path, 'FAILURE' if result == 'FAILURE' else
                                 'TERMINATED' if result == 'TERMINATE' else 'SUCCESS')
                    return
                st.phase = 'Error' if kind == 'FAILURE' else 'Idle'
                self._bubble(parent_path, kind)
                return
            st.phase = 'Error' if kind == 'FAILURE' else 'Idle'
            self._bubble(parent_path, kind)
            return
        if node.kind == 'on_abort':
            if idx == 0:
                if kind == 'ABORTED':
                    # An externally signalled interruption invokes the handler.
                    self._begin_abort_handler(node, report_to_parent=True)
                    return
                st.phase = 'Idle' if kind in ('SUCCESS','TERMINATED') else 'Error'
                self._bubble(parent_path, kind)
                return
            # Handler finished after primary interruption / external abort.
            st.phase = 'Idle' if kind in ('SUCCESS','TERMINATED') else 'Error'
            self._bubble(parent_path, kind)
            return
        if node.kind == 'flow_call':
            st.phase = 'Idle' if kind in ('SUCCESS','ABORTED','TERMINATED') else 'Error'
            self._emit(f'flow.{node.path}.event.{kind}.{node.flow}')
            self._bubble(parent_path,kind)
            return
        if node.kind in ('choose','either'):
            if idx != st.current:
                self._emit(f'internal.ignored_inactive_child.{path}')
                return
            st.phase = 'Error' if kind == 'FAILURE' else 'Idle'
            self._bubble(parent_path, kind)
            return
        if node.kind == 'within':
            if idx == 0 and st.waiting == 'timeout_abort':
                if kind in ('ABORTED', 'SUCCESS'):
                    self._start_within_fallback(node)
                else:
                    st.phase = 'Error' if kind == 'FAILURE' else 'Idle'
                    self._bubble(parent_path, kind)
                return
            if idx != st.current:
                self._emit(f'internal.ignored_inactive_child.{path}')
                return
            st.phase = 'Error' if kind == 'FAILURE' else 'Idle'
            self._bubble(parent_path, kind)
            return
        if node.kind in ('repeat', 'retry'):
            if kind == 'TERMINATED':
                st.phase = 'Idle'
                self._bubble(parent_path, 'TERMINATED')
                return
            if node.kind == 'repeat' and kind == 'SUCCESS':
                if node.iterations and st.iteration >= node.iterations:
                    st.phase = 'Idle'
                    self._bubble(parent_path,'SUCCESS')
                    return
                # Schedule the next iteration as a separate macrostep, EVEN
                # when interval_ms == 0.  The interval is a minimum delay, not
                # permission to run another iteration in this event's step.
                # The reference model currently abstracts elapsed time, so
                # positive intervals are also scheduled as untimed events.
                st.waiting = 'repeat_scheduled'
                st.phase = 'Running'
                self.pending.append(Pending(node.path, st.generation, 'REPEAT'))
                return
            if node.kind == 'retry' and kind == 'FAILURE':
                if self._schedule_retry_after_failure(node):
                    return
                st.phase = 'Error'
                self._bubble(parent_path, 'FAILURE')
                return
            if kind == 'DONE' and node.kind == 'repeat':
                # Synchronous DONE finishes one iteration, not the enclosing
                # Repeat. Keep its lifecycle Running until _bubble decides
                # whether to schedule another iteration or finish the loop.
                st.phase = 'Running'
                self._bubble(node.children[0].path, 'SUCCESS')
                return

            st.phase = 'Error' if kind == 'FAILURE' else 'Idle'
            self._bubble(parent_path, kind)
            return
        if node.kind == "sequence":
            if idx != st.current:
                self._emit(f"internal.ignored_inactive_child.{path}")
                return
            if kind == "SUCCESS":
                for i in range(idx + 1, len(node.children)):
                    st.current = i
                    result = self._trigger(node.children[i])
                    if result == "DONE":
                        continue
                    if result == "ACCEPTED":
                        st.phase = 'Running'
                        return
                    st.phase = 'Idle' if result == 'TERMINATE' else 'Error'
                    self._bubble(parent_path, 'TERMINATED' if result == 'TERMINATE' else 'FAILURE')
                    return
                st.phase = "Idle"
                self._bubble(parent_path, "SUCCESS")
                return
            st.phase = "Error" if kind == "FAILURE" else "Idle"
            self._bubble(parent_path, kind)
            return
        if node.kind == "parallel":
            if kind == "SUCCESS":
                st.completed.add(idx)
                if len(st.completed) == len(node.children):
                    st.phase = "Idle"
                    self._bubble(parent_path, "SUCCESS")
                return
            for i, child in enumerate(node.children):
                if i != idx and self.state[child.path].phase == "Running":
                    self._abort(child)
            st.phase = "Error" if kind == "FAILURE" else "Idle"
            # If peer abort fails after ABORTED, the wrapper is not clean.
            if kind in ('ABORTED','TERMINATED') and any(self.state[ch.path].phase == "Error" for ch in node.children):
                st.phase = "Error"
                self._bubble(parent_path, "FAILURE")
            else:
                self._bubble(parent_path, kind)
            return
        raise AssertionError(node.kind)

    def _schedule_retry_after_failure(self, node: Node) -> bool:
        """Reset a failed Retry body and schedule the next attempt.

        Called from both synchronous trigger rejection and asynchronous child
        failure. `iterations` counts TOTAL attempts, not retries. The reset
        runs in the failure's macrostep; the next trigger cannot run until a
        separate scheduler dispatch. A failed reset or exhausted budget means
        failure propagates to the enclosing composition.
        """
        st = self.state[node.path]
        assert node.kind == 'retry'
        assert self.state[node.children[0].path].phase == 'Error'
        if st.iteration >= node.iterations:
            return False
        if self._reset(node.children[0]) != 'SUCCESS':
            return False
        st.phase = 'Running'
        st.waiting = 'retry_scheduled'
        self.pending.append(Pending(node.path, st.generation, 'RETRY'))
        return True

    def _start_within_fallback(self, node: Node) -> None:
        st = self.state[node.path]
        st.waiting = ''
        st.current = 1
        result = self._trigger(node.children[1])
        st.phase = {'ACCEPTED':'Running', 'DONE':'Idle', 'FAILURE':'Error', 'TERMINATE':'Idle'}[result]
        if result != 'ACCEPTED':
            self._bubble(node.path, {'DONE':'SUCCESS','FAILURE':'FAILURE','TERMINATE':'TERMINATED'}[result])

    def _restart_loop(self, node: Node) -> None:
        st = self.state[node.path]
        st.iteration += 1
        result = self._trigger(node.children[0])
        if node.kind == 'retry' and result == 'FAILURE':
            # Even an attempt started by a deferred RETRY may reject in the
            # trigger call. Process its failure *before* completing the Retry.
            if self._schedule_retry_after_failure(node):
                return
        st.phase = {'ACCEPTED':'Running','DONE':'Idle','FAILURE':'Error','TERMINATE':'Idle'}[result]
        if result != 'ACCEPTED':
            self._bubble(node.path, {'DONE':'SUCCESS','FAILURE':'FAILURE','TERMINATE':'TERMINATED'}[result])

    def _begin_abort_handler(self, node: Node, *, report_to_parent: bool) -> str:
        """Run handler after original action is known stopped.

        The initial conformance subset allows an async handler or immediate
        failure. Synchronous successful completion during a public abort call
        requires resolving the event/reply ordering in spec v0.4 O5.
        """
        st = self.state[node.path]
        outcome = self._trigger(node.children[1])
        if outcome == 'ACCEPTED':
            st.phase = 'Handling'
            return 'HANDLING'
        if outcome == 'FAILURE':
            st.phase = 'Error'
            if report_to_parent:
                self._bubble(node.path, 'FAILURE')
            return 'FAILURE'
        st.phase = 'Idle'
        event = 'TERMINATED' if outcome == 'TERMINATE' else 'SUCCESS'
        if report_to_parent:
            self._bubble(node.path, event)
            return 'SUCCESS'
        # For public aborts we need a protocol choice when synchronous handler
        # completion causes an observable completion and an abort reply.
        return 'SUCCESS' 

    def _abort(self, node: Node) -> str:
        st = self.state[node.path]
        if st.phase not in ("Running", "Handling"):
            raise ModelError(f"{node.path}: abort requires Running/Handling, got {st.phase}")
        if node.kind == "call":
            result = self._choose([*([self._mock(node).get("abort", "SUCCESS")] if isinstance(self._mock(node).get("abort", "SUCCESS"), str) else self._mock(node)["abort"])])
            self._emit(f"child.{node.path}.abort")
            self._emit(f"child.{node.path}.abort_reply.{result}")
            st.phase = "Idle" if result == "SUCCESS" else "Error"
            return result
        if node.kind in ('on_abort','on_error') and st.phase == 'Handling':
            result = self._abort(node.children[1])
            st.phase = {'SUCCESS':'Idle','FAILURE':'Error','HANDLING':'Handling'}[result]
            return result
        if node.kind == 'on_error':
            result = self._abort(node.children[0])
            st.phase = {'SUCCESS':'Idle','FAILURE':'Error','HANDLING':'Handling'}[result]
            return result
        if node.kind == 'on_abort':
            stopped = self._abort(node.children[0])
            if stopped == 'FAILURE':
                st.phase = 'Error'
                return 'FAILURE'
            if stopped == 'HANDLING':
                raise ModelError('nested asynchronous abort handler not supported yet')
            return self._begin_abort_handler(node, report_to_parent=False)
        if node.kind == 'flow_call':
            result = self._abort(node.children[0])
        elif node.kind in ('sequence', 'within', 'choose', 'either'):
            current = node.children[st.current]
            # A shared capability abort may have stopped the child before a
            # parent cleanup call reaches it. Cancellation is idempotent for
            # this narrow observation case (there is no live execution left).
            if self.state[current.path].phase not in ('Running','Handling'):
                result = 'SUCCESS'
            else:
                result = self._abort(current)
        elif node.kind in ('repeat','retry'):
            child = node.children[0]
            deferred = ('repeat_scheduled' if node.kind == 'repeat'
                        else 'retry_scheduled')
            if st.waiting == deferred and self.state[child.path].phase == 'Idle':
                # No child is running between iterations/attempts. External
                # abort cancels the scheduling obligation; the stale pending
                # event cannot restart the cancelled loop.
                st.waiting = ''
                result = 'SUCCESS'
            else:
                result = self._abort(child)
        else:
            results = [self._abort(ch) for ch in node.children if self.state[ch.path].phase in ('Running','Handling')]
            result = 'FAILURE' if 'FAILURE' in results else 'HANDLING' if 'HANDLING' in results else 'SUCCESS' 
        st.phase = {'SUCCESS':'Idle','FAILURE':'Error','HANDLING':'Handling'}[result]
        return result

    def _reset(self, node: Node) -> str:
        st = self.state[node.path]
        if st.phase != "Error":
            raise ModelError(f"{node.path}: reset requires Error, got {st.phase}")
        if node.kind == "call":
            result = self._choose([*([self._mock(node).get("reset", "SUCCESS")] if isinstance(self._mock(node).get("reset", "SUCCESS"), str) else self._mock(node)["reset"])])
            self._emit(f"child.{node.path}.reset")
            self._emit(f"child.{node.path}.reset_reply.{result}")
            st.phase = "Idle" if result == "SUCCESS" else "Error"
            return result
        # General cleanup: reset only children in Error. No child may remain
        # Running in a cleanly constructed Error state for this subset.
        results = [self._reset(ch) for ch in node.children if self.state[ch.path].phase == "Error"]
        running = any(self.state[ch.path].phase == "Running" for ch in node.children)
        result = "SUCCESS" if not running and all(x == "SUCCESS" for x in results) else "FAILURE"
        st.phase = "Idle" if result == "SUCCESS" else "Error"
        return result

    def state_snapshot(self) -> dict[str, str]:
        return {p: st.phase for p, st in self.state.items()}

    def fingerprint(self) -> tuple:
        return (
            tuple((p, x.phase, x.generation, x.attempts, x.current,
                   tuple(sorted(x.completed)), x.iteration, x.waiting) for p, x in sorted(self.state.items())),
            tuple((x.source, x.generation, x.kind) for x in self.pending),
            tuple(self.decisions), self._choice_cursor,
        )

    def semantic_key(self) -> tuple:
        """Identity of a *stable*, quiescent macrostep-boundary configuration.

        Decisions, choice cursor, trace history, output buffer, and macrostep
        ordinal belong to the explorer, not to KODA's runtime state. Pending
        events are a MULTISET: dispatch may choose any event (no FIFO contract),
        but duplicate pending events still count independently.

        Generation and attempt counters deliberately remain exact. Collapsing
        them without a separately proved abstraction can hide stale events or
        change the behavior of legacy per-attempt mock scripts. Consequently,
        unrestricted repetition can yield an unbounded LTS.
        """
        if self._in_macrostep or self._broadcasts:
            raise ModelError('semantic_key requires a completed macrostep')
        return (
            tuple((path, runtime.phase, runtime.generation,
                   runtime.attempts, runtime.current,
                   tuple(sorted(runtime.completed)), runtime.iteration,
                   runtime.waiting)
                  for path, runtime in sorted(self.state.items())),
            tuple(sorted((p.source, p.generation, p.kind) for p in self.pending)),
        )

    def explore_runs(self, max_dispatches: int = 15, max_traces: int = 2000) -> list["Engine"]:
        """Explore all mock choices and event orders, within explicit bounds.

        Call on a fresh engine (without trigger); a previously triggered engine
        explores only choices reachable after its recorded prefix.
        """
        def alternatives(base: Engine, method, *args):
            work = [base]
            while work:
                original = work.pop()
                candidate = original.clone()
                try:
                    getattr(candidate, method)(*args)
                except ChoiceNeeded as need:
                    for i in reversed(range(need.options)):
                        next_base = original.clone()
                        next_base.decisions = candidate.decisions + [i]
                        work.append(next_base)
                else:
                    yield candidate

        outcomes = []
        cutoff = False
        initial = list(alternatives(self, 'trigger')) if not self.history else [self.clone()]
        frontier = [(engine,0) for engine in initial]
        while frontier:
            engine, depth = frontier.pop()
            if depth >= max_dispatches or not engine.pending:
                if depth >= max_dispatches and engine.pending:
                    cutoff = True
                outcomes.append(engine)
                if len(outcomes) >= max_traces:
                    cutoff = True  # Conservative: the frontier may be exhausted.
                    break
                continue
            visited_events = set()
            for i, evt in enumerate(engine.pending):
                identity = (evt.source, evt.generation, evt.kind)
                if identity in visited_events:
                    continue
                visited_events.add(identity)
                for candidate in alternatives(engine,'dispatch',i):
                    frontier.append((candidate,depth+1))
        self.last_cutoff = cutoff
        return outcomes

    def explore_abort_runs(self, max_dispatches: int = 8, max_traces: int = 2000) -> tuple[list["Engine"], bool]:
        """Explore ONE external task.abort at every reachable *stable* Running point.

        The initial trigger and every deferred capability completion are atomic.
        The abort is injected *between* macrosteps, never inside one. We stop
        immediately after the abort reply, so pending stale completions and
        later reset are outside this specific experiment.

        Returns (runs, cutoff_reached). These are concrete scenario traces,
        not a proof that the potentially unbounded reference LTS was exhausted.
        """
        if self.history:
            raise ModelError('explore_abort_runs requires a fresh engine')
        if max_dispatches < 0 or max_traces < 1:
            raise ValueError('exploration limits must be nonnegative and max_traces positive')

        def alternatives(base: Engine, method: str, *args):
            work = [base]
            while work:
                original = work.pop()
                candidate = original.clone()
                try:
                    getattr(candidate, method)(*args)
                except ChoiceNeeded as need:
                    for i in reversed(range(need.options)):
                        branch = original.clone()
                        branch.decisions = candidate.decisions + [i]
                        work.append(branch)
                else:
                    yield candidate

        initial = list(alternatives(self, 'trigger'))
        frontier = [(e, 0) for e in initial]
        results: list[Engine] = []
        cutoff = False
        while frontier:
            engine, dispatches = frontier.pop()
            if engine.state[engine.root.path].phase != 'Running':
                continue
            # Abort now, before any further independently pending event.
            for aborted in alternatives(engine, 'abort'):
                results.append(aborted)
                if len(results) >= max_traces:
                    return results, True  # Reached output cap; do not claim completeness.
            if dispatches >= max_dispatches:
                if engine.pending:
                    cutoff = True
                continue
            seen_events: set[tuple[str,int,str]] = set()
            for i, evt in enumerate(engine.pending):
                identity = (evt.source, evt.generation, evt.kind)
                if identity in seen_events:
                    continue
                seen_events.add(identity)
                for resumed in alternatives(engine, 'dispatch', i):
                    frontier.append((resumed, dispatches + 1))
        return results, cutoff

    def explore_abort_complete_runs(self, max_dispatches: int = 8,
                                    max_traces: int = 2000) -> tuple[list["Engine"], bool]:
        """One abort, followed by pending handler completions (not a second abort).

        This is separate from the legacy explore-abort operation, which stops
        at the synchronous abort reply to preserve existing comparison tests.
        """
        prefixes, cutoff = self.explore_abort_runs(max_dispatches, max_traces)
        results = []
        for prefix in prefixes:
            completions = prefix.explore_runs(max_dispatches, max_traces)
            cutoff = cutoff or prefix.last_cutoff
            for completed in completions:
                results.append(completed)
                if len(results) >= max_traces:
                    return results, True
        return results, cutoff

    def explore(self, max_dispatches: int = 15, max_traces: int = 2000) -> list[list[dict[str, Any]]]:
        """Legacy API: return macrostep histories of reachable bounded executions."""
        return [run.history for run in self.explore_runs(max_dispatches, max_traces)]
