"""Compare a single external task.abort() against a Dezyne system LTS.

Scope: one task.trigger(), then one task.abort() at a stable Running point.
The observation stops at the synchronous abort reply. This deliberately does
NOT claim equivalence of reset, post-abort notifications, or protocol states.

Unlike the older single-trigger comparators, Result:Success is decoded in the
context of the pending call: trigger=ACCEPTED; abort/reset=SUCCESS.
"""
from __future__ import annotations

import argparse
from collections import Counter, deque
from dataclasses import dataclass
import json
from pathlib import Path
from typing import Any

from .aut_compare import parse_aut, read_call_contracts_from_model, _CHILD_EVENT


@dataclass(frozen=True)
class Configuration:
    state: int
    prior: tuple[tuple[str, ...], ...]
    active: tuple[str, ...]
    running: bool
    abort_requested: bool
    task_operation: str | None
    child_operations: tuple[tuple[str, str], ...]
    hops: int


def project_reference_abort(trace: dict[str, Any]) -> tuple[tuple[str, ...], ...]:
    """Stable projection distinguishing trigger replies from abort replies."""
    groups: dict[int, list[str]] = {}
    for ev in trace.get('events', []):
        scope, kind, op = ev.get('scope'), ev.get('kind'), ev.get('operation')
        result = ev.get('result')
        token = None
        if scope == 'task':
            if kind == 'invoke' and op in ('trigger', 'abort'):
                token = f'task.{op}'
            elif kind == 'reply' and op in ('reply', 'abort_reply'):
                token = f'task.{op}.{result}'
            elif kind == 'event':
                token = f'task.event.{result}'
        elif scope == 'capability':
            name = ev.get('symbol')
            if not name:
                raise ValueError(f'Missing capability symbol: {ev}')
            if kind == 'invoke' and op in ('trigger','service','abort'):
                token = f'{name}.{op}'
            elif kind == 'reply' and op in ('reply', 'abort_reply'):
                token = f'{name}.{op}.{result}'
            elif kind == 'event':
                token = f'{name}.event.{result}'
        if token is not None:
            groups.setdefault(ev['macrostep'], []).append(token)
    return tuple(tuple(groups[n]) for n in sorted(groups))


