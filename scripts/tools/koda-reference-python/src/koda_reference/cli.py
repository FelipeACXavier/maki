"""KODA reference explorer CLI, structured trace output schema v1."""
import argparse
import json
import sys
from pathlib import Path
from .model import validate_scenario, attach_mocks
from .engine import Engine, ChoiceNeeded


def export_run(run):
    return {'schema': 'koda.reference.trace.v1',
            'events': run.normalized_trace(),
            'macrosteps': run.history,
            'states': run.state_snapshot(),
            'pending': [{'source':p.source,'generation':p.generation,'kind':p.kind} for p in run.pending]}


def main():
    parser = argparse.ArgumentParser(description='KODA independent reference explorer')
    parser.add_argument('scenario', type=Path)
    parser.add_argument('--mode', choices=('run','explore','explore-abort','explore-abort-complete','aut'), default='run')
    parser.add_argument('--mocks',type=Path,help='Separate capability outcome fixtures for IR-emitted model')
    parser.add_argument('--max-dispatches',type=int,default=15)
    parser.add_argument('--max-traces',type=int,default=2000)
    parser.add_argument('--output',type=Path,help='Write stable machine-readable JSON')
    parser.add_argument('--abort-after-trigger',action='store_true')
    parser.add_argument('--reset-after',action='store_true')
    # AUT mode is a separate graph exploration; original trace modes unchanged.
    parser.add_argument('--max-states',type=int,default=5000,
                        help='AUT mode: maximum stable runtime states')
    parser.add_argument('--max-transitions',type=int,default=50000,
                        help='AUT mode: maximum macrostep transitions')
    parser.add_argument('--aut-include-abort',action='store_true',
                        help='AUT mode: allow one external task.abort at stable Running/Handling states')
    parser.add_argument('--aut-include-reset',action='store_true',
                        help='AUT mode: allow one external task.reset in Error')
    parser.add_argument('--aut-observe',choices=('task','orchestration','all'),
                        default='orchestration',
                        help='AUT mode: visible action alphabet (others become tau)')
    parser.add_argument('--aut-macro-boundaries',action='store_true',
                        help='AUT mode: emit explicit macro.begin / macro.end labels')
    parser.add_argument('--aut-report',type=Path,
                        help='AUT mode: optional JSON of full macrostep graph and completeness')
    args = parser.parse_args()
    scenario = json.loads(args.scenario.read_text(encoding='utf-8'))
    mock_path = args.mocks
    if mock_path is None and args.scenario.name.endswith('.reference.json'):
        # Generated reference models have a matching sidecar produced by the emitter.
        stem = args.scenario.name[:-len('.reference.json')]
        candidates = [args.scenario.with_name(stem + '.mocks.json'),
                      args.scenario.with_name(stem + '.default-mocks.json')]
        mock_path = next((candidate for candidate in candidates if candidate.is_file()), None)
        if mock_path is None:
            parser.error(f'no mock sidecar found; expected {candidates[1]} (generated) '
                         f'or {candidates[0]} (override), or pass --mocks explicitly')
    if mock_path is not None:
        scenario = attach_mocks(scenario,json.loads(mock_path.read_text(encoding='utf-8')))
    root,caps = validate_scenario(scenario)
    engine = Engine(root,caps)
    if args.mode == 'aut':
        from .lts_export import explore_lts, aut_text, write_aut
        graph = explore_lts(engine, max_states=args.max_states,
                            max_transitions=args.max_transitions,
                            include_abort=args.aut_include_abort,
                            include_reset=args.aut_include_reset)
        if args.output:
            # Always emit a completeness/diagnostics report with an AUT.
            report_path = args.aut_report or args.output.with_name(args.output.name + '.report.json')
            write_aut(graph, args.output, observe=args.aut_observe,
                      macro_boundaries=args.aut_macro_boundaries, report_path=report_path)
            print(f'AUT: {args.output} | stable states: {len(graph.states)} | '
                  f'macrosteps: {len(graph.edges)} | complete: {graph.complete} | '
                  f'report: {report_path}', file=sys.stderr)
        else:
            print(aut_text(graph, observe=args.aut_observe,
                           macro_boundaries=args.aut_macro_boundaries), end='')
            print(f'AUT complete: {graph.complete}; stable states: {len(graph.states)}; '
                  f'macrosteps: {len(graph.edges)}', file=sys.stderr)
        return
    if args.mode in ('explore-abort','explore-abort-complete'):
        runs, cutoff = (engine.explore_abort_complete_runs(args.max_dispatches, args.max_traces)
                        if args.mode == 'explore-abort-complete'
                        else engine.explore_abort_runs(args.max_dispatches, args.max_traces))
        result = {'schema': 'koda.reference.exploration.v1',
                  'scenario': args.mode,
                  'traces': [export_run(run) for run in runs],
                  'bounded': True, 'cutoff_reached': cutoff,
                  'max_dispatches': args.max_dispatches,
                  'max_traces': args.max_traces}
    elif args.mode == 'explore':
        runs = engine.explore_runs(args.max_dispatches,args.max_traces)
        result = {'schema':'koda.reference.exploration.v1', 'traces':[export_run(run) for run in runs],
                  'bounded': True, 'cutoff_reached':engine.last_cutoff, 'max_dispatches':args.max_dispatches,
                  'max_traces':args.max_traces}
    else:
        try:
            engine.trigger()
            if args.abort_after_trigger and engine.state[root.path].phase == 'Running':
                engine.abort()
            while engine.pending:
                engine.dispatch(0)
            if args.reset_after and engine.state[root.path].phase == 'Error':
                engine.reset()
        except ChoiceNeeded:
            parser.error('nondeterministic mock outcomes require --mode explore')
        result = export_run(engine)
    output = json.dumps(result, indent=2)
    if args.output:
        args.output.write_text(output+'\n',encoding='utf-8')
    else:
        print(output)

if __name__ == '__main__':
    main()
