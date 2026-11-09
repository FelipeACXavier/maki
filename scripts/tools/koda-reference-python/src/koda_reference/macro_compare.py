"""Compare per-macrostep observable behavior, under an explicit scheduling assumption.

Dezyne AUT labels do not encode macrostep numbers. We *infer* a boundary when
an external input (task.trigger or capability completion event) arrives.
Within that boundary, events, synchronous calls and their replies are atomic.
This is a diagnostic abstraction, not a proof of Dezyne scheduling semantics.
"""
from __future__ import annotations

import argparse
from collections import Counter, deque
import json
from pathlib import Path
from typing import Any

from .aut_compare import parse_aut, normalize_aut, read_call_contracts_from_model, _ALLOWED_TERMINALS


def reference_steps(trace: dict[str, Any]) -> tuple[tuple[str, ...], ...]:
    grouped: dict[int, list[str]] = {}
    for ev in trace.get('events', []):
        scope, kind, op, result = ev.get('scope'), ev.get('kind'), ev.get('operation'), ev.get('result')
        token: str | None = None
        if scope == 'task':
            if kind == 'invoke' and op in ('trigger','abort'):
                token = 'task.'+op
            elif kind == 'reply':
                token = f'task.{op if op in ("abort_reply", "reset_reply") else "reply"}.{result}'
            elif kind == 'event':
                token = f'task.event.{result}'
        elif scope == 'capability':
            # An abort_call node is an internal adapter invocation (cabort_call
            # -> capability armour iabort), not an external capability event.
            # The actual abort of a Running action, if any, remains observable.
            if ev.get('call_kind') == 'abort_call':
                continue
            symbol = ev.get('symbol')
            if not symbol:
                raise ValueError(f'Capability event lacks symbol: {ev}')
            if kind == 'invoke' and op in ('trigger', 'service','abort','reset'):
                token = f'{symbol}.{op}'
            elif kind in ('reply','event'):
                reply_kind = op if op in ('abort_reply','reset_reply') else kind
                token = f'{symbol}.{reply_kind}.{result}'
        if token is not None:
            grouped.setdefault(ev['macrostep'], []).append(token)
    return tuple(tuple(grouped[key]) for key in sorted(grouped))


def aut_steps(lts, *, async_actions: set[str], max_hops: int = 100,
              max_configurations: int = 100_000, service_actions: set[str] | None = None):
    adj = lts.successors()
    service_actions = service_actions or set()
    if async_actions & service_actions:
        raise ValueError('An operation cannot be both async_action and sync_service')
    starts = [nxt for label, nxt in adj.get(lts.initial, []) if label == 'api.trigger']
    if not starts:
        raise ValueError('No initial api.trigger in AUT')
    # Flattened current macrostep, completed macrosteps, source state, hops.
    queue = deque((s, (), ('task.trigger',), 1, ()) for s in starts)
    visited = set()
    completed = set()
    omissions = Counter()
    truncated = False
    while queue:
        state, prior, active, hops, ops = queue.popleft()
        key = (state, prior, active, ops)
        if key in visited:
            continue
        visited.add(key)
        if len(visited) >= max_configurations or hops >= max_hops:
            truncated = True
            continue
        for label, nxt in adj.get(state, []):
            token = normalize_aut(label)
            next_ops = dict(ops)
            # Some Dezyne service adapters expose a synchronous In invocation
            # through a generic action-style trigger label. Enable only after
            # inspecting the actual service AUT labels.
            if token is not None and token.endswith('.trigger') and token[:-len('.trigger')] in service_actions:
                token = token[:-len('.trigger')] + '.service'
            if token is None:
                if nxt != state:
                    queue.append((nxt, prior, active, hops+1, ops))
                continue
            if token in ('INTERNAL_DEFER', 'INTERNAL_TIMEOUT'):
                # Deferred callbacks and timer timeouts both begin new
                # run-to-completion steps. Neither label is a KODA observable;
                # subsequent invocations/replies are assigned to the new step.
                queue.append((nxt, prior + ((active,) if active else ()),
                              (), hops+1, ops))
                continue
            # A Dezyne Result:Success has meaning only relative to the
            # outstanding call. In particular, abort Success != trigger ACCEPTED.
            if token is not None:
                if token.endswith(('.trigger', '.service', '.abort', '.reset')) and not token.startswith('task.'):
                    name, operation = token.rsplit('.', 1)
                    next_ops[name] = operation
                elif '.reply.' in token and not token.startswith('task.'):
                    name, suffix = token.rsplit('.reply.', 1)
                    pending = next_ops.pop(name, None)
                    if pending in ('abort', 'reset'):
                        mapping = {'ACCEPTED':'SUCCESS', 'UNRECOGNIZED(Running)':'RUNNING',
                                   'UNRECOGNIZED(Error)':'ERROR'}
                        token = f'{name}.{pending}_reply.{mapping.get(suffix,suffix)}'
            
            if token in ('EXTERNAL_OTHER_OPERATION', 'INVALID_PROTOCOL', 'task.trigger'):
                omissions[token] += 1
                continue
            if token.startswith('UNMAPPED(') or 'UNRECOGNIZED(' in token:
                omissions['unknown: '+label] += 1
                truncated = True
                continue
            if token.endswith('.reply.DONE'):
                action = token.rsplit('.reply.', 1)[0]
                if action in async_actions:
                    omissions['async DONE'] += 1
                    continue
            if token.endswith(('.reply.ACCEPTED', '.reply.UNRECOGNIZED(Running)')):
                action = token.rsplit('.reply.', 1)[0]
                if action in service_actions:
                    omissions['sync service ACCEPTED/Running'] += 1
                    continue
            # An asynchronous capability completion is an *external input*.
            # It begins another macrostep, even if Dezyne's LTS has intervening
            # simulator transitions (tau/optional) between the previous reply and it.
            is_external_completion = (token.endswith(('.event.SUCCESS','.event.FAILURE','.event.ABORTED'))
                                      and not token.startswith('task.')
                                      and next_ops.get(token.rsplit('.event.',1)[0]) != 'abort')
            next_prior = prior + (active,) if is_external_completion else prior
            next_active = (token,) if is_external_completion else active + (token,)
            if token in _ALLOWED_TERMINALS:
                completed.add(next_prior + (next_active,))
            else:
                queue.append((nxt, next_prior, next_active, hops+1, tuple(sorted(next_ops.items()))))
    return completed, dict(omissions), truncated


