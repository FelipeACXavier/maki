"""Scalable coarse KODA/Dezyne conformance diagnostic for large AUT graphs.

This compares only task-facing event *prefixes*. Capability calls, guards,
timers and interleavings are hidden, so even perfect prefix agreement does
NOT prove faithful internal behavior, concurrency, timing, or compiler correctness.
No claim of equivalence is made when the reference is bounded/truncated.
"""
from __future__ import annotations
from collections import Counter,deque
from dataclasses import dataclass
import json
import re
from pathlib import Path
import argparse
from .aut_compare import parse_aut
from .contracts import resolve_contracts

@dataclass(frozen=True)
class Config:
    state:int
    events:tuple[str,...]
    operation:str|None
    started:bool
    aborted:bool
    handling:bool
    pending_abort_notification:bool
    hops:int


def task_trace_reference(trace:dict)->tuple[str,...]:
    items=[]
    for ev in trace.get('events',[]):
        if ev.get('scope')!='task':continue
        op,kind,res=ev.get('operation'),ev.get('kind'),ev.get('result')
        if kind=='invoke' and op in ('trigger','abort'):
            items.append('task.'+op)
        elif kind=='reply':
            items.append(f'task.{op}.{res}')
        elif kind=='event':
            items.append('task.event.'+res)
    return tuple(items)


def action_name(raw:str)->str:
    """Dezyne ports are suffixed _N per *call-site*; not a KODA event name.

    This is only a nominal heuristic for the supplied hospital fixture; confirm
    against generated wiring before treating matches as semantic evidence.
    """
    inst,_,ev=raw.partition('.')
    return f'{inst}.{re.sub(r"_\\d+$","",ev)}'


def task_prefixes_aut(lts,*,scenario='normal',async_actions=frozenset(),service_actions=frozenset(),
                     max_events=8,max_hops=140,max_configurations=150_000):
    adj=lts.successors()
    work=deque([Config(lts.initial,(),None,False,False,False,False,0)])
    seen=set(); terminals=set(); prefixes=set();omitted=Counter();cutoff=False
    while work:
        cfg=work.popleft()
        key=(cfg.state,cfg.events,cfg.operation,cfg.started,cfg.aborted,cfg.handling,cfg.pending_abort_notification)
        if key in seen:continue
        seen.add(key)
        if cfg.events:prefixes.add(cfg.events)
        if len(seen)>max_configurations or cfg.hops>=max_hops:
            cutoff=True
            if len(seen)>max_configurations:break
            continue
        for label,nxt in adj.get(cfg.state,[]):
            ev=cfg.events;op=cfg.operation;started=cfg.started
            aborted=cfg.aborted;handling=cfg.handling;pending_ack=cfg.pending_abort_notification;finish=False
            if label in ('<illegal>','<missing-reply>','<non-compliance>'):
                omitted['illegal / non-compliant']+=1;continue
            if label=='api.trigger':
                if started:continue
                started=True;op='trigger';ev=ev+('task.trigger',)
            elif label=='api.abort':
                if scenario!='abort' or not started or aborted or op is not None:
                    continue
                aborted=True;op='abort';ev=ev+('task.abort',)
            elif label=='api.reset':
                continue
            elif label.startswith('api.Result:'):
                if op is None:
                    omitted['unpaired top-level result']+=1;continue
                result=label.partition(':')[2]
                if op=='trigger':
                    result={'Success':'ACCEPTED','Failure':'FAILURE','Done':'DONE',
                            'Terminated':'TERMINATE'}.get(result,result.upper())
                    ev=ev+(f'task.reply.{result}',)
                    finish=result!='ACCEPTED'
                elif op=='abort':
                    result=result.upper()
                    if pending_ack and result!='SUCCESS':
                        ev=ev+('task.event.ABORTED',)
                    pending_ack=False
                    ev=ev+(f'task.abort_reply.{result}',)
                    handling=result=='HANDLING'
                    finish=not handling
                op=None
            elif label in ('api.success','api.failure','api.terminated','api.aborted'):
                outcome={'success':'SUCCESS','failure':'FAILURE','terminated':'SUCCESS','aborted':'ABORTED'}[label.split('.')[-1]]
                # A redundant notification during a successful synchronous
                # abort is projected as a protocol-level acknowledgement.
                if label=='api.aborted' and op=='abort':
                    # Buffer until the reply confirms a successful synchronous
                    # cancellation; other notifications must remain visible.
                    pending_ack=True
                else:
                    ev=ev+('task.event.'+outcome,)
                    finish=True
            elif label.startswith('sut.'):
                # Suppress invalid generic Dezyne async-DONE branches using
                # contracts declared by the emitter, not observed results.
                if '.Result:Done' in label:
                    raw=label[4:].split('.Result:Done',1)[0]
                    if action_name(raw) in async_actions:
                        omitted['async action DONE']+=1;continue
                elif '.Result:Success' in label:
                    raw=label[4:].split('.Result:Success',1)[0]
                    if action_name(raw) in service_actions:
                        omitted['sync service ACCEPTED']+=1;continue
                # Timers, condition responses, and per-call events are hidden
                # only in this explicitly task-facing projection.
                pass
            elif label=='tau' or label=='<defer>' or label.endswith('.<flush>'):
                pass
            else:
                omitted['unknown label: '+label]+=1;cutoff=True;continue
            if len(ev)>max_events:
                cutoff=True
                continue
            if finish:
                terminals.add(ev)
                prefixes.add(ev)
            else:
                work.append(Config(nxt,ev,op,started,aborted,handling,pending_ack,cfg.hops+1))
    return prefixes,terminals,dict(omitted),cutoff,len(seen)


