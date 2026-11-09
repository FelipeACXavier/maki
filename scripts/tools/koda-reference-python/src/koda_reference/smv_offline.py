"""EXPERIMENTAL standalone finite-state SMV expander for KODA-generated subset.

This is not nuXmv. It directly evaluates DEFINE, ASSIGN next, TRANS under
finite IVAR/VAR domains and can be useful to debug tiny models when the nuXmv
binary is unavailable. The authoritative LTS should be exported by nuXmv.
"""
from __future__ import annotations

from collections import deque
import itertools
from pathlib import Path

from .smv_observations import SmvModel


def enumerate_finite(smv: Path, max_states: int=1000, max_choices: int=5_000_000):
    model=SmvModel.read(smv)
    initial={key:tuple(model.domains[key])[0] for key in model.state_vars}
    initial.update({key: next((v for v in model.domains[key] if str(v)==s),None)
                    for key,s in model.init.items()})
    if None in initial.values():
        raise ValueError('SMV initial values are not in variable domains')
    state_names=sorted(model.state_vars)
    input_names=sorted(model.input_vars)
    def identity(state):return tuple(state[key] for key in state_names)
    states={identity(initial): 's0'}
    q=deque([initial])
    edges={}
    budget=0
    while q:
        state=q.popleft()
        source=states[identity(state)]
        for values in itertools.product(*(model.domains[k] for k in input_names)):
            budget+=1
            if budget>max_choices:
                raise ValueError(f'Candidate budget exceeded ({max_choices}); export native nuXmv state space')
            inputs=dict(zip(input_names,values))
            # This tests the whole transition relation, INCLUDING global TRANS.
            successor=model.next_if_valid(state,inputs)
            if successor is None:
                continue
            key=identity(successor)
            if key not in states:
                if len(states)>=max_states:
                    raise ValueError(f'State limit exceeded ({max_states}); export native nuXmv graph')
                states[key]=f's{len(states)}'
                q.append(successor)
            target=states[key]
            signature=(source,target,tuple(inputs.items()))
            edges[signature]={'source':source,'target':target,'state':state,
                              'input':inputs,'next_state':successor}
    return {'initial':'s0','transitions':list(edges.values()),
            'candidate_inputs_checked':budget,'state_count':len(states),
            'source':'experimental Python evaluation of a finite SMV subset, NOT native nuXmv'}
