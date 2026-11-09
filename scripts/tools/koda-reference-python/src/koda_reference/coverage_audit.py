"""Inspect compiler reference IR and a Dezyne AUT without claiming equivalence.

Reports structural KODA constructs and corresponding *observed Dezyne labels*.
For a real source program this also helps select targeted conformance cases.
"""
from __future__ import annotations
from collections import Counter,deque
from pathlib import Path
import argparse
import json
import re
from .aut_compare import parse_aut


def structural_counts(model:dict)->dict[str,int]:
    counts=Counter()
    def visit(n):
        if not isinstance(n,dict) or len(n)!=1:return
        kind,body=next(iter(n.items()))
        counts[kind]+=1
        if kind in ('sequence','parallel','either'):
            for c in body:visit(c)
        elif kind=='choose':
            for item in body:visit(item['body'] if 'body' in item else item['default'])
        elif kind in ('on_abort','on_error'):
            visit(body['action']);visit(body['handler'])
        elif kind in ('repeat','retry'):
            visit(body['body'])
        elif kind=='within':
            visit(body['body']);visit(body['else'])
    for name,flow in model.get('flows',{}).items():visit(flow)
    return dict(sorted(counts.items()))


def reachability(lts, regexes:dict[str,str],max_states:int=500_000):
    adj=lts.successors()
    targets={name:re.compile(pattern) for name,pattern in regexes.items()}
    found={}
    q=deque([(lts.initial,0)])
    visited={lts.initial}
    while q and len(visited)<=max_states and len(found)<len(targets):
        node,d=q.popleft()
        for label,nxt in adj.get(node,()):
            if label in ('<illegal>','<missing-reply>','<non-compliance>'):continue
            for name,pat in targets.items():
                if name not in found and pat.fullmatch(label):
                    found[name]={'label':label,'distance_transitions':d+1}
            if nxt not in visited:
                visited.add(nxt);q.append((nxt,d+1))
    return found,len(visited),bool(q)


def audit(aut:Path,model:Path,generated:Path|None=None):
    lts=parse_aut(aut)
    m=json.loads(model.read_text())
    counts=structural_counts(m)
    regexes={'handler_completed_terminate':r'api\.terminated',
             'abort_handling_reply':r'api\.Result:Handling',
             'timeout_event':r'sut\.alarm\d+\.api\.timeout',
             'condition_true':r'sut\..+condition\d+\.api\.ConditionResult:True',
             'condition_false':r'sut\..+condition\d+\.api\.ConditionResult:False',
             'failure_event':r'api\.failure',
             'abort_event':r'api\.aborted'}
    witness,visited,limited=reachability(lts,regexes)
    required={
        'sequence':['sequence2.dzn','sequence3.dzn'],
        'parallel':['parallel2.dzn'],
        'within':['within30000.dzn','ialarm.dzn'],
        'repeat':['repeat.dzn'],
        'choose':['selector3.dzn','icondition.dzn'],
        'on_abort':['abort_handler.dzn'],
        'on_error':['error_handler.dzn'],
        'abort_call':['abort_call.dzn','iabort.dzn'],
        'success':['terminate.dzn'],
        'failure':['failure.dzn']}
    libraries={}
    if generated:
        for kind,files in required.items():
            if counts.get(kind):
                libraries[kind]={f:(generated/'lib'/f).exists() for f in files}
    return {'schema':'koda.reference.coverage-audit.v1',
            'aut':{'states':lts.declared_states,'transitions':lts.declared_transitions,
                   'initial':lts.initial},
            'model':str(model),'model_fixture_origin':m.get('fixture_origin','emitter_model_unverified'),
            'flow_count':len(m.get('flows',{})),
            'construct_counts':counts,'dezyne_generated_libraries':libraries,
            'reachable_witnesses':witness,'reachable_lts_states':visited,
            'reachability_truncated':limited,
            'limitations':['LTS label reachability is not semantic equivalence',
                           'AUT contains implementation scheduling steps and generic protocol nondeterminism',
                           'Unbounded Repeat and timing cannot be covered by finite path enumeration']}


def main(argv=None):
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--aut',type=Path,required=True);p.add_argument('--model',type=Path,required=True)
    p.add_argument('--generated',type=Path,help='Dezyne output directory for library presence checks')
    p.add_argument('--output',type=Path)
    args=p.parse_args(argv)
    d=audit(args.aut,args.model,args.generated)
    print('LTS:',d['aut'],'KODA flows:',d['flow_count'])
    print('Constructs:',d['construct_counts'])
    print('Reachable Dezyne labels:',{k:v['label'] for k,v in d['reachable_witnesses'].items()})
    if args.output:args.output.write_text(json.dumps(d,indent=2)+'\n')
    return 0

if __name__=='__main__':raise SystemExit(main())
