"""Convert an EXPLICIT nuXmv transition graph into a canonical KODA AUT.

Input is an explicit state/input/next-state graph exported by nuXmv (and
converted to the small, documented JSON/wide-CSV exchange format). No
simulation traces are mistaken for a complete state space.

The KODA observation mapper is intentionally explicit and fail-closed: a
transition with a selected call and RESULT_NONE is reported as invalid, not
silently erased. Asynchronous completion events are emitted before synchronous
chaining within a single SMV edge (one KODA macrostep).

Limitations: only source constructs supported by the provided per-model mapping
can be projected. Native nuXmv CSV output varies with version and may require
an additional parser/normalizer. See NUXMV_EXPLICIT_LTS.md.
"""
from __future__ import annotations

import argparse
from collections import defaultdict, deque
import csv
from dataclasses import dataclass
import json
from pathlib import Path
from typing import Any

from .smv_observations import SmvModel, typed_values

VERSION = 'koda.nuxmv.explicit.v1'
RESULTS = {0: None, 1: 'ACCEPTED', 2: 'FAILURE', 3: 'HANDLING', 5: 'DONE'}
EVENTS = {0: None, 1: 'SUCCESS', 2: 'FAILURE', 3: 'ABORTED'}


def _val(value):
    if isinstance(value, dict):
        return typed_values(value)
    raise ValueError(f'Assignments must be mappings, got {type(value).__name__}')


def load_edges_json(path: Path):
    document = json.loads(path.read_text())
    if not isinstance(document, dict) or not isinstance(document.get('transitions'), list):
        raise ValueError('Expected a JSON object containing transitions[]')
    edges = []
    for i, record in enumerate(document['transitions']):
        for k in ('source', 'target', 'state', 'input', 'next_state'):
            if k not in record:
                raise ValueError(f'transition {i}: missing {k!r}')
        edges.append({**record, 'source': str(record['source']),
                      'target': str(record['target']),
                      'state': _val(record['state']),
                      'input': _val(record['input']),
                      'next_state': _val(record['next_state'])})
    return edges


def load_edges_csv(path: Path):
    """Read explicit WIDE CSV: source,target,state.*,input.*,next.* fields.

    This is an *interchange* CSV, not a claim about nuXmv's native CSV dialect.
    Native output without this header is rejected with an actionable message.
    """
    with path.open(newline='', encoding='utf-8-sig') as stream:
        reader = csv.DictReader(stream)
        columns = reader.fieldnames or []
        src = next((n for n in ('source','source_id','from','state_id') if n in columns), None)
        dst = next((n for n in ('target','target_id','to','next_state_id') if n in columns), None)
        groups = {'state':('state.','current.','current_','state_'),
                  'input':('input.','input_'),
                  'next_state':('next.','next_state.','next_','next_state_')}
        classified = {g:{} for g in groups}
        for name in columns:
            for group, prefixes in groups.items():
                for prefix in prefixes:
                    if name.startswith(prefix) and name != src and name != dst:
                        classified[group][name] = name[len(prefix):]
                        break
                if name in classified[group]:
                    break
        if not src or not dst or not all(classified.values()):
            raise ValueError(
                f'{path}: CSV headers are not the supported wide exchange format. '
                'Expected source,target,state.<var>,input.<var>,next.<var>; '
                f'found: {columns!r}. nuXmv native CSV may require adaptation '
                'after its first export. Never interpret the CSV as an AUT without fields.')
        edges=[]
        for i, row in enumerate(reader, 2):
            if any(v is None for v in row.values()):
                raise ValueError(f'{path}:{i}: malformed CSV row')
            edges.append({'source': row[src], 'target': row[dst],
                          **{group: typed_values({var:row[col] for col,var in values.items()})
                             for group,values in classified.items()}})
        return edges


def read_mapping(path: Path):
    data=json.loads(path.read_text())
    if data.get('schema') != 'koda.nuxmv.explicit.mapping.v1':
        raise ValueError('Wrong mapping schema')
    if not isinstance(data.get('actions'), list) or not data['actions']:
        raise ValueError('Mapping requires actions[]')
    for action in data['actions']:
        for key in ('signal','call','trigger_guard','kind'):
            if key not in action:
                raise ValueError(f'Mapping action missing {key}: {action}')
        if action['kind'] not in ('async','service'):
            raise ValueError('Action kind must be async or service')
    for key in ('task_reply', 'task_outcome', 'task_state'):
        if not isinstance(data.get(key), str):
            raise ValueError(f'Missing mapping {key}')
    return data