def explore_aut_abort(lts, *, async_actions: set[str], service_actions: set[str],
                      max_hops: int = 100, max_configurations: int = 150_000,
                      max_pre_abort_steps: int = 3) -> tuple[set[tuple[tuple[str, ...], ...]], dict[str,int], bool]:
    """Enumerate projected AUT paths ending at the first external abort reply.

    Restrict only known-invalid replies using the KODA contracts. All unknown
    or unrecognized labels are counted explicitly and make coverage incomplete.
    Macrostep boundaries are inferred, not provided by AUT.
    """
    if async_actions & service_actions:
        raise ValueError('Operation declared both asynchronous and synchronous')
    if max_hops < 1 or max_configurations < 1 or max_pre_abort_steps < 0:
        raise ValueError('Exploration limits must be positive')
    adjacency = lts.successors()
    initial = [Configuration(nxt, (), ('task.trigger',), False, False, 'trigger', (), 1)
               for label, nxt in adjacency.get(lts.initial, ()) if label == 'api.trigger']
    if not initial:
        raise ValueError('No initial api.trigger in Dezyne LTS')
    queue = deque(initial)
    seen: set[tuple] = set()
    traces: set[tuple[tuple[str,...],...]] = set()
    omitted: Counter[str] = Counter()
    cutoff = False

    while queue:
        cfg = queue.popleft()
        # Compare observable paths, not path length: several tau transitions
        # may lead to the same semantic projection.
        key = (cfg.state, cfg.prior, cfg.active, cfg.running,
               cfg.abort_requested, cfg.task_operation, cfg.child_operations)
        if key in seen:
            continue
        seen.add(key)
        if len(seen) > max_configurations or cfg.hops >= max_hops:
            cutoff = True
            if len(seen) > max_configurations:
                break
            continue

        for label, nxt in adjacency.get(cfg.state, ()):
            prior, active = cfg.prior, cfg.active
            running, abort_requested = cfg.running, cfg.abort_requested
            task_op = cfg.task_operation
            child_ops = dict(cfg.child_operations)
            end = False
            token = None
            child = _CHILD_EVENT.fullmatch(label)

            if label in ('tau','api.optional') or label.endswith('.<flush>') or (child and child.group(2) == 'optional'):
                pass
            elif label in ('<illegal>', '<missing-reply>'):
                omitted['invalid Dezyne protocol edge'] += 1
                continue
            elif label == 'api.abort':
                if not running or abort_requested or task_op is not None:
                    omitted['abort outside stable Running state'] += 1
                    continue
                abort_requested, task_op = True, 'abort'
                prior = prior + (active,)
                active = ('task.abort',)
            elif label in ('api.trigger','api.reset'):
                omitted['other top-level invocation excluded'] += 1
                continue
            elif label.startswith('api.Result:'):
                result = label.split(':',1)[1]
                if task_op == 'trigger':
                    token = 'task.reply.' + {'Success':'ACCEPTED','Done':'DONE','Failure':'FAILURE'}.get(result, f'UNEXPECTED_{result}')
                    if result != 'Success':
                        # No Running task: external abort is not a legal scenario.
                        omitted['task did not enter Running'] += 1
                        continue
                    running, task_op = True, None
                elif task_op == 'abort':
                    token = 'task.abort_reply.' + result.upper()
                    task_op = None
                    end = True
                else:
                    omitted['task result without matching request'] += 1
                    cutoff = True
                    continue
            elif label in ('api.success','api.failure','api.aborted'):
                token = 'task.event.' + label.rsplit('.',1)[1].upper()
                if not abort_requested:
                    # Task finished without the external abort scenario.
                    omitted['task completed before external abort'] += 1
                    continue
            elif child:
                name, operation = child.groups()
                if operation in ('trigger', 'abort', 'reset'):
                    if name in child_ops:
                        omitted['overlapping child operation'] += 1
                        cutoff = True
                        continue
                    child_ops[name] = operation
                    token = name + ('.service' if operation == 'trigger' and name in service_actions
                                    else '.' + operation)
                elif operation.startswith('Result:'):
                    result = operation.partition(':')[2]
                    pending = child_ops.pop(name, None)
                    if pending is None:
                        omitted['child result without matching request'] += 1
                        cutoff = True
                        continue
                    if pending == 'trigger':
                        if name in async_actions and result == 'Done':
                            omitted['async action DONE excluded'] += 1
                            continue
                        if name in service_actions and result in ('Success','Running'):
                            omitted['sync service ACCEPTED/Running excluded'] += 1
                            continue
                        out = {'Success':'ACCEPTED','Done':'DONE','Failure':'FAILURE'}.get(result, result.upper())
                        token = f'{name}.reply.{out}'
                    else:
                        token = f'{name}.{pending}_reply.{result.upper()}'
                elif operation in ('success','failure','aborted'):
                    token = f'{name}.event.{operation.upper()}'
                    # In Dezyne, aborted may be emitted as part of abort().
                    # Such an event belongs to that macrostep, not a new one.
                    if not abort_requested and child_ops.get(name) != 'abort':
                        if len(prior) >= max_pre_abort_steps:
                            omitted['pre-abort macrostep bound'] += 1
                            cutoff = True
                            continue
                        prior = prior + (active,)
                        active = ()
                else:
                    omitted['unmapped child event: ' + label] += 1
                    cutoff = True
                    continue
            else:
                omitted['unmapped AUT label: ' + label] += 1
                cutoff = True
                continue

            if token is not None:
                active = active + (token,)
            if end:
                traces.add(prior + (active,))
                continue
            queue.append(Configuration(nxt,prior,active,running,abort_requested,
                                       task_op,tuple(sorted(child_ops.items())),cfg.hops+1))

    return traces, dict(sorted(omitted.items())), cutoff


def _project_dezyne_abort_ack(trace: tuple[tuple[str, ...], ...]) -> tuple[tuple[str, ...], ...]:
    """Optionally treat the redundant api.aborted emitted during a successful
    explicit task.abort() as Dezyne protocol acknowledgement rather than a
    separate public KODA completion event.

    The projection is deliberately narrow: task.event.ABORTED is retained for
    spontaneous aborts, failed aborts, HANDLING replies, and later events.
    """
    steps = []
    for step in trace:
        if (step and step[0] == 'task.abort'
                and step[-1] == 'task.abort_reply.SUCCESS'
                and step.count('task.event.ABORTED') == 1):
            step = tuple(token for token in step if token != 'task.event.ABORTED')
        steps.append(step)
    return tuple(steps)


