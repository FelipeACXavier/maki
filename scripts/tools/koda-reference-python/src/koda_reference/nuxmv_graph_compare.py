"""Compare macrostep traces from a projected nuXmv explicit graph to KODA.

No graph completeness is inferred from a finite trace limit. Preserves the
existing XML-witness nuXmv adapter and Dezyne comparator unchanged.
"""
from __future__ import annotations

import argparse
from collections import defaultdict,deque
import json
from pathlib import Path
from typing import Any

from .macro_compare import reference_steps

TERMINALS={
  'task.reply.DONE','task.reply.FAILURE','task.reply.TERMINATE',
  'task.event.SUCCESS','task.event.FAILURE','task.event.TERMINATED','task.event.ABORTED',
}


def explore(graph:dict[str,Any],max_macrosteps:int=16,max_paths:int=100_000):
    if graph.get('schema')!='koda.nuxmv.explicit.v1':
        raise ValueError('Expected koda.nuxmv.explicit.v1')
    adj=defaultdict(list)
    for edge in graph['macrosteps']:
        adj[edge['source']].append(edge)
    queue=deque([(graph['initial'],(),0,False)])
    found=set()
    prefixes=set()
    cutoff=False
    expanded=0
    while queue:
        state,steps,n,started=queue.popleft()
        expanded+=1
        if expanded>max_paths:
            cutoff=True
            break
        if n>=max_macrosteps:
            prefixes.add(steps)
            cutoff=True
            continue
        for edge in adj.get(state,[]):
            labels=tuple(edge['labels'])
            # skip pure stuttering self loops, but retain state-changing tau
            # transitions: they could affect future behavior.
            if not labels and edge['source']==edge['target']:
                continue
            new_started=started or 'task.trigger' in labels
            if not new_started:
                if edge['target']==state:
                    continue
                queue.append((edge['target'],steps,n+1,False))
                continue
            new_steps=steps+((labels,) if labels else ())
            if any(t in TERMINALS for t in labels):
                found.add(new_steps)
            elif labels or edge['target']!=state:
                queue.append((edge['target'],new_steps,n+1,new_started))
    return found,prefixes,cutoff


def compare(graph:dict[str,Any], reference:dict[str,Any], max_macrosteps:int=16,max_paths:int=100_000):
    if reference.get('schema')!='koda.reference.exploration.v1':
        raise ValueError('Expected KODA exploration JSON')
    observed,prefixes,limit=explore(graph,max_macrosteps,max_paths)
    expected={reference_steps(t) for t in reference['traces']}
    only_expected=expected-observed
    only_observed=observed-expected
    cutoff=(limit or reference.get('cutoff_reached') is True or
            not graph.get('complete_for_selected_scope'))
    result=('inconclusive_cutoff' if cutoff else
            'match_under_assumptions' if not only_expected and not only_observed else 'mismatch')
    return {
        'schema':'koda.nuxmv.graph-comparison.v1',
        'counts':{'reference':len(expected),'nuxmv':len(observed),
                  'matched':len(expected&observed),
                  'reference_only':len(only_expected),
                  'nuxmv_only':len(only_observed)},
        'result':result, 'inconclusive':cutoff,
        'graph_complete_claimed':graph.get('complete_for_selected_scope',False),
        'reference_cutoff_reached':reference.get('cutoff_reached'),
        'comparison_cutoff_reached':limit,
        'nonterminal_prefixes':len(prefixes),
        'reference_only_examples':[[list(st) for st in trace] for trace in sorted(only_expected)[:12]],
        'nuxmv_only_examples':[[list(st) for st in trace] for trace in sorted(only_observed)[:12]],
        'scope':graph.get('scope'),
        'limitations':['Comparison of finite terminal macrostep paths only; infinite runs are not decided',
                       'Same mock contracts and environment must be used on both sides',
                       'No model-checking property-preservation proof',
                       'nuXmv fairness not encoded in this graph exploration',
                       'Macrostep order must be justified for concurrent emitters'],
    }


def main(argv=None):
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--graph',type=Path,required=True)
    p.add_argument('--reference',type=Path,required=True)
    p.add_argument('--max-macrosteps',type=int,default=16)
    p.add_argument('--max-paths',type=int,default=100_000)
    p.add_argument('--output',type=Path)
    a=p.parse_args(argv)
    report=compare(json.loads(a.graph.read_text()),json.loads(a.reference.read_text()),a.max_macrosteps,a.max_paths)
    if a.output:
        a.output.write_text(json.dumps(report,indent=2)+'\n')
    print(f"nuXmv macrostep traces: {report['counts']}\nResult: {report['result']}")
    return 0 if report['result']=='match_under_assumptions' else 1

if __name__=='__main__':
    raise SystemExit(main())
