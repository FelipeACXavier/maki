"""Independent, minimal KODA subset tree and capability-event contract.

JSON schema v0.2: program, flows, capabilities -> instances -> events.
No source parser or compiler code is reused.
"""
from __future__ import annotations
from dataclasses import dataclass
from typing import Any

class ModelError(ValueError):
    pass

@dataclass(frozen=True)
class Node:
    kind: str
    path: str
    children: tuple['Node', ...] = ()
    capability: str | None = None
    event: str | None = None
    flow: str | None = None
    argument_count: int = 0
    interval_ms: int = 0
    iterations: int = 0
    guard_expressions: tuple[dict | None, ...] = ()


def parse_node(value: Any, path: str = 'root', flows: dict | None = None,
               stack: tuple[str, ...] = ()) -> Node:
    """Parse the compiler's control-flow projection; no data or backend semantics inferred."""
    if not isinstance(value, dict):
        raise ModelError(f'{path}: expected an object')
    kinds = {'call', 'capability_event', 'service_call', 'abort_call', 'flow_call',
             'sequence', 'parallel', 'either', 'choose', 'repeat', 'retry', 'within',
             'on_abort', 'on_error', 'success', 'failure', 'continue'}
    present = kinds.intersection(value)
    if len(present) != 1 or set(value) != present:
        raise ModelError(f'{path}: expected exactly one of {sorted(kinds)}')
    kind = next(iter(present))
    if kind in ('success', 'failure', 'continue'):
        if value[kind] != {}:
            raise ModelError(f'{path}: {kind} must have an empty object')
        return Node(kind, path)
    if kind in ('on_abort', 'on_error'):
        parts = value[kind]
        if not isinstance(parts, dict) or set(parts) != {'action', 'handler'}:
            raise ModelError(f'{path}: {kind} requires action and handler')
        primary = parse_node(parts['action'], path + '.action', flows, stack)
        handler = parse_node(parts['handler'], path + '.handler', flows, stack)
        return Node(kind, path, (primary, handler))
    if kind == 'call':
        name = value['call']
        if not isinstance(name, str) or not name:
            raise ModelError(f'{path}: call must be a nonempty name')
        return Node('call', path, capability=name)
    if kind in ('capability_event', 'service_call', 'abort_call'):
        ref = value[kind]
        allowed = {'instance', 'event', 'argument_count'} if kind != 'abort_call' else {'instance', 'event'}
        if not isinstance(ref, dict) or not {'instance', 'event'} <= set(ref) or set(ref) - allowed:
            raise ModelError(f'{path}: invalid {kind} declaration')
        argc = ref.get('argument_count', 0)
        if type(argc) is not int or argc < 0:
            raise ModelError(f'{path}: invalid argument_count')
        instance, event = ref['instance'], ref['event']
        if not isinstance(instance, str) or not instance or not isinstance(event, str) or not event:
            raise ModelError(f'{path}: instance and event must be nonempty strings')
        return Node({'capability_event': 'call', 'service_call': 'service', 'abort_call': 'abort_call'}[kind],
                    path, capability=instance, event=event, argument_count=argc)
    if kind == 'flow_call':
        ref = value[kind]
        if not isinstance(ref, dict) or set(ref) != {'name'} or not isinstance(ref['name'], str):
            raise ModelError(f'{path}: flow_call requires name')
        name = ref['name']
        if flows is None or name not in flows:
            raise ModelError(f'{path}: unknown flow {name!r}')
        if name in stack:
            raise ModelError(f'{path}: recursive flow call currently unsupported: {stack + (name,)}')
        body = parse_node(flows[name], f'{path}.body', flows, stack + (name,))
        return Node('flow_call', path, (body,), flow=name)
    if kind in ('repeat', 'retry', 'within'):
        details = value[kind]
        if not isinstance(details, dict):
            raise ModelError(f'{path}: {kind} needs an object')
        if kind == 'within':
            required = {'timeout_ms', 'body', 'else'}
            if set(details) != required:
                raise ModelError(f'{path}: within requires {required}')
            milliseconds = details['timeout_ms']
            if type(milliseconds) is not int or milliseconds < 0:
                raise ModelError(f'{path}: invalid timeout_ms')
            return Node('within', path,
                        (parse_node(details['body'], path + '.body', flows, stack),
                         parse_node(details['else'], path + '.else', flows, stack)), interval_ms=milliseconds)
        required = {'interval_ms', 'iterations', 'body'} if kind == 'repeat' else {'attempts', 'body'}
        if set(details) != required:
            raise ModelError(f'{path}: {kind} requires {required}')
        count = details['iterations'] if kind == 'repeat' else details['attempts']
        milliseconds = details['interval_ms'] if kind == 'repeat' else 0
        if type(count) is not int or count < 0 or type(milliseconds) is not int or milliseconds < 0:
            raise ModelError(f'{path}: invalid loop count or interval')
        if kind == 'retry' and count == 0:
            # Zero-attempt Retry is explicitly undefined in semantics v0.4;
            # reject it rather than silently executing the body once.
            raise ModelError(f'{path}: retry attempts must be at least 1')
        return Node(kind, path, (parse_node(details['body'], path + '.body', flows, stack),),
                    iterations=count, interval_ms=milliseconds)
    if kind == 'choose':
        items = value['choose']
        if not isinstance(items, list) or not items:
            raise ModelError(f'{path}: choose must have nonempty options')
        children = []
        guards = []
        default_seen = False
        for i, item in enumerate(items):
            if not isinstance(item, dict):
                raise ModelError(f'{path}.{i}: invalid choose option')
            if set(item) == {'default'}:
                if i != len(items)-1 or default_seen:
                    raise ModelError(f'{path}: default must be last and unique')
                default_seen = True
                children.append(parse_node(item['default'], f'{path}.{i}', flows, stack))
                guards.append(None)
            elif set(item) == {'when','body'} and not default_seen:
                if not isinstance(item['when'], dict):
                    raise ModelError(f'{path}.{i}: guard must be an expression object')
                children.append(parse_node(item['body'], f'{path}.{i}', flows, stack))
                guards.append(item['when'])
            else:
                raise ModelError(f'{path}.{i}: invalid choose option')
        return Node('choose', path, tuple(children), guard_expressions=tuple(guards))
    children = value[kind]
    if not isinstance(children, list) or not children:
        raise ModelError(f'{path}: {kind} requires a nonempty list')
    return Node(kind, path, tuple(parse_node(ch, f'{path}.{i}', flows, stack) for i, ch in enumerate(children)))


