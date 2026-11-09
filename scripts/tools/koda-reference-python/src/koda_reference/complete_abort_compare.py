"""Bounded Dezyne comparison following one external abort *through* HANDLING.

This diagnostic distinguishes the immediate abort reply from any subsequent
handler completion. It is not a bisimulation or timing proof. Unknown external
labels invalidate coverage rather than being silently hidden.
"""
from __future__ import annotations
from collections import Counter, deque
from dataclasses import dataclass
from pathlib import Path
import argparse
import json
from .aut_compare import parse_aut, _CHILD_EVENT
from .abort_compare import project_reference_abort, _project_dezyne_abort_ack
from .contracts import resolve_contracts

@dataclass(frozen=True)
class State:
    lts: int
    prior: tuple[tuple[str,...], ...]
    step: tuple[str,...]
    running: bool
    abort_requested: bool
    task_op: str | None
    child_ops: tuple[tuple[str,str], ...]
    handling: bool
    hops: int
    pre_steps: int
    post_steps: int


def enumerate_complete(lts, *, async_actions: set[str], service_actions: set[str],
                       max_hops=150, max_configurations=150_000,
                       max_pre_abort_steps=3, max_post_abort_steps=4):
    if async_actions & service_actions:
        raise ValueError('conflicting operation contracts')
    adj = lts.successors()
    queue = deque(State(nxt, (), ('task.trigger',), False, False, 'trigger', (), False, 1, 0, 0)
                  for label,nxt in adj.get(lts.initial, ()) if label=='api.trigger')
    if not queue:
        raise ValueError('no initial api.trigger transition')
    seen=set(); traces=set(); omitted=Counter(); cutoff=False
    while queue:
        cfg=queue.popleft()
        key=(cfg.lts, cfg.prior, cfg.step, cfg.running, cfg.abort_requested,
             cfg.task_op, cfg.child_ops, cfg.handling)
        if key in seen: continue
        seen.add(key)
        if len(seen)>max_configurations or cfg.hops>=max_hops:
            cutoff=True
            if len(seen)>max_configurations: break
            continue
        for label,nxt in adj.get(cfg.lts, ()):
            prior,step=cfg.prior,cfg.step
            running,aborted=cfg.running,cfg.abort_requested
            task_op,handling=cfg.task_op,cfg.handling
            child_ops=dict(cfg.child_ops)
            pre_steps,post_steps=cfg.pre_steps,cfg.post_steps
            token=None;finished=False;external=False
            match=_CHILD_EVENT.fullmatch(label)
            if label in ('tau','api.optional') or label.endswith('.<flush>') or (match and match.group(2)=='optional'):
                pass
            elif label in ('<illegal>','<missing-reply>'):
                omitted['invalid protocol']+=1; continue
            elif label=='api.abort':
                if not running or aborted or task_op is not None:
                    omitted['abort outside stable Running']+=1; continue
                aborted=True;task_op='abort'
                prior=prior+(step,);step=('task.abort',)
            elif label in ('api.trigger','api.reset'):
                omitted['other API invocation']+=1;continue
            elif label.startswith('api.Result:'):
                result=label.split(':',1)[1]
                if task_op=='trigger':
                    if result!='Success':
                        omitted['not Running after trigger']+=1;continue
                    token='task.reply.ACCEPTED';running=True;task_op=None
                elif task_op=='abort':
                    token='task.abort_reply.'+result.upper()
                    task_op=None;handling=result=='Handling'
                    if result in ('Success','Failure','Error'):finished=True
                    elif not handling:
                        omitted['unknown abort result '+result]+=1;cutoff=True;continue
                else:
                    omitted['unpaired API reply']+=1;cutoff=True;continue
            elif label in ('api.success','api.failure','api.aborted','api.terminated'):
                result={'success':'SUCCESS','failure':'FAILURE','aborted':'ABORTED','terminated':'SUCCESS'}[label.split('.')[-1]]
                token='task.event.'+result
                if not aborted:
                    omitted['completed before abort']+=1;continue
                if handling and task_op is None:
                    finished=True
            elif match:
                name,op=match.groups()
                if op in ('trigger','abort','reset'):
                    if name in child_ops:
                        omitted['overlapping child call '+name]+=1;cutoff=True;continue
                    child_ops[name]=op
                    token=name+('.service' if op=='trigger' and name in service_actions else '.'+op)
                elif op.startswith('Result:'):
                    result=op.partition(':')[2]
                    operation=child_ops.pop(name,None)
                    if operation is None:
                        omitted['unpaired child reply '+name]+=1;cutoff=True;continue
                    if operation=='trigger':
                        if result=='Done' and name in async_actions:
                            omitted['async DONE excluded']+=1;continue
                        if result in ('Success','Running') and name in service_actions:
                            omitted['sync ACCEPTED excluded']+=1;continue
                        mapped={'Success':'ACCEPTED','Done':'DONE','Failure':'FAILURE','Terminated':'TERMINATE'}.get(result,result.upper())
                        token=f'{name}.reply.{mapped}'
                    else:token=f'{name}.{operation}_reply.{result.upper()}'
                elif op in ('success','failure','aborted','terminated'):
                    token=f'{name}.event.{op.upper()}'
                    external=name not in child_ops or child_ops[name]!='abort'
                    if external:
                        if not aborted and pre_steps>=max_pre_abort_steps:
                            omitted['pre-abort step bound']+=1;cutoff=True;continue
                        if aborted and post_steps>=max_post_abort_steps:
                            omitted['post-abort step bound']+=1;cutoff=True;continue
                else:
                    omitted['unknown child event '+label]+=1;cutoff=True;continue
            else:
                omitted['unknown AUT label '+label]+=1;cutoff=True;continue
            if external:
                prior=prior+(step,);step=()
                if aborted: post_steps+=1
                else: pre_steps+=1
            if token is not None:step=step+(token,)
            if finished:
                traces.add(prior+(step,));continue
            queue.append(State(nxt,prior,step,running,aborted,task_op,
                               tuple(sorted(child_ops.items())),handling,cfg.hops+1,
                               pre_steps,post_steps))
    return traces,dict(sorted(omitted.items())),cutoff


