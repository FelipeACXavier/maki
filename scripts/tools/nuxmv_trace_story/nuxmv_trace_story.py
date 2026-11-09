#!/usr/bin/env python3
"""Turn nuXmv text counterexamples into readable Markdown and offline HTML.

Usage:
  python3 nuxmv_trace_story.py verify.log -o reports

Only the Python standard library is required. Supports the ordinary nuXmv
"-> State:" / "-> Input:" plain-text trace format, including sparse deltas.
"""
from __future__ import annotations

import argparse
import html
import re
from dataclasses import dataclass, field
from pathlib import Path

PROPERTY = re.compile(r'^-- specification\s+(.*?)\s+is false\s*$')
MARKER = re.compile(r'^\s*-> (State|Input): (\d+\.\d+) <-\s*$')
ASSIGNMENT = re.compile(r'^\s+([\w.]+)\s*=\s*(.*?)\s*$')

@dataclass
class Step:
    id: str
    changes: dict[str, str] = field(default_factory=dict)
    inputs: dict[str, str] = field(default_factory=dict)
    state: dict[str, str] = field(default_factory=dict)
    loop_start: bool = False

@dataclass
class Trace:
    property: str
    steps: list[Step] = field(default_factory=list)


def parse(text: str) -> list[Trace]:
    traces: list[Trace] = []
    current: Trace | None = None
    active: Step | None = None
    section = ''
    pending: dict[str, dict[str, str]] = {}
    last_property = '(unknown property)'
    next_loop = False
    for line in text.splitlines():
        m = PROPERTY.match(line)
        if m:
            last_property = m.group(1)
            current = None
            active = None
            section = ''
            continue
        if 'Trace Description:' in line and ('Counterexample' in line or 'Counter-example' in line):
            current = Trace(last_property)
            traces.append(current)
            pending = {}
            active = None
            section = ''
            next_loop = False
            continue
        if current is None:
            continue
        if '-- Loop starts here' in line:
            next_loop = True
            continue
        m = MARKER.match(line)
        if m:
            section, label = m.groups()
            if section == 'State':
                active = Step(label, loop_start=next_loop)
                next_loop = False
                current.steps.append(active)
                if label in pending:
                    active.inputs.update(pending.pop(label))
            else:
                active = None
                pending.setdefault(label, {})
            continue
        m = ASSIGNMENT.match(line)
        if m:
            name, value = m.groups()
            if section == 'State' and active is not None:
                active.changes[name] = value
            elif section == 'Input':
                # Input i.j is associated with the state labeled i.j.
                pending.setdefault(label, {})[name] = value
                if current.steps and current.steps[-1].id == label:
                    current.steps[-1].inputs[name] = value
        elif line.strip().startswith('-- specification'):
            current = None
            section = ''
            active = None
    for trace in traces:
        full: dict[str, str] = {}
        for step in trace.steps:
            full = {**full, **step.changes}
            step.state = full.copy()
    return [t for t in traces if t.steps]


def val(step: Step, key: str) -> str:
    return step.state.get(key, '—')


def cmd(step: Step) -> str:
    c = step.inputs.get('command', '0')
    return {'0': '—', '1': 'TRIGGER', '2': 'ABORT', '3': 'RESET'}.get(c, c)


def overlap(step: Step) -> bool:
    return val(step, 'camera_state') == 'Running' and val(step, 'safety_state') == 'Running'


def salient(step: Step) -> list[str]:
    result: list[str] = []
    # Keep only *new* observations, not the hundreds of generated DEFINEs.
    for name, value in step.changes.items():
        if (name in ('main_sequence_state', 'main_sequence_join_state',
                     'main_sequence_join_retry_state', 'main_sequence_join_retry_1_state',
                     'main_sequence_join_handler_state', 'camera_state', 'spectrometer_state',
                     'safety_state', 'recovery_running', 'enter_safety_running',
                     'initialize_running', 'main_running', 'camera_owner', 'safety_owner')
            or (name.endswith('_started') and value == 'TRUE')
            or (name.endswith('_failed') and value == 'TRUE' and name.count('_') <= 2)):
            result.append(f'{name}={value}')
    for name, value in step.inputs.items():
        if name != 'command' and value not in ('0', 'FALSE') and (
            name.endswith('_trigger_result') or name.endswith('_reset_result')
            or name.endswith('_abort_result') or name.endswith('_event')
        ):
            result.append(f'input {name}={value}')
    return result


def md_escape(v: str) -> str:
    return v.replace('|', '\\|').replace('`', '\\`')