def _options(value: Any, label: str, allowed: tuple[str,...]) -> list[str]:
    if isinstance(value, str):
        value = [value]
    if not isinstance(value, list) or not value or any(v not in allowed for v in value):
        raise ModelError(f'{label}: expected nonempty list of {allowed}')
    return value


def _validate_mock(cfg: dict, name: str, *, action_kind: str | None = None) -> None:
    if not isinstance(cfg, dict):
        raise ModelError(f'{name}: mock must be an object')
    if 'outcomes' in cfg:
        scenarios = cfg['outcomes']
        if not isinstance(scenarios, list) or not scenarios:
            raise ModelError(f'{name}.outcomes must be nonempty')
        for outcome in scenarios:
            if not isinstance(outcome, dict) or outcome.get('reply') not in ('DONE', 'ACCEPTED', 'FAILURE'):
                raise ModelError(f'{name}: invalid outcome')
            if outcome['reply'] == 'ACCEPTED':
                if outcome.get('event') not in ('SUCCESS', 'FAILURE', 'ABORTED'):
                    raise ModelError(f'{name}: ACCEPTED requires completion event')
            elif 'event' in outcome:
                raise ModelError(f'{name}: only ACCEPTED may carry completion event')
            if action_kind == 'async_action' and outcome['reply'] == 'DONE':
                raise ModelError(f'{name}: async actions cannot return DONE')
            if action_kind in ('sync_action', 'sync_service') and outcome['reply'] == 'ACCEPTED':
                raise ModelError(f'{name}: synchronous actions cannot return ACCEPTED')
            if set(outcome) - {'reply','event'}:
                raise ModelError(f'{name}: unsupported outcome fields')
    elif 'trigger' in cfg:  # legacy fixed per-attempt scripts
        scenarios = cfg['trigger']
        if not isinstance(scenarios, list) or not scenarios:
            raise ModelError(f'{name}.trigger must be a nonempty list')
        for outcome in scenarios:
            _validate_mock({'outcomes':[outcome]},name,action_kind=action_kind)
    else:
        raise ModelError(f'{name}: requires outcomes (or legacy trigger)')
    if action_kind == 'sync_service' and ('abort' in cfg or 'reset' in cfg):
        raise ModelError(f'{name}: service mocks cannot declare abort/reset')
    for op in (() if action_kind == 'sync_service' else ('abort', 'reset')):
        _options(cfg.get(op, 'SUCCESS'), f'{name}.{op}', ('SUCCESS', 'FAILURE'))