def compare_abort(aut: Path, reference: Path, *, async_actions: set[str], service_actions: set[str],
                  max_hops: int = 100, max_configurations: int = 150_000,
                  max_pre_abort_steps: int = 3,
                  abort_notification_policy: str = 'strict') -> dict[str, Any]:
    data = json.loads(reference.read_text(encoding='utf-8'))
    if data.get('schema') != 'koda.reference.exploration.v1' or data.get('scenario') not in ('one_external_abort', 'explore-abort'):
        raise ValueError('Expected an explore-abort reference file')
    expected = {project_reference_abort(trace) for trace in data['traces']}
    if any(not t[-1][-1].startswith('task.abort_reply.') for t in expected):
        raise ValueError('Expected traces ending with task.abort_reply')
    lts = parse_aut(aut)
    actual, omitted, cutoff = explore_aut_abort(lts,async_actions=async_actions,service_actions=service_actions,
                                               max_hops=max_hops,max_configurations=max_configurations,
                                               max_pre_abort_steps=max_pre_abort_steps)
    if abort_notification_policy not in ('strict', 'dezyne_ack'):
        raise ValueError(f'Unsupported abort notification policy: {abort_notification_policy}')
    projected_acks = 0
    if abort_notification_policy == 'dezyne_ack':
        projected = {_project_dezyne_abort_ack(t) for t in actual}
        projected_acks = sum(_project_dezyne_abort_ack(t) != t for t in actual)
        actual = projected
    cutoff = cutoff or bool(data.get('cutoff_reached', False))
    missing, extra = expected-actual, actual-expected
    def serial(items):
        return [[list(step) for step in trace] for trace in sorted(items)]
    # An extra AUT trace is a witnessed difference under the stated projection,
    # even if exhaustive coverage was cut short.
    if extra:
        result = 'mismatch_observed_with_cutoff' if cutoff else 'mismatch'
    elif cutoff:
        result = 'inconclusive_cutoff'
    elif missing:
        result = 'mismatch'
    else:
        result = 'match_under_assumptions'
    shortest = lambda ts: [list(step) for step in min(ts, key=lambda t:(sum(map(len,t)),t))] if ts else None
    return {
        'schema':'koda.reference.abort-comparison.v1',
        'scenario':'one_external_abort',
        'aut':{'states':lts.declared_states,'transitions':lts.declared_transitions},
        'counts':{'reference':len(expected),'dezyne':len(actual),'matched':len(expected & actual),
                  'reference_only':len(missing),'dezyne_only':len(extra)},
        'result':result,
        'abort_notification_policy': abort_notification_policy,
        'projected_dezyne_ack_traces': projected_acks,
        'shortest_dezyne_only':shortest(extra),
        'shortest_reference_only':shortest(missing),
        'reference_only':serial(missing),'dezyne_only':serial(extra),
        'omitted':omitted,'cutoff_reached':cutoff,
        'assumptions':[
            'one external abort at a stable Running point, no reset',
            'stop observing at the immediate synchronous abort reply',
            'async action DONE and sync service ACCEPTED excluded by declared KODA contracts',
            'Dezyne step boundaries inferred from trigger, capability completion, external abort',
            ('Dezyne api.aborted inside successful explicit task.abort is treated as a protocol acknowledgement'
             if abort_notification_policy == 'dezyne_ack'
             else 'Dezyne aborted events are retained, including those emitted inside abort()')
        ],
        'limitations':[
            'Graph simulator includes environment nondeterminism; checks are conditional',
            'No comparison of post-abort stale events, late api.aborted, or reset',
            'AUT lacks explicit macrostep ids; inferred boundaries are hypotheses',
            'Finite path exploration can reach a cutoff; missing traces then prove nothing',
            'A witnessed extra trace is evidence only if the normalization/protocol assumptions are valid'
        ]
    }


def main(argv: list[str] | None = None) -> int:
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--aut', type=Path, required=True)
    p.add_argument('--reference', type=Path, required=True, help='Output of --mode explore-abort')
    p.add_argument('--model', type=Path, help='Emitter model; auto-discovered beside exploration by default')
    p.add_argument('--mocks', type=Path, help='Capability outcome fixture; auto-discovers override/default-mocks sidecar')
    p.add_argument('--async-action', action='append', default=[])
    p.add_argument('--service-action', action='append', default=[])
    p.add_argument('--abort-notification-policy', choices=('strict', 'dezyne_ack'),
                   default='strict',
                   help='strict keeps all Dezyne task events; dezyne_ack projects only api.aborted during successful task.abort')
    p.add_argument('--max-hops', type=int, default=100)
    p.add_argument('--max-configurations', type=int, default=150_000)
    p.add_argument('--max-pre-abort-steps', type=int, default=3)
    p.add_argument('--output', type=Path)
    a = p.parse_args(argv)
    from .contracts import resolve_contracts
    contracts = resolve_contracts(a.reference, model=a.model, mocks=a.mocks,
                                  explicit_async=a.async_action, explicit_services=a.service_action)
    report = compare_abort(a.aut,a.reference,
                           async_actions=set(contracts.async_actions),
                           service_actions=set(contracts.service_actions),
                           max_hops=a.max_hops,max_configurations=a.max_configurations,
                           max_pre_abort_steps=a.max_pre_abort_steps,
                           abort_notification_policy=a.abort_notification_policy)
    report['call_contracts'] = {
        'source': contracts.source,
        'model': str(contracts.model_path) if contracts.model_path else None,
        'mocks': str(contracts.mocks_path) if contracts.mocks_path else None,
        'async_actions': sorted(contracts.async_actions),
        'sync_services': sorted(contracts.service_actions),
    }
    print(f"Abort macrostep traces: {report['counts']}")
    print(f"Result: {report['result']} (cutoff={report['cutoff_reached']})")
    if report['shortest_dezyne_only']:
        print('Shortest Dezyne-only trace:')
        for i, step in enumerate(report['shortest_dezyne_only']):
            print(f'  [{i}] ' + ' -> '.join(step))
    if a.output:
        a.output.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
    return 0 if report['result']=='match_under_assumptions' else 1


if __name__ == '__main__':
    raise SystemExit(main())