def project_macrostep(model: SmvModel, mapping: dict[str, Any], edge: dict[str, Any]):
    state, inputs = edge['state'], edge['input']
    get = model.evaluator(state, inputs)
    command = inputs.get('command')
    if command is None:
        raise ValueError('SMV input command is absent')
    result = []
    if command == 1 and state[mapping['task_state']] == 'Idle':
        result.append('task.trigger')
    # External asynchronous completions are dispatched inputs. They precede
    # any calls synchronously triggered by the completion in this macrostep.
    for a in mapping['actions']:
        if a['kind'] != 'async':
            continue
        outcome = get(a['signal']+'_outcome')
        if outcome not in EVENTS:
            raise ValueError(f'Unexpected SMV outcome {outcome} for {a["call"]}')
        if outcome and command == 0:
            result.append(f'{a["call"]}.event.{EVENTS[outcome]}')
    for a in mapping['actions']:
        if get(a['trigger_guard']):
            result.append(a['call'] + ('.service' if a['kind']=='service' else '.trigger'))
            reply = get(a['signal']+'_trigger_reply')
            if reply not in RESULTS or RESULTS[reply] is None:
                raise ValueError(f'INVALID generated SMV: {a["call"]} was selected for a trigger '
                                 f'but {_name(a)}_trigger_reply={reply!r} (expected a real reply); '
                                 f'state={state}, input={inputs}')
            if a['kind'] == 'service' and RESULTS[reply] not in ('DONE','FAILURE'):
                raise ValueError(f'Synchronous service {a["call"]} has illegal reply {reply}')
            if a['kind'] == 'async' and RESULTS[reply] not in ('ACCEPTED','FAILURE'):
                raise ValueError(f'Asynchronous action {a["call"]} has illegal reply {reply}')
            result.append(f'{a["call"]}.reply.{RESULTS[reply]}')
    if command == 1 and state[mapping['task_state']] == 'Idle':
        reply = get(mapping['task_reply'])
        if reply not in RESULTS or RESULTS[reply] is None:
            raise ValueError(f'Invalid task trigger reply {reply}')
        result.append(f'task.reply.{RESULTS[reply]}')
    elif command == 0:
        outcome = get(mapping['task_outcome'])
        if outcome not in EVENTS:
            raise ValueError(f'Unexpected task outcome {outcome}')
        if outcome:
            result.append(f'task.event.{EVENTS[outcome]}')
    return tuple(result)


def _name(action):
    return action['signal']


def generate(smv_path: Path, edges: list[dict[str,Any]], mapping: dict[str,Any],
             *, assume_complete: bool=False, allow_external: bool=False):
    model = SmvModel.read(smv_path)
    if not edges:
        raise ValueError('No explicit transitions imported')
    signatures = {}
    targets=set()
    for i, edge in enumerate(edges):
        # Independently check all imported triples against the source SMV. This
        # catches mismatched CSV columns and inconsistent conversion early.
        model.validate_transition(edge['state'],edge['input'],edge['next_state'])
        # Every graph vertex must have a consistent state valuation.
        for vertex, vals in ((edge['source'],edge['state']), (edge['target'],edge['next_state'])):
            current = signatures.setdefault(vertex, vals)
            if current != vals:
                raise ValueError(f'Graph vertex {vertex!r} has inconsistent state assignments at edge {i}')
        targets.add(edge['target'])
    initial_candidates = [vertex for vertex, state in signatures.items()
                          if all(str(state.get(var)) == init for var, init in model.init.items())]
    # Fully initialized models may have multiple initial states; here the
    # current compiler initializes every state VAR to one constant.
    if len(initial_candidates) != 1:
        raise ValueError(f'Expected exactly one initial state valuation, found {initial_candidates}. '
                         'Use initial states from the model, not an arbitrary CSV row.')
    start = initial_candidates[0]
    adjacency=defaultdict(list)
    for edge in edges:
        adjacency[edge['source']].append(edge)
    seen={start}
    q=deque([start])
    kept=[]
    filtered=0
    while q:
        vertex=q.popleft()
        for edge in adjacency[vertex]:
            # Scope matches the initial KODA/Dezyne comparison: one top-level
            # trigger, no external abort/reset, no second task trigger.
            cmd=edge['input'].get('command')
            if not allow_external and (cmd in (2,3) or
                  cmd == 1 and vertex != start):
                filtered += 1
                continue
            labels=project_macrostep(model,mapping,edge)
            kept.append({'source':vertex,'target':edge['target'],
                         'input':edge['input'], 'labels':list(labels)})
            if edge['target'] not in seen:
                seen.add(edge['target'])
                q.append(edge['target'])
    return {'schema': VERSION, 'backend':'nuxmv', 'smv':str(smv_path),
            'initial':start, 'reachable_vertices':len(seen),
            'reachable_macrostep_edges':len(kept),
            'input_transitions':len(edges), 'external_scope_filtered':filtered,
            'complete_export_claimed':assume_complete,
            'complete_for_selected_scope':assume_complete,
            'boundary_method':'one complete synchronous SMV transition is projected to one atomic KODA macrostep',
            'scope':'one task trigger, no external abort/reset/retrigger' if not allow_external else 'all exported inputs',
            'macrosteps':kept,
            'notes':['Only reachable source vertices are explored',
                     'No fairness or temporal abstraction is inferred from CSV',
                     'An exported full transition relation is required for language-level comparisons',
                     'Backend operation order is inferred from the explicit mapping; simultaneous independent operations need a justified canonical order',
                     'The AUT keeps macrostep actions adjacent but does not independently mark their boundaries']}