def validate_scenario(data: Any) -> tuple[Node, dict[str,dict]]:
    if not isinstance(data,dict) or not isinstance(data.get('capabilities'),dict) or 'program' not in data:
        raise ModelError('scenario requires program and capabilities')
    flows = data.get('flows',{})
    if not isinstance(flows,dict):
        raise ModelError('flows must be an object')
    root = parse_node(data['program'], flows=flows)
    caps = data['capabilities']
    for instance, cfg in caps.items():
        if not isinstance(cfg,dict):
            raise ModelError(f'{instance}: invalid capability definition')
        if 'events' in cfg:
            if not isinstance(cfg['events'],dict):
                raise ModelError(f'{instance}.events must be an object')
            abort_events = cfg.get('abort_events', [])
            if not isinstance(abort_events, list) or any(not isinstance(x,str) or not x for x in abort_events) or len(abort_events) != len(set(abort_events)):
                raise ModelError(f'{instance}.abort_events must contain unique names')
            kinds = cfg.get('event_kinds', {})
            if not isinstance(kinds, dict):
                raise ModelError(f'{instance}.event_kinds must be an object')
            if kinds and set(kinds) != set(cfg['events']):
                raise ModelError(f'{instance}.event_kinds does not match declared events')
            for ev, mock in cfg['events'].items():
                kind = kinds.get(ev)
                if kind is not None and kind not in ('async_action', 'sync_action', 'sync_service'):
                    raise ModelError(f'{instance}.{ev}: unknown action kind {kind!r}')
                _validate_mock(mock, f'{instance}.{ev}', action_kind=kind)
        else:
            _validate_mock(cfg,instance)  # legacy only
    def check(n:Node):
        if n.kind in ('call', 'service', 'abort_call'):
            if n.capability not in caps:
                raise ModelError(f'{n.path}: unknown capability instance {n.capability}')
            cfg = caps[n.capability]
            if n.kind != 'abort_call' and n.event is not None and ('events' not in cfg or n.event not in cfg['events']):
                raise ModelError(f'{n.path}: missing capability event {n.capability}.{n.event}')
            # New emitters declare abort operations separately from trigger events.
            # Older IR models lack abort_events; keep them readable, but do not
            # silently accept an invalid abort event when metadata is present.
            if n.kind == 'abort_call' and 'abort_events' in cfg and n.event not in cfg['abort_events']:
                raise ModelError(f'{n.path}: undeclared capability abort event {n.capability}.{n.event}')
            kinds = cfg.get('event_kinds', {})
            declared = kinds.get(n.event) if n.event is not None else None
            if n.kind == 'service' and declared not in (None, 'sync_service'):
                raise ModelError(f'{n.path}: service_call requires sync_service metadata')
            if n.kind == 'call' and declared == 'sync_service':
                raise ModelError(f'{n.path}: service must use service_call')
        for c in n.children:
            check(c)
    check(root)
    return root,caps


def attach_mocks(model: dict, fixture: dict) -> dict:
    """Overlay test-only capability event outcomes on a compiler-emitted model.

    IR emitter owns `program`, `flows`, and capability instance/event declarations.
    Test fixture owns nondeterministic capability outcomes and abort/reset replies.
    """
    from copy import deepcopy
    merged = deepcopy(model)
    if not isinstance(fixture, dict):
        raise ModelError('mock fixture must be an object')
    if 'schema' in fixture and fixture['schema'] != 'koda.reference.mocks.v1':
        raise ModelError(f"unsupported mock schema: {fixture['schema']!r}")
    mocks = fixture.get('mocks', fixture)
    if not isinstance(mocks, dict):
        raise ModelError('mock fixture must contain an object named mocks')
    declarations = merged.get('capabilities')
    if not isinstance(declarations, dict):
        raise ModelError('model capabilities must be instance declarations')
    for instance, per_event in mocks.items():
        if instance not in declarations:
            raise ModelError(f'fixture for undeclared instance {instance}')
        declared = declarations[instance].get('events')
        if not isinstance(declared, (list, dict)):
            raise ModelError(f'{instance}: events must be a list or object')
        names = set(declared)
        if not isinstance(per_event, dict):
            raise ModelError(f'{instance}: mock event collection must be object')
        for event_name, cfg in per_event.items():
            if event_name not in names:
                raise ModelError(f'fixture for undeclared event {instance}.{event_name}')
            if not isinstance(cfg, dict):
                raise ModelError(f'{instance}.{event_name}: mock definition must be object')
        if set(per_event) != names:
            missing = sorted(names - set(per_event))
            raise ModelError(f'{instance}: fixture missing event mocks: {missing}')
        kinds = declarations[instance].get('event_kinds', {})
        if not isinstance(kinds, dict) or (kinds and set(kinds) != names):
            raise ModelError(f'{instance}: invalid event_kinds metadata')
        for event_name, mock in per_event.items():
            kind = kinds.get(event_name)
            if kind is not None and kind not in ('async_action', 'sync_action', 'sync_service'):
                raise ModelError(f'{instance}.{event_name}: unknown kind {kind!r}')
            _validate_mock(mock, f'{instance}.{event_name}', action_kind=kind)
        declarations[instance]['events'] = deepcopy(per_event)
    if set(mocks) != set(declarations):
        raise ModelError(f'fixtures missing capability instances: {sorted(set(declarations) - set(mocks))}')
    return merged