def compare_task_projection(aut:Path,reference:Path,*,scenario='normal',async_actions=frozenset(),
                            service_actions=frozenset(),max_hops=140,max_events=8,max_configurations=150_000):
    lts=parse_aut(aut)
    ref=json.loads(reference.read_text())
    if ref.get('schema')!='koda.reference.exploration.v1':raise ValueError('invalid reference exploration')
    expected={task_trace_reference(t) for t in ref['traces']}
    prefixes,terminals,omitted,cutoff,configs=task_prefixes_aut(lts,scenario=scenario,
        async_actions=async_actions,service_actions=service_actions,max_hops=max_hops,
        max_events=max_events,max_configurations=max_configurations)
    found=expected&prefixes
    # Every observed reference prefix is a witness target, not an obligation
    # for the Dezyne backend to exhaust all behaviors under finite bounds.
    return {'schema':'koda.reference.task-prefix-comparison.v1',
            'scenario':scenario,'observation':'task-interface-only',
            'reference_count':len(expected),'reference_prefixes_reachable':len(found),
            'reference_prefixes_not_witnessed':len(expected-found),
            'missing_reference_prefixes':[list(x) for x in sorted(expected-found)],
            'aut_task_terminal_sequences':[list(x) for x in sorted(terminals)[:100]],
            'aut_states_visited':configs,'cutoff_reached':cutoff or ref.get('cutoff_reached',False),
            'omitted':omitted,'result':'bounded_prefix_witnesses_only',
            'limitations':['Hides *all* capability-level calls, shared-resource ownership, condition evaluation and timers',
                           'AUT port suffixes normalized heuristically by removing _N',
                           'Not trace-language equivalence or a proof of compiler correctness',
                           'Unbounded loops cannot be exhausted by bounded exploration']}


def main(argv=None):
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--aut',required=True,type=Path);p.add_argument('--reference',required=True,type=Path)
    p.add_argument('--model',type=Path);p.add_argument('--mocks',type=Path)
    p.add_argument('--scenario',choices=('normal','abort'),default='normal')
    p.add_argument('--max-hops',type=int,default=140)
    p.add_argument('--max-events',type=int,default=8)
    p.add_argument('--max-configurations',type=int,default=150000)
    p.add_argument('--output',type=Path)
    a=p.parse_args(argv)
    contracts=resolve_contracts(a.reference,model=a.model,mocks=a.mocks)
    r=compare_task_projection(a.aut,a.reference,scenario=a.scenario,
        async_actions=contracts.async_actions,service_actions=contracts.service_actions,
        max_hops=a.max_hops,max_events=a.max_events,max_configurations=a.max_configurations)
    r['contract_source']=contracts.source
    if a.output:a.output.write_text(json.dumps(r,indent=2)+'\n')
    print(f"Task prefix witnesses {r['reference_prefixes_reachable']}/{r['reference_count']}; "
          f"AUT configurations {r['aut_states_visited']}; cutoff={r['cutoff_reached']} "
          f"(not equivalence)")
    return 0

if __name__=='__main__':raise SystemExit(main())