def write_aut(graph: dict[str,Any], path: Path):
    # Intern original SMV graph nodes; internal intermediate nodes are fresh.
    ids={graph['initial']:0}
    for edge in graph['macrosteps']:
        for node in (edge['source'],edge['target']):
            ids.setdefault(node,len(ids))
    nxt=len(ids)
    transitions=set()
    for edge in graph['macrosteps']:
        source=ids[edge['source']]
        target=ids[edge['target']]
        labels=edge['labels'] or ['tau']
        for i,label in enumerate(labels):
            destination=target if i==len(labels)-1 else nxt
            if i<len(labels)-1:
                nxt+=1
            # AUT label strings are quoted, escape for Aldebaran.
            transitions.add((source,label,destination))
            source=destination
    lines=[f'des (0,{len(transitions)},{nxt})']
    for source,label,destination in sorted(transitions):
        lines.append(f'({source},{json.dumps(label)},{destination})')
    path.write_text('\n'.join(lines)+'\n', encoding='utf-8')
    return len(transitions),nxt


def main(argv=None):
    parser=argparse.ArgumentParser(description=__doc__)
    group=parser.add_mutually_exclusive_group(required=True)
    group.add_argument('--edges-json', type=Path)
    group.add_argument('--edges-csv', type=Path)
    group.add_argument('--offline-enumerate', action='store_true', help='Experimental: explicitly enumerate supported finite SMV subset locally (NOT nuXmv)')
    parser.add_argument('--smv',type=Path,required=True)
    parser.add_argument('--mapping',type=Path,required=True)
    parser.add_argument('--aut',type=Path,required=True)
    parser.add_argument('--report',type=Path)
    parser.add_argument('--assume-complete-export',action='store_true',help='Assert external export is complete; do NOT set for a bounded sample')
    parser.add_argument('--max-states',type=int,default=500,help='Offline-only explicit state limit')
    parser.add_argument('--max-candidates',type=int,default=1000000,help='Offline-only input candidate limit')
    a=parser.parse_args(argv)
    if a.offline_enumerate:
        from .smv_offline import enumerate_finite
        local=enumerate_finite(a.smv,a.max_states,a.max_candidates)
        data=local['transitions']
        print(f'EXPERIMENTAL local SMV evaluation: {local["state_count"]} states, '
              f'{len(data)} transitions; NOT native nuXmv output')
    else:
        data=load_edges_json(a.edges_json) if a.edges_json else load_edges_csv(a.edges_csv)
    graph=generate(a.smv,data,read_mapping(a.mapping),
                   assume_complete=a.assume_complete_export and not a.offline_enumerate)
    if a.offline_enumerate:
        graph['source']='experimental standalone finite-SMV evaluator, NOT nuXmv'
    transitions,states=write_aut(graph,a.aut)
    report=a.report or Path(str(a.aut)+'.report.json')
    report.write_text(json.dumps(graph,indent=2)+'\n')
    print(f'Wrote {a.aut}: {states} states, {transitions} AUT transitions, '
          f'{graph["reachable_macrostep_edges"]} macrosteps. Report: {report}')
    if not graph['complete_for_selected_scope']:
        print('WARNING: export completeness not established; no equivalence claim')
    return 0

if __name__=='__main__':
    raise SystemExit(main())