def compare_complete(aut:Path, reference:Path, *, async_actions:set[str],service_actions:set[str],
                     notification_policy='dezyne_ack',max_hops=150,max_configurations=150_000,
                     max_pre_abort_steps=3,max_post_abort_steps=4):
    ref=json.loads(reference.read_text())
    if ref.get('schema')!='koda.reference.exploration.v1' or ref.get('scenario')!='explore-abort-complete':
        raise ValueError('requires --mode explore-abort-complete reference')
    expected={project_reference_abort(t) for t in ref['traces']}
    lts=parse_aut(aut)
    actual, omitted,cutoff=enumerate_complete(lts,async_actions=async_actions,service_actions=service_actions,
                      max_hops=max_hops,max_configurations=max_configurations,
                      max_pre_abort_steps=max_pre_abort_steps,max_post_abort_steps=max_post_abort_steps)
    if notification_policy=='dezyne_ack':
        actual={_project_dezyne_abort_ack(x) for x in actual}
    elif notification_policy!='strict':
        raise ValueError('unknown notification policy')
    cutoff=cutoff or bool(ref.get('cutoff_reached',False))
    missing=expected-actual;extra=actual-expected
    def ordered(xs):return [[list(s) for s in t] for t in sorted(xs)]
    def shortest(xs):return [list(s) for s in min(xs,key=lambda t:(sum(map(len,t)),t))] if xs else None
    verdict=('mismatch_observed_with_cutoff' if cutoff else 'mismatch') if extra else (
        'inconclusive_cutoff' if cutoff else 'mismatch' if missing else 'match_under_assumptions')
    return {'schema':'koda.reference.complete-abort-comparison.v1',
            'scenario':'one_external_abort_including_post_handling',
            'aut':{'states':lts.declared_states,'transitions':lts.declared_transitions},
            'counts':{'reference':len(expected),'dezyne':len(actual),'matched':len(actual&expected),
                      'reference_only':len(missing),'dezyne_only':len(extra)},
            'result':verdict,'abort_notification_policy':notification_policy,'cutoff_reached':cutoff,
            'shortest_reference_only':shortest(missing),'shortest_dezyne_only':shortest(extra),
            'reference_only':ordered(missing),'dezyne_only':ordered(extra),'omitted':omitted,
            'assumptions':['one external abort at a stable running macrostep',
                           'follow HANDLING until task completion, no further external abort/reset',
                           'no real-time clock semantics; per-macrostep boundaries inferred',
                           'capability outcomes filtered by emitter-declared contracts'],
            'limitations':['finite bounded trace-language comparison, not bisimulation',
                           'reference may be bounded and unbounded Repeat is not exhausted',
                           'Dezyne AUT may include implementation-specific observer events']}


def main(argv=None):
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--aut',type=Path,required=True);p.add_argument('--reference',type=Path,required=True)
    p.add_argument('--model',type=Path);p.add_argument('--mocks',type=Path)
    p.add_argument('--async-action',action='append',default=[])
    p.add_argument('--service-action',action='append',default=[])
    p.add_argument('--abort-notification-policy',default='dezyne_ack',choices=('strict','dezyne_ack'))
    p.add_argument('--max-hops',type=int,default=150)
    p.add_argument('--max-configurations',type=int,default=150_000)
    p.add_argument('--max-pre-abort-steps',type=int,default=3)
    p.add_argument('--max-post-abort-steps',type=int,default=4)
    p.add_argument('--output',type=Path)
    a=p.parse_args(argv)
    contracts=resolve_contracts(a.reference,model=a.model,mocks=a.mocks,
                               explicit_async=a.async_action,explicit_services=a.service_action)
    r=compare_complete(a.aut,a.reference,async_actions=set(contracts.async_actions),
        service_actions=set(contracts.service_actions),notification_policy=a.abort_notification_policy,
        max_hops=a.max_hops,max_configurations=a.max_configurations,
        max_pre_abort_steps=a.max_pre_abort_steps,max_post_abort_steps=a.max_post_abort_steps)
    r['call_contracts']={'source':contracts.source,'model':str(contracts.model_path) if contracts.model_path else None,
                         'async_actions':sorted(contracts.async_actions),'sync_services':sorted(contracts.service_actions)}
    print('Complete abort macrosteps:',r['counts'],'result:',r['result'],'cutoff:',r['cutoff_reached'])
    if r['shortest_dezyne_only']:
        print('Shortest extra Dezyne trace:')
        for i,step in enumerate(r['shortest_dezyne_only']):print(' ',i,' -> '.join(step))
    if a.output:a.output.write_text(json.dumps(r,indent=2)+'\n')
    return 0 if r['result']=='match_under_assumptions' else 1


if __name__=='__main__':raise SystemExit(main())