def markdown(t: Trace, n: int) -> str:
    lines = [f'# Counterexample {n}', '', f'**Specification:** `{t.property}`', '',
             '> A row describes the state reached after the input shown in its Command column.',
             '> An omitted value in the original nuXmv trace retains its previous value.', '',
             '| State | Command | Main | Join | Handler | Camera | Spectrometer | Safety | Recovery | Overlap |',
             '|---|---|---|---|---|---|---|---|---|---|']
    for s in t.steps:
        r = [s.id + (' ↺' if s.loop_start else ''), cmd(s), val(s, 'main_sequence_state'),
             val(s, 'main_sequence_join_state'), val(s, 'main_sequence_join_handler_state'),
             val(s, 'camera_state'), val(s, 'spectrometer_state'), val(s, 'safety_state'),
             val(s, 'recovery_running'), '**VIOLATION**' if overlap(s) else '']
        lines.append('| ' + ' | '.join(md_escape(x) for x in r) + ' |')
    lines.extend(['', '## Relevant changes and inputs', ''])
    for s in t.steps:
        lines += [f'### State {s.id}' + (' (loop starts)' if s.loop_start else ''),
                  f'Command: `{cmd(s)}`' + (' — **camera/safety overlap**' if overlap(s) else ''), '']
        for item in salient(s):
            lines.append(f'- `{item}`')
        lines.append('')
    lines.extend(['', '## Interpretation note', '',
                  'This is an explanation of the emitted SMV trace; it is not an independently verified KODA execution. '
                  'Check the underlying emitter and lifecycle semantics before treating the violation as a source-model defect.', ''])
    return '\n'.join(lines)


def html_doc(t: Trace, n: int) -> str:
    esc = html.escape
    rows = []
    for s in t.steps:
        row = [s.id + (' ↺' if s.loop_start else ''), cmd(s), val(s, 'main_sequence_state'),
               val(s, 'main_sequence_join_state'), val(s, 'main_sequence_join_handler_state'),
               val(s, 'camera_state'), val(s, 'spectrometer_state'), val(s, 'safety_state'),
               val(s, 'recovery_running')]
        cls = ' class="bad"' if overlap(s) else ''
        rows.append('<tr'+cls+'>'+''.join(f'<td>{esc(x)}</td>' for x in row)+'</tr>')
    cards = []
    for s in t.steps:
        changed = '\n'.join(salient(s)) or '(no relevant changes)'
        full = '\n'.join(f'{k} = {v}' for k, v in sorted(s.changes.items()))
        head = f'State {s.id} — {cmd(s)}' + (' — VIOLATION' if overlap(s) else '')
        cards.append(f'''<details{' open' if overlap(s) else ''}><summary>{esc(head)}</summary>
        <h4>Relevant changes and inputs</h4><pre>{esc(changed)}</pre>
        <details><summary>All sparse assignments ({len(s.changes)})</summary><pre>{esc(full)}</pre></details></details>''')
    return f'''<!doctype html><html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width, initial-scale=1">
    <title>nuXmv counterexample {n}</title><style>
    :root {{color-scheme:light dark}} body {{font:15px/1.55 system-ui,-apple-system,sans-serif;max-width:1200px;margin:32px auto;padding:0 16px}}
    table {{border-collapse:collapse;width:100%;font-size:13px}} th,td {{padding:9px 11px;text-align:left;border-bottom:1px solid #9995;white-space:nowrap}}
    th {{position:sticky;top:0;background:Canvas}} tr.bad {{background:light-dark(#ffe0df,#4b2525);font-weight:700}}
    .scroll {{overflow:auto;border:1px solid #9996;border-radius:8px}} details {{margin:10px 0;padding:10px 14px;border:1px solid #9996;border-radius:8px}}
    summary {{cursor:pointer;font-weight:600}} pre {{overflow:auto;white-space:pre-wrap;overflow-wrap:anywhere;font-size:12px}}
    code {{overflow-wrap:anywhere}} .note {{opacity:.8}}
    </style></head><body><h1>Counterexample {n}</h1><p><b>Specification:</b> <code>{esc(t.property)}</code></p>
    <p class="note">Each command/input applies to the state in its row. Omitted values carry forward from earlier states. Red rows violate camera/safety exclusion.</p>
    <div class="scroll"><table><thead><tr><th>State</th><th>Command</th><th>Main</th><th>Join</th><th>Handler</th><th>Camera</th><th>Spectrometer</th><th>Safety</th><th>Recovery</th></tr></thead>
    <tbody>{''.join(rows)}</tbody></table></div><h2>Step details</h2>{''.join(cards)}</body></html>'''


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('log', type=Path, help='Output captured from nuXmv (e.g. tee verification.log)')
    ap.add_argument('-o', '--outdir', type=Path, default=Path('trace-reports'))
    args = ap.parse_args()
    traces = parse(args.log.read_text(encoding='utf-8', errors='replace'))
    if not traces:
        ap.error('No plain-text counterexamples found. Capture the full nuXmv output (including Trace Description).')
    args.outdir.mkdir(parents=True, exist_ok=True)
    for n, t in enumerate(traces, 1):
        name = f'counterexample-{n:02d}'
        (args.outdir / f'{name}.md').write_text(markdown(t, n), encoding='utf-8')
        (args.outdir / f'{name}.html').write_text(html_doc(t, n), encoding='utf-8')
        bad = [s.id for s in t.steps if overlap(s)]
        print(f'{name}: {len(t.steps)} states, overlap at {", ".join(bad) if bad else "not observed"}; reports in {args.outdir}')

if __name__ == '__main__':
    main()
