"""Bounded nuXmv witness comparison with KODA reference macrostep traces.

This compares execution *witnesses*, not full nuXmv reachable languages.
By default, only a prefix of a possible reference trace is required because
nuXmv simulation and BMC traces may stop before task completion.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path
from typing import Any

from .macro_compare import reference_steps
from .nuxmv_adapter import TRACE_SCHEMA, convert_files


def _project(step: tuple[str, ...] | list[str], observation: str) -> tuple[str, ...]:
    return tuple(t for t in step if observation == 'task+capability' or t.startswith('task.'))


def _projection(trace: tuple[tuple[str, ...], ...] | list[list[str]],
                observation: str) -> tuple[tuple[str, ...], ...]:
    return tuple(p for p in (_project(step, observation) for step in trace) if p)


def _prefix_macro(observed: tuple[tuple[str, ...], ...], reference: tuple[tuple[str, ...], ...]) -> bool:
    if not observed or len(observed) > len(reference):
        return False
    if observed[:-1] != reference[:len(observed)-1]:
        return False
    return reference[len(observed)-1][:len(observed[-1])] == observed[-1]


def _flatten(steps: tuple[tuple[str, ...], ...]) -> tuple[str, ...]:
    return tuple(t for step in steps for t in step)


def compare_observations(
    observations: dict[str, Any], reference: dict[str, Any],
    *, observation: str = 'task+capability', granularity: str = 'auto',
    mode: str = 'prefix',
) -> dict[str, Any]:
    if observations.get('schema') != TRACE_SCHEMA:
        raise ValueError('Expected ' + TRACE_SCHEMA)
    if reference.get('schema') != 'koda.reference.exploration.v1':
        raise ValueError('Expected koda.reference.exploration.v1')
    if observation not in ('task', 'task+capability'):
        raise ValueError('Unknown observation scope')
    if mode not in ('prefix', 'full'):
        raise ValueError('mode must be prefix or full')
    explicit = observations.get('has_explicit_macrosteps') is True
    if granularity == 'auto':
        granularity = 'macro' if explicit else 'flat'
    if granularity not in ('macro','flat'):
        raise ValueError('granularity must be macro or flat')
    if granularity == 'macro' and not explicit:
        raise ValueError('Macrostep comparison requires a verified SMV macrostep identifier; '
                         'use --granularity flat for transition-only traces')

    expected = {_projection(reference_steps(t), observation) for t in reference['traces']}
    # A reference exploration prefix can be empty after projection; never use
    # an empty projected trace as affirmative evidence.
    expected.discard(())
    if not expected:
        raise ValueError('No reference observations at selected scope')
    mapped_tokens = observations.get('mapping_tokens')
    required_tokens = {token for trace in expected for step in trace for token in step}
    available_tokens = set(mapped_tokens or [])
    missing_tokens = sorted(required_tokens-available_tokens) if mapped_tokens is not None else None
    observed = [_projection(t['macrosteps'], observation) for t in observations['traces']]
    unique = set(observed)
    found = set()
    witnesses: list[dict[str, Any]] = []
    for tr, candidate in zip(observations['traces'], observed):
        complete = bool(tr.get('complete'))
        if not candidate:
            status, matches = 'empty_observation', []
        else:
            matches = []
            for ref in expected:
                if granularity == 'macro':
                    agrees = (candidate == ref if mode == 'full' or complete
                              else _prefix_macro(candidate, ref))
                else:
                    got, exp = _flatten(candidate), _flatten(ref)
                    agrees = (got == exp if mode == 'full' or complete else
                              len(got) <= len(exp) and exp[:len(got)] == got)
                if agrees:
                    matches.append(ref)
            status = 'compatible_witness' if matches else 'not_in_reference'
            found.update(matches)
        witnesses.append({
            'file': tr.get('file'), 'status': status, 'complete': complete,
            'observed': [list(s) for s in candidate] if granularity == 'macro'
                        else list(_flatten(candidate)),
            'compatible_reference_traces': len(matches),
        })
    incompatible = [w for w in witnesses if w['status'] == 'not_in_reference']
    return {
        'schema': 'koda.reference.nuxmv-comparison.v1',
        'backend': 'nuxmv', 'mode': mode, 'observation': observation,
        'granularity': granularity,
        'comparison_strength': 'finite_execution_witnesses_only',
        'reference_bounded': reference.get('bounded'),
        'reference_max_dispatches': reference.get('max_dispatches'),
        'reference_max_traces': reference.get('max_traces'),
        'mapping_covers_reference_alphabet': (not missing_tokens if missing_tokens is not None else None),
        'unmapped_reference_tokens': missing_tokens,
        'counts': {
            'reference_unique': len(expected), 'nuxmv_traces': len(observed),
            'nuxmv_unique': len(unique), 'compatible_witnesses':
            sum(w['status']=='compatible_witness' for w in witnesses),
            'incompatible_witnesses': len(incompatible),
            'empty_witnesses': sum(w['status']=='empty_observation' for w in witnesses),
            'reference_traces_with_a_compatible_witness': len(found),
            'reference_traces_not_witnessed': len(expected-found),
        },
        'witnesses': witnesses,
        'result': ('incomplete_mapping' if missing_tokens else
                   'incompatible_witnesses' if incompatible else
                   'no_observations' if not any(w['status']=='compatible_witness' for w in witnesses) else
                   'compatible_witnesses_only'),
        'limitations': [
            'nuXmv simulation and counterexample export do not enumerate all traces; no equivalence claim',
            'The observation mapping must faithfully expose every relevant KODA call, reply, event and order',
            'Transition-only projections lose KODA macrostep atomicity' if not explicit or granularity=='flat'
            else 'Explicit macrostep variable must correctly group synchronous KODA actions',
            'Reference exploration is bounded, so absent backend witnesses are not automatically bugs',
            'Timed, fairness and infinite executions are outside finite trace matching',
            'A counterexample is a property-specific execution; no counterexample is not a trace-equality proof',
        ],
    }



def compare_dezyne_witnesses(observations: dict[str, Any], aut_path: Path, reference_path: Path,
                             *, observation: str, granularity: str, mode: str,
                             model: Path | None = None, mocks: Path | None = None,
                             max_hops: int = 100) -> dict[str, Any]:
    """Cross-check observed nuXmv witnesses against the existing AUT adapter.

    This is a *diagnostic third leg*, with the same limitations as the AUT
    macrostep inference (no external abort/reset, bounded graph traversal).
    """
    from .aut_compare import parse_aut
    from .macro_compare import aut_steps
    from .contracts import resolve_contracts

    contracts = resolve_contracts(reference_path, model=model, mocks=mocks)
    paths, omitted, cutoff = aut_steps(
        parse_aut(aut_path), async_actions=set(contracts.async_actions),
        service_actions=set(contracts.service_actions), max_hops=max_hops)
    dezyne = {_projection(p, observation) for p in paths}
    dezyne.discard(())
    rows = []
    for witness in observations['traces']:
        candidate = _projection(witness['macrosteps'], observation)
        if not candidate:
            status = 'empty_observation'
        else:
            found = False
            for path in dezyne:
                if granularity == 'macro':
                    found = (candidate == path if mode == 'full' or witness.get('complete')
                             else _prefix_macro(candidate, path))
                else:
                    got, expected = _flatten(candidate), _flatten(path)
                    found = (got == expected if mode == 'full' or witness.get('complete')
                             else len(got) <= len(expected) and expected[:len(got)] == got)
                if found:
                    break
            status = 'compatible_witness' if found else ('inconclusive_cutoff' if cutoff
                                                          else 'not_in_dezyne_aut')
        rows.append({'file': witness.get('file'), 'status': status})
    return {
        'schema': 'koda.reference.nuxmv-dezyne-witness-check.v1',
        'aut': str(aut_path), 'aut_macrostep_method': 'inferred (not annotated in AUT)',
        'call_contract_source': contracts.source, 'aut_max_hops': max_hops,
        'aut_cutoff_reached': cutoff, 'aut_omitted': omitted,
        'dezyne_unique_terminal_traces': len(dezyne),
        'nuxmv_witnesses_compatible_with_dezyne': sum(
            row['status']=='compatible_witness' for row in rows),
        'nuxmv_witnesses_not_in_dezyne': sum(
            row['status']=='not_in_dezyne_aut' for row in rows),
        'witnesses': rows,
        'assumptions': [
            'AUT is restricted to one initial task.trigger and no external task abort/reset',
            'AUT macrostep boundaries inferred from capability completions',
            'async/sync call kinds are constrained using KODA emitted model/fixtures',
            'AUT is explored to a finite hop/configuration limit; no trace language proof',
        ],
    }


def main(argv: list[str] | None = None) -> int:
    p = argparse.ArgumentParser(description=__doc__)
    source = p.add_mutually_exclusive_group(required=True)
    source.add_argument('--observations', type=Path, help='Previously converted nuXmv JSON')
    source.add_argument('--xml', type=Path, nargs='+', help='Raw nuXmv XML trace(s)')
    p.add_argument('--mapping', type=Path, help='Required with --xml')
    p.add_argument('--aut', type=Path, help='Optional: compare same witnesses against Dezyne .aut')
    p.add_argument('--model', type=Path, help='KODA .reference.json (required for AUT kind contracts if not autodetected)')
    p.add_argument('--mocks', type=Path, help='KODA mock sidecar for AUT call contracts')
    p.add_argument('--max-aut-hops', type=int, default=100)
    p.add_argument('--reference', type=Path, required=True)
    p.add_argument('--granularity', choices=('auto','macro','flat'), default='auto')
    p.add_argument('--observation', choices=('task','task+capability'), default='task+capability')
    p.add_argument('--mode', choices=('prefix','full'), default='prefix')
    p.add_argument('--output', type=Path)
    a = p.parse_args(argv)
    if a.xml:
        if a.mapping is None:
            p.error('--mapping is required with --xml')
        obs = convert_files(a.xml, a.mapping)
    else:
        if a.mapping is not None:
            p.error('--mapping is only used with --xml')
        obs = json.loads(a.observations.read_text(encoding='utf-8'))
    report = compare_observations(obs,
        json.loads(a.reference.read_text(encoding='utf-8')),
        observation=a.observation, granularity=a.granularity, mode=a.mode)
    if a.aut is not None:
        report['dezyne_witness_check'] = compare_dezyne_witnesses(
            obs, a.aut, a.reference, observation=a.observation,
            granularity=report['granularity'], mode=a.mode,
            model=a.model, mocks=a.mocks, max_hops=a.max_aut_hops)
    elif a.model is not None or a.mocks is not None:
        p.error('--model/--mocks require --aut')
    if a.output:
        a.output.write_text(json.dumps(report, indent=2)+'\n', encoding='utf-8')
    c = report['counts']
    print(f"nuXmv {report['granularity']} {a.mode}: "
          f"compatible {c['compatible_witnesses']}/{c['nuxmv_traces']}; "
          f"incompatible={c['incompatible_witnesses']}, empty={c['empty_witnesses']}; "
          f"reference witnessed {c['reference_traces_with_a_compatible_witness']}/{c['reference_unique']} "
          f"({report['result']}; not equivalence)")
    if a.aut is not None:
        d = report['dezyne_witness_check']
        print(f"Dezyne AUT: compatible {d['nuxmv_witnesses_compatible_with_dezyne']}/{c['nuxmv_traces']}; "
              f"missing={d['nuxmv_witnesses_not_in_dezyne']}, "
              f"cutoff={d['aut_cutoff_reached']} (bounded diagnostic)")
    return 2 if (report['result'] in ('incompatible_witnesses','incomplete_mapping') or
                 a.aut is not None and report['dezyne_witness_check']['nuxmv_witnesses_not_in_dezyne'] > 0) else 0


if __name__ == '__main__':
    raise SystemExit(main())
