"""Validate native nuXmv XML witnesses against a KODA reference exploration.

Unlike nuxmv_graph_compare, this tool compares individual *sampled paths*.
It reconstructs edge observations using the exact generated SMV expressions,
verifies the native XML next-state and combinatorial values, and reports first
observable differences. It does not claim trace-language equivalence.

The default one-invocation scope matches the established KODA/Dezyne comparison
contract: one task trigger, no externally initiated abort/reset/retrigger.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path
from typing import Any

from .macro_compare import reference_steps
from .nuxmv_adapter import load_xml
from .nuxmv_explicit import project_macrostep, read_mapping
from .smv_observations import SmvModel, typed_values

SCHEMA = 'koda.nuxmv.xml-witness-comparison.v1'
TERMINALS = {'task.reply.DONE', 'task.reply.FAILURE', 'task.reply.TERMINATE',
             'task.event.SUCCESS', 'task.event.FAILURE',
             'task.event.TERMINATED', 'task.event.ABORTED'}


def _flat(steps):
    return tuple(tok for step in steps for tok in step)


def _project(steps, observation):
    return tuple(tuple(tok for tok in step if observation == 'task+capability'
                       or tok.startswith('task.'))
                 for step in steps if any(observation == 'task+capability'
                                          or tok.startswith('task.') for tok in step))


def _prefix(obs, ref):
    return len(obs) <= len(ref) and ref[:len(obs)] == obs


def _earliest_difference(observed, expected):
    flat_o, flat_e = _flat(observed), _flat(expected)
    upto = 0
    for a, b in zip(flat_o, flat_e):
        if a != b:
            break
        upto += 1
    return {
        'common_observations': upto,
        'observed_next': flat_o[upto] if upto < len(flat_o) else None,
        'expected_next': flat_e[upto] if upto < len(flat_e) else None,
    }


def compare_xml(
    xml_path: Path, smv_path: Path, mapping_path: Path, reference_path: Path,
    *, observation='task+capability', scope='single-invocation',
    validate=True,
) -> dict[str, Any]:
    if observation not in ('task', 'task+capability'):
        raise ValueError('observation must be task or task+capability')
    if scope != 'single-invocation':
        raise ValueError('Only single-invocation scope is currently supported; extra resets/retriggers cannot be compared to the reference')
    mapping = read_mapping(mapping_path)
    smv = SmvModel.read(smv_path)
    samples = load_xml(xml_path)
    if len(samples) < 2:
        raise ValueError('XML witness has no transitions')

    steps = []
    inspected = []
    violations = []
    began = False
    terminal = False
    stopped_because = None
    for edge_i in range(len(samples)-1):
        source, destination = samples[edge_i], samples[edge_i+1]
        state = typed_values({k: v for k, v in source['state'].items() if k in smv.state_vars})
        next_state = typed_values({k: v for k, v in destination['state'].items() if k in smv.state_vars})
        inputs = typed_values({k: v for k, v in destination['input'].items() if k in smv.input_vars})
        comb = typed_values(destination['combinatorial'])
        if validate:
            # Native traces contain constants and DEFINE values inside <state>;
            # only actual state variables belong in validate_transition().
            smv.validate_transition(state, inputs, next_state)
            get = smv.evaluator(state, inputs)
            for name, value in comb.items():
                if name in smv.defines:
                    calculated = get(name)
                    if calculated != value:
                        raise ValueError(f'XML transition {edge_i}: combinatorial '
                                         f'{name}={value!r}, SMV expression says {calculated!r}')
        command = inputs.get('command')
        if command is None:
            raise ValueError(f'XML transition {edge_i} has no command')
        if scope == 'single-invocation':
            if not began:
                if command != 1:
                    if command != 0:
                        stopped_because = f'external command {command} before initial trigger'
                        break
                    continue
                began = True
            elif command in (1, 2, 3):
                violations.append({'transition': edge_i, 'command': command,
                                   'reason':'extra external trigger/abort/reset'})
                stopped_because = f'external command {command} after initial trigger'
                break
        tokens = list(project_macrostep(smv, mapping, {'state':state, 'input':inputs}))
        inspected.append({'transition':edge_i, 'command':command,
                          'from_state': source['state'].get(mapping['task_state']),
                          'to_state': destination['state'].get(mapping['task_state']),
                          'observations':tokens})
        if tokens:
            steps.append(tuple(tokens))
        if any(t in TERMINALS for t in tokens):
            terminal = True
            if scope == 'single-invocation':
                stopped_because = 'task terminal observation'
                # If task reports completion, no capability should remain
                # running unless explicitly permitted by the semantic contract.
                lingering = {name: next_state[name] for name in next_state
                             if name.endswith('_state') and next_state[name]=='Running'
                             and name.removesuffix('_state') in (
                                 a['call'].split('.')[0] for a in mapping['actions'])}
                if lingering:
                    violations.append({'transition':edge_i, 'reason':
                                       'task terminated with capability still Running',
                                       'capabilities':lingering})
                break
    if not began:
        raise ValueError('No CMD_TRIGGER found for a single-invocation comparison')

    raw_reference = json.loads(reference_path.read_text())
    if raw_reference.get('schema') != 'koda.reference.exploration.v1':
        raise ValueError('reference file is not a KODA exploration')
    reference = {_project(reference_steps(tr), observation)
                 for tr in raw_reference['traces']}
    reference.discard(())
    observed = _project(steps, observation)
    # One SMV transition is treated as one atomic KODA macrostep; this is an
    # explicit hypothesis to test, not an established correspondence for all
    # backend constructs. Preserve a flat diagnostic separately.
    exact = observed in reference
    prefixes = [r for r in reference if _prefix(observed, r)]
    flat_prefixes = [r for r in reference if _prefix(_flat(observed), _flat(r))]
    closest = max(reference, key=lambda r: _earliest_difference(observed, r)['common_observations'])
    return {
        'schema': SCHEMA, 'source':str(xml_path), 'smv':str(smv_path),
        'mapping':str(mapping_path), 'reference':str(reference_path),
        'scope':scope, 'observation':observation,
        'xml_states':len(samples), 'xml_transitions':len(samples)-1,
        'inspected_transitions':len(inspected), 'stopped_because':stopped_because,
        'task_terminal_observed':terminal,
        'observable_macrosteps':[list(s) for s in observed],
        'flat_observations':list(_flat(observed)),
        'reference_unique':len(reference),
        'exact_macrostep_matches':int(exact),
        'macrostep_prefix_matches':len(prefixes),
        'flat_prefix_matches':len(flat_prefixes),
        'diagnostic':_earliest_difference(observed, closest),
        'first_differing_reference_example':[list(s) for s in closest],
        'environment_or_lifecycle_issues': violations,
        'result': ('compatible_observed_witness' if exact or prefixes else
                   'incompatible_observed_witness'),
        'limitations':[
            'One sampled nuXmv witness does not enumerate the backend trace language',
            'This projection assumes each SMV edge is a KODA macrostep; test atomicity separately',
            'The current action projection does not reconstruct abort/reset calls; '
            'missing those operations is itself a reason not to declare equivalence',
            'Reference exploration is bounded; missing matches warrant manual examination',
            'The provided mapping must describe every relevant action and its trigger guard',
        ],
        'inspected': inspected,
    }


def main(argv=None):
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--xml', type=Path, required=True)
    p.add_argument('--smv', type=Path, required=True)
    p.add_argument('--mapping', type=Path, required=True)
    p.add_argument('--reference', type=Path, required=True)
    p.add_argument('--observation', choices=('task', 'task+capability'), default='task+capability')
    p.add_argument('--scope', choices=('single-invocation',), default='single-invocation')
    p.add_argument('--no-validate', action='store_true')
    p.add_argument('--output', type=Path)
    a=p.parse_args(argv)
    report=compare_xml(a.xml,a.smv,a.mapping,a.reference,
                       observation=a.observation,scope=a.scope,validate=not a.no_validate)
    if a.output:
        a.output.write_text(json.dumps(report, indent=2)+'\n')
    print(f"nuXmv XML: {report['result']} ("
          f"{report['inspected_transitions']}/{report['xml_transitions']} transitions, "
          f"{len(report['observable_macrosteps'])} observed macrosteps)")
    print('First difference:', report['diagnostic'])
    if report['environment_or_lifecycle_issues']:
        print('Environment/lifecycle issues:', report['environment_or_lifecycle_issues'])
    return 2 if report['result'] == 'incompatible_observed_witness' else 0

if __name__ == '__main__':
    raise SystemExit(main())