def compare_macrosteps(aut: Path, reference: Path, *, async_actions: set[str], max_hops: int = 100,
                       service_actions: set[str] | None = None) -> dict[str, Any]:
    lts = parse_aut(aut)
    ref = json.loads(reference.read_text(encoding='utf-8'))
    if ref.get('schema') != 'koda.reference.exploration.v1':
        raise ValueError('Expected koda.reference.exploration.v1')
    expected = {reference_steps(trace) for trace in ref['traces']}
    reference_cutoff = ref.get('cutoff_reached') is True
    actual, omitted, cutoff = aut_steps(lts, async_actions=async_actions, max_hops=max_hops, service_actions=service_actions)
    def serial(items):
        return [[list(step) for step in trace] for trace in sorted(items)]
    return {
        'schema': 'koda.reference.macro-comparison.v1',
        'service_action_label_overrides': sorted(service_actions or ()),
        'boundary_method': 'inferred from task.trigger, capability completions, Dezyne <defer>, and timer timeout dispatch',
        'assumptions': ['one top-level trigger, no external abort/reset',
                        'capability completion is the start of its own Dezyne run-to-completion step',
                        'tau and optional labels are hidden; <defer> and timer timeouts start new macrosteps',
                        'async action DONE excluded using provided contract',
                        'sync service ACCEPTED/Running excluded using provided contract'],
        'counts': {'reference':len(expected), 'dezyne':len(actual), 'matched':len(actual & expected),
                   'reference_only':len(expected-actual), 'dezyne_only':len(actual-expected)},
        'reference_only':serial(expected-actual), 'dezyne_only':serial(actual-expected),
        'omitted': omitted,
        'cutoff_reached': cutoff or reference_cutoff,
        'reference_cutoff_reached': reference_cutoff,
        'dezyne_cutoff_reached': cutoff,
        'result': 'match_under_assumptions' if actual == expected and not (cutoff or reference_cutoff) else
                  'inconclusive_cutoff' if (cutoff or reference_cutoff) else 'mismatch',
        'limitations': ['AUT lacks explicit macrostep IDs; boundaries are inferred, not directly observed',
                        'No external abort/reset comparison',
                        'Finite bounded exploration; reference completeness is not established merely by its bounded flag',
                        'Service trigger labels normalized under explicit sync_service declarations; generated label conventions must be checked for other Dezyne versions',
                        'Within timeout ordering is abstract/nondeterministic; neither model verifies elapsed wall-clock milliseconds'],
    }


def main(argv=None) -> int:
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--aut', type=Path, required=True)
    p.add_argument('--reference', type=Path, required=True)
    p.add_argument('--model', type=Path, help='Emitter model; auto-discovered beside the exploration by default')
    p.add_argument('--mocks', type=Path, help='Capability outcome fixture; auto-discovers override/default-mocks sidecar')
    p.add_argument('--async-action', action='append', default=[])
    p.add_argument('--service-action', action='append', default=[], help='Map confirmed Dezyne .trigger labels to .service for these INSTANCE.EVENT names')
    p.add_argument('--max-hops', type=int, default=100)
    p.add_argument('--output', type=Path)
    a = p.parse_args(argv)
    from .contracts import resolve_contracts
    contracts = resolve_contracts(a.reference, model=a.model, mocks=a.mocks,
                                  explicit_async=a.async_action, explicit_services=a.service_action)
    actions = set(contracts.async_actions)
    services = set(contracts.service_actions)
    report = compare_macrosteps(a.aut, a.reference, async_actions=actions, max_hops=a.max_hops, service_actions=services)
    report['call_contracts'] = {
        'source': contracts.source,
        'model': str(contracts.model_path) if contracts.model_path else None,
        'mocks': str(contracts.mocks_path) if contracts.mocks_path else None,
        'async_actions': sorted(contracts.async_actions),
        'sync_services': sorted(contracts.service_actions),
    }
    print(f"Macrostep traces: {report['counts']}\nResult: {report['result']}")
    if a.output:
        a.output.write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8')
    return 0 if report['result'] == 'match_under_assumptions' else 1


if __name__ == '__main__':
    raise SystemExit(main())
