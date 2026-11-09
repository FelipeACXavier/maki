"""Generate a conservative construct-coverage inventory for a KODA IR projection.

A construct appearing in the serialized IR is not proof that its Dezyne
implementation has been validated. Run concrete scenarios and compare traces.
"""
from __future__ import annotations
from collections import Counter
from pathlib import Path
import argparse
import json
from .model import parse_node

# 'implemented' means reference-engine paths exist, not full semantic proof.
KNOWN = {
    'call':'bounded_implementation','service':'bounded_implementation',
    'flow_call':'bounded_implementation','sequence':'bounded_implementation',
    'parallel':'bounded_implementation','on_abort':'bounded_implementation',
    'on_error':'bounded_implementation','success':'bounded_implementation',
    'failure':'bounded_implementation','continue':'bounded_implementation',
    'abort_call':'provisional_shared_ownership',
    'within':'untimed_abstract_timer','repeat':'bounded_iteration_only',
    'retry':'bounded_iteration_only','choose':'nondeterministic_unknown_guards',
    'either':'nondeterministic_choice'
}


def inventory(model:dict, exploration:dict|None=None) -> dict:
    root=parse_node(model['program'],flows=model.get('flows',{}))
    counts=Counter()
    usages:dict[str,list[str]]={}
    call_sites=Counter()
    def visit(node):
        counts[node.kind]+=1
        usages.setdefault(node.kind,[]).append(node.path)
        if node.kind in ('call','service','abort_call'):
            call_sites[f'{node.capability}.{node.event}']+=1
        for child in node.children:visit(child)
    visit(root)
    critical=[]
    if any(counts[k] for k in ('repeat','retry')):
        critical.append('Loops are bounded exploration; infinite repeat cannot be exhausted.')
    if counts['within']:
        critical.append('Within timeout is abstracted as a nondeterministic event; elapsed time is not verified.')
    if counts['choose']:
        critical.append('Choose unknown data guards are nondeterministic; use concrete data valuations to prove guard truth.')
    if any(x>1 for x in call_sites.values()):
        critical.append('Repeated capability-event call sites do not establish shared execution/arbiter ownership equivalence.')
    if counts['abort_call']:
        critical.append('Direct abort broadcasts are provisional; there are no stable shared execution IDs.')
    if counts['on_abort']:
        critical.append('Nested HANDLING, repeated abort while Handling and synchronous handler completion need dedicated tests.')
    result={'schema':'koda.reference.construct-inventory.v1',
            'constructs':{k:{'instances':n,'status':KNOWN.get(k,'unsupported'),
                             'examples':usages[k][:5]} for k,n in sorted(counts.items())},
            'call_sites':dict(sorted(call_sites.items())),
            'unsupported_not_encoded':['on_signal (Emitter, EmitterContinue)','flow parameters',
                                       'timed ordering / clocks','parameter value propagation'],
            'verification_caveats':critical,
            'metric':'Structural presence and bounded interpreter availability, not equivalence proof'}
    if exploration is not None:
        result['exploration']={'runs':len(exploration.get('traces',[])),
                               'cutoff_reached':bool(exploration.get('cutoff_reached',True)),
                               'max_dispatches':exploration.get('max_dispatches'),
                               'max_traces':exploration.get('max_traces')}
    return result


def main(argv=None):
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--model',required=True,type=Path)
    p.add_argument('--exploration',type=Path)
    p.add_argument('--output',type=Path)
    args=p.parse_args(argv)
    model=json.loads(args.model.read_text(encoding='utf-8'))
    exploration=json.loads(args.exploration.read_text(encoding='utf-8')) if args.exploration else None
    report=inventory(model,exploration)
    text=json.dumps(report,indent=2)+'\n'
    if args.output:args.output.write_text(text,encoding='utf-8')
    print('Constructs:',', '.join(f'{k}={v["instances"]}' for k,v in report['constructs'].items()))
    print('Caveats:',len(report['verification_caveats']))
    if exploration is not None:
        print('Exploration:',report['exploration'])
    if not args.output:print(text)
    return 0

if __name__=='__main__': raise SystemExit(main())
