"""Project nuXmv XML state/input traces to ordered KODA observations.

Requires an explicit compiler-specific observation mapping: raw SMV states do
not encode the operation calls/replies, their order, or macrostep boundaries.
See NUXMV_COMPARISON.md for the mapping contract and safety limitations.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path
import xml.etree.ElementTree as ET
from typing import Any

SCHEMA = 'koda.nuxmv.mapping.v1'
TRACE_SCHEMA = 'koda.nuxmv.observations.v1'
TERMINALS = frozenset({
    'task.reply.DONE', 'task.reply.FAILURE', 'task.reply.TERMINATE',
    'task.event.SUCCESS', 'task.event.FAILURE', 'task.event.TERMINATED',
    'task.event.ABORTED',
})


def _section(parent: ET.Element | None, tag: str) -> dict[str, str]:
    if parent is None:
        return {}
    section = parent.find(tag)
    if section is None:
        return {}
    values: dict[str, str] = {}
    for val in section.findall('value'):
        name = val.get('variable')
        if not name:
            raise ValueError(f'<{tag}> value without variable name')
        if name in values:
            raise ValueError(f'Duplicate variable {name!r} in <{tag}>')
        values[name] = (val.text or '').strip()
    return values


def load_xml(path: Path) -> list[dict[str, dict[str, str]]]:
    """Re-align nuXmv input[i+1] in node[i] with destination state[i+1].

    State assignments may be delta-coded; reconstruct them conservatively. An
    input or combinatorial assignment is *not* carried over: those are per edge.
    """
    try:
        root = ET.parse(path).getroot()
    except ET.ParseError as exc:
        raise ValueError(f'{path}: malformed nuXmv XML: {exc}') from exc
    if root.tag not in ('counter-example', 'counterexample'):
        raise ValueError(f'{path}: expected nuXmv <counter-example>, found <{root.tag}>')
    nodes = root.findall('node')
    if not nodes:
        raise ValueError(f'{path}: no <node> elements in XML trace')
    samples: list[dict[str, dict[str, str]]] = []
    state: dict[str, str] = {}
    for i, node in enumerate(nodes):
        current = _section(node, 'state')
        if not current:
            raise ValueError(f'{path}: missing/empty <state> in node {i}')
        state.update(current)
        if i == 0:
            inputs, combinatorial = {}, {}
        else:
            prev = nodes[i-1]
            inputs = _section(prev, 'input')
            combinatorial = _section(prev, 'combinatorial')
        samples.append({'state': dict(state), 'input': inputs,
                        'combinatorial': combinatorial})
    return samples


def read_mapping(path: Path) -> dict[str, Any]:
    data = json.loads(path.read_text(encoding='utf-8'))
    if not isinstance(data, dict) or data.get('schema') != SCHEMA:
        raise ValueError(f'{path}: expected schema {SCHEMA}')
    signals = data.get('signals')
    if not isinstance(signals, list) or not signals:
        raise ValueError(f'{path}: provide a nonempty ordered signals list')
    for i, entry in enumerate(signals):
        _validate_selector(entry, f'signals[{i}]')
        if entry.get('emit', 'active') not in ('active', 'change', 'rise'):
            raise ValueError(f'signals[{i}].emit must be active, change or rise')
        mapping = entry.get('values')
        if not isinstance(mapping, dict) or not mapping or not all(
            isinstance(k, str) and isinstance(v, str) and v for k, v in mapping.items()
        ):
            raise ValueError(f'signals[{i}].values must map SMV values to KODA tokens')
        if any(not _valid_token(v) for v in mapping.values()):
            raise ValueError(f'signals[{i}]: a mapped value is not a supported KODA observation token')
        if 'ignore' in entry and (not isinstance(entry['ignore'], list) or
                                   not all(isinstance(v, str) for v in entry['ignore'])):
            raise ValueError(f'signals[{i}].ignore must be a list of values')
        if set(mapping) & set(entry.get('ignore', [])):
            raise ValueError(f'signals[{i}]: mapped and ignored values overlap')
    if 'macrostep' in data and data['macrostep'] is not None:
        _validate_selector(data['macrostep'], 'macrostep')
    if data.get('initial_state', 'ignore') not in ('ignore', 'include'):
        raise ValueError('initial_state must be ignore or include')
    return data


def _valid_token(token: str) -> bool:
    parts = token.split('.')
    if len(parts) < 2:
        return False
    if parts[0] == 'task':
        return token in ('task.trigger', 'task.abort', 'task.reset') or (
            len(parts) == 3 and parts[1] in ('reply','event','abort_reply','reset_reply')
            and parts[2] in ('ACCEPTED','DONE','FAILURE','SUCCESS','HANDLING',
                             'TERMINATE','TERMINATED','ABORTED'))
    return len(parts) >= 3 and parts[-1] in ('trigger','service','abort','reset') or (
        len(parts) >= 4 and parts[-2] in ('reply','event','abort_reply','reset_reply')
        and parts[-1] in ('ACCEPTED','DONE','FAILURE','SUCCESS','HANDLING',
                          'TERMINATE','TERMINATED','ABORTED'))


def _validate_selector(selector: Any, name: str) -> None:
    if not isinstance(selector, dict) or selector.get('source') not in ('state', 'input', 'combinatorial'):
        raise ValueError(f'{name}.source must be state, input, or combinatorial')
    if not isinstance(selector.get('variable'), str) or not selector['variable']:
        raise ValueError(f'{name}.variable is required')


def _value(sample: dict[str, dict[str, str]], selector: dict[str, Any],
           *, path: Path, step: int) -> str:
    src, var = selector['source'], selector['variable']
    if var not in sample[src]:
        raise ValueError(f'{path}: step {step}: missing {src} variable {var!r}. '
                         'Check mapping/trace export; observations cannot be guessed.')
    return sample[src][var]


def convert_trace(xml: Path, mapping: dict[str, Any]) -> dict[str, Any]:
    samples = load_xml(xml)
    signals = mapping['signals']
    prev: list[str | None] = [None] * len(signals)
    initial = 0 if mapping.get('initial_state', 'ignore') == 'include' else 1
    has_marker = mapping.get('macrostep') is not None
    steps: list[list[str]] = []
    seen_marker_blocks: set[str] = set()
    previous_step_marker: str | None = None
    markers: list[str] = []
    emissions: list[dict[str, Any]] = []
    raw_steps_with_events: list[int] = []
    for index in range(initial, len(samples)):
        sample = samples[index]
        marker = (_value(sample, mapping['macrostep'], path=xml, step=index)
                  if has_marker else str(index))
        if has_marker:
            if marker != previous_step_marker:
                if marker in seen_marker_blocks:
                    raise ValueError(f'{xml}: macrostep marker {marker!r} reappeared after a different marker')
                seen_marker_blocks.add(marker)
                previous_step_marker = marker
        active: list[str] = []
        for j, spec in enumerate(signals):
            value = _value(sample, spec, path=xml, step=index)
            old = prev[j]
            prev[j] = value
            if value in spec.get('ignore', ['NONE']):
                continue
            if value not in spec['values']:
                raise ValueError(f'{xml}: step {index}: {spec["variable"]}={value!r} '
                                 'is neither mapped nor ignored (fail closed)')
            mode = spec.get('emit', 'active')
            if mode == 'change' and value == old:
                continue
            if mode == 'rise' and old not in spec.get('ignore', ['NONE']) and old is not None:
                continue
            active.append(spec['values'][value])
        if not active:
            continue
        if has_marker and marker in markers and markers[-1] != marker:
            raise ValueError(f'{xml}: macrostep marker {marker!r} reappeared noncontiguously')
        if not steps or markers[-1] != marker:
            steps.append([])
            markers.append(marker)
        raw_steps_with_events.append(index)
        for token in active:
            steps[-1].append(token)
            emissions.append({'step': index, 'macrostep': marker, 'ordinal': len(steps[-1])-1,
                              'token': token})
    flattened = [token for step in steps for token in step]
    task_terminals = [token for token in flattened if token in TERMINALS]
    # Terminal recognition is conservative: terminal event/reply should be LAST.
    complete = bool(flattened and flattened[-1] in TERMINALS)
    return {'file': str(xml), 'xml_states': len(samples), 'raw_steps_with_events': raw_steps_with_events,
            'macrostep_ids': markers, 'macrosteps': steps, 'flat': flattened,
            'emissions': emissions, 'complete': complete,
            'task_terminal_tokens': task_terminals}


def convert_files(paths: list[Path], mapping_path: Path) -> dict[str, Any]:
    if not paths:
        raise ValueError('Pass at least one nuXmv XML trace')
    mapping = read_mapping(mapping_path)
    mapped = sorted({token for entry in mapping['signals'] for token in entry['values'].values()})
    return {'schema': TRACE_SCHEMA, 'backend': 'nuxmv', 'mapping': str(mapping_path),
            'mapping_tokens': mapped,
            'has_explicit_macrosteps': mapping.get('macrostep') is not None,
            'boundary_method': ('explicit observable macrostep identifier from SMV trace'
                                if mapping.get('macrostep') is not None else
                                'SMV transitions only (NOT semantic macrosteps)'),
            'traces': [convert_trace(path, mapping) for path in paths]}


def main(argv: list[str] | None = None) -> int:
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--xml', type=Path, required=True, nargs='+', help='One or more XML trace files')
    p.add_argument('--mapping', type=Path, required=True)
    p.add_argument('--output', type=Path, required=True)
    a = p.parse_args(argv)
    data = convert_files(a.xml, a.mapping)
    a.output.write_text(json.dumps(data, indent=2) + '\n', encoding='utf-8')
    print(f"Converted {len(data['traces'])} nuXmv trace(s); "
          f"explicit macrosteps={data['has_explicit_macrosteps']}; saved {a.output}")
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
