"""Restricted single-invocation Dezyne .aut / KODA reference trace comparison.

This is a trace-projection diagnostic, NOT a compiler-correctness proof.
It deliberately excludes external abort/reset requests and invalid protocol calls.

Run: python -m koda_reference.aut_compare --aut ... --reference ...
         --async-action camera.initialize --async-action camera.process
"""
from __future__ import annotations

import argparse
from collections import Counter, defaultdict, deque
from dataclasses import dataclass
import json
from pathlib import Path
import re
from typing import Any

_AUT_HEADER = re.compile(r'^des\s*\(\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*\)\s*$')
_AUT_EDGE = re.compile(r'^\(\s*(\d+)\s*,\s*("(?:\\.|[^"\\])*")\s*,\s*(\d+)\s*\)\s*$')
_CHILD_EVENT = re.compile(r'^sut\.(.+)\.(trigger|success|failure|aborted|abort|reset|optional|Result:[A-Za-z]+)$')
_RESULTS = {"Success": "ACCEPTED", "Done": "DONE", "Failure": "FAILURE"}
_ALLOWED_TERMINALS = {"task.reply.FAILURE", "task.reply.DONE", "task.event.SUCCESS", "task.event.FAILURE", "task.event.ABORTED"}


@dataclass(frozen=True)
class LTS:
    initial: int
    transitions: tuple[tuple[int, str, int], ...]
    declared_states: int
    declared_transitions: int

    def successors(self) -> dict[int, list[tuple[str, int]]]:
        adj: dict[int, list[tuple[str, int]]] = defaultdict(list)
        for source, label, target in self.transitions:
            adj[source].append((label, target))
        return adj


def parse_aut(path: Path) -> LTS:
    with path.open(encoding="utf-8") as fh:
        header = fh.readline()
        match = _AUT_HEADER.fullmatch(header.strip())
        if not match:
            raise ValueError(f"{path}: missing Aldebaran des(initial,transitions,states) header")
        initial, number_of_edges, number_of_states = map(int, match.groups())
        edges: list[tuple[int, str, int]] = []
        for index, line in enumerate(fh, start=2):
            if not line.strip():
                continue
            edge = _AUT_EDGE.fullmatch(line.strip())
            if not edge:
                raise ValueError(f"{path}:{index}: invalid .aut transition: {line.rstrip()}")
            try:
                label = json.loads(edge.group(2))
            except json.JSONDecodeError as exc:
                raise ValueError(f"{path}:{index}: invalid .aut quoted label") from exc
            edges.append((int(edge.group(1)), label, int(edge.group(3))))
    if len(edges) != number_of_edges:
        raise ValueError(f"{path}: header declares {number_of_edges} edges, parsed {len(edges)}")
    if initial < 0 or initial >= number_of_states or any(a >= number_of_states or b >= number_of_states for a, _, b in edges):
        raise ValueError(f"{path}: state identifier exceeds header's declared state count")
    return LTS(initial, tuple(edges), number_of_states, number_of_edges)


def project_reference_trace(trace: dict[str, Any]) -> tuple[str, ...]:
    tokens: list[str] = []
    for event in trace.get("events", []):
        scope, kind, result = event.get("scope"), event.get("kind"), event.get("result")
        if scope == "task":
            if kind == "invoke" and event.get("operation") == "trigger":
                tokens.append("task.trigger")
            elif kind in ("reply", "event"):
                tokens.append(f"task.{kind}.{result}")
        elif scope == "capability":
            name = event.get("symbol")
            if not isinstance(name, str):
                raise ValueError(f"Capability event lacks symbol: {event}")
            if kind == "invoke" and event.get("operation") in ("trigger", "service"):
                tokens.append(f"{name}.{event['operation']}")
            elif kind in ("reply", "event"):
                tokens.append(f"{name}.{kind}.{result}")
    return tuple(tokens)


def read_reference(path: Path) -> set[tuple[str, ...]]:
    data = json.loads(path.read_text(encoding="utf-8"))
    if data.get("schema") != "koda.reference.exploration.v1":
        raise ValueError(f"Unexpected exploration schema: {data.get('schema')!r}")
    traces = data.get("traces", [])
    if not isinstance(traces, list):
        raise ValueError("Exploration traces must be a list")
    return {project_reference_trace(t) for t in traces}


def read_call_contracts_from_model(path: Path) -> tuple[set[str], set[str]]:
    """Read async-action and synchronous-service contracts from the IR projection.

    The contract comes from the KODA emitter, never from observed Dezyne paths.
    """
    data = json.loads(path.read_text(encoding="utf-8"))
    if data.get("schema") != "koda.reference.model.v1":
        raise ValueError(f"Unsupported KODA reference model schema: {data.get('schema')!r}")
    async_actions: set[str] = set()
    sync_services: set[str] = set()
    for instance, decl in data.get("capabilities", {}).items():
        for event, kind in decl.get("event_kinds", {}).items():
            symbol = f"{instance}.{event}"
            if kind == "async_action":
                async_actions.add(symbol)
            elif kind == "sync_service":
                sync_services.add(symbol)
    if async_actions & sync_services:
        raise ValueError("Conflicting async and service declarations")
    return async_actions, sync_services


def read_async_from_model(path: Path) -> set[str]:
    """Backward compatible helper for the first AUT comparison adapter."""
    return read_call_contracts_from_model(path)[0]


def normalize_aut(label: str) -> str | None:
    # These are Dezyne LTS simulator choices, hidden in the limited observation projection.
    # Their safe treatment outside this example must be checked against Dezyne's interfaces.
    if label == "tau" or label == "api.optional" or label.endswith(".<flush>"):
        return None
    # Timer plumbing of generated Within component: set/reset/return are
    # internal synchronous alarm administration; timeout is a *new* input.
    if re.fullmatch(r"sut\.alarm\d+\.api\.(set|reset|return)", label):
        return None
    if re.fullmatch(r"sut\.alarm\d+\.api\.timeout", label):
        return "INTERNAL_TIMEOUT"
    if label == "<defer>":
        # Internal Dezyne dispatch: macrostep boundary, not a KODA observable.
        return "INTERNAL_DEFER"
    if label in ("<illegal>", "<missing-reply>"):
        return "INVALID_PROTOCOL"
    if label == "api.trigger":
        return "task.trigger"
    if label == "api.abort" or label == "api.reset":
        return "EXTERNAL_OTHER_OPERATION"
    if label.startswith("api.Result:"):
        result = label.partition(":")[2]
        return "task.reply." + _RESULTS.get(result, f"UNRECOGNIZED({result})")
    if label in ("api.success", "api.failure", "api.aborted"):
        return "task.event." + label.rsplit(".", 1)[1].upper()
    m = _CHILD_EVENT.fullmatch(label)
    if m:
        name, operation = m.groups()
        if operation == "optional":
            return None
        if operation == "trigger":
            return name + ".trigger"
        if operation in ("success", "failure", "aborted"):
            return name + ".event." + operation.upper()
        if operation in ("abort", "reset"):
            return name + "." + operation
        result = operation.partition(":")[2]
        return name + ".reply." + _RESULTS.get(result, f"UNRECOGNIZED({result})")
    return "UNMAPPED(" + label + ")"


def enumerate_aut(
    lts: LTS, *, async_actions: set[str], max_hops: int = 60,
    max_configurations: int = 100_000, service_actions: set[str] | None = None,
) -> tuple[set[tuple[str, ...]], dict[str, int], bool]:
    adj = lts.successors()
    service_actions = service_actions or set()
    if async_actions & service_actions:
        raise ValueError("An operation cannot be both async_action and sync_service")
    starts = [b for lab,b in adj.get(lts.initial, []) if lab == "api.trigger"]
    if not starts:
        raise ValueError("No api.trigger transition from initial .aut state")
    # External environment is restricted to one initial trigger and no subsequent API calls.
    queue = deque((s, ("task.trigger",), 1) for s in starts)
    visited: set[tuple[int, tuple[str, ...]]] = set()
    terminals: set[tuple[str, ...]] = set()
    omitted: Counter[str] = Counter()
    truncated = False
    while queue:
        state, observed, hops = queue.popleft()
        if (state, observed) in visited:
            continue
        visited.add((state, observed))
        if len(visited) > max_configurations or hops >= max_hops:
            truncated = True
            continue
        for label, nxt in adj.get(state, []):
            token = normalize_aut(label)
            if token is not None and token.endswith(".trigger") and token[:-len(".trigger")] in service_actions:
                token = token[:-len(".trigger")] + ".service"
            if token is None or token in ("INTERNAL_DEFER", "INTERNAL_TIMEOUT"):
                if nxt != state:
                    queue.append((nxt, observed, hops + 1))
                continue
            if token == "EXTERNAL_OTHER_OPERATION" or token == "task.trigger":
                omitted["external operations outside single-invocation scenario"] += 1
                continue
            if token == "INVALID_PROTOCOL":
                omitted["illegal/missing-reply transitions"] += 1
                continue
            if token.startswith("UNMAPPED(") or "UNRECOGNIZED(" in token:
                # Do not silently hide unknown labels: flag the projection as incomplete.
                omitted["unmapped/unknown protocol labels: " + label] += 1
                truncated = True
                continue
            # Restrict environment responses using known async-action declarations.
            if token.endswith(".reply.DONE") or token.endswith(".reply.UNRECOGNIZED(Running)"):
                action = token.rsplit(".reply.", 1)[0]
                if action in async_actions:
                    omitted["async action cannot return " + token.rsplit(".",1)[1]] += 1
                    continue
            if token.endswith((".reply.ACCEPTED", ".reply.UNRECOGNIZED(Running)")):
                action = token.rsplit(".reply.", 1)[0]
                if action in service_actions:
                    omitted["sync service cannot return ACCEPTED/Running"] += 1
                    continue
            next_observed = observed + (token,)
            if token in _ALLOWED_TERMINALS:
                terminals.add(next_observed)
            else:
                queue.append((nxt, next_observed, hops + 1))
    return terminals, dict(sorted(omitted.items())), truncated


def compare(aut_path: Path, reference_path: Path, *, async_actions: set[str], max_hops: int = 60,
            service_actions: set[str] | None = None) -> dict[str, Any]:
    lts = parse_aut(aut_path)
    actual, omitted, truncated = enumerate_aut(lts, async_actions=async_actions, max_hops=max_hops,
                                             service_actions=service_actions)
    expected = read_reference(reference_path)
    reference_cutoff = json.loads(reference_path.read_text(encoding='utf-8')).get('cutoff_reached') is True
    common = actual & expected
    def lines(items: set[tuple[str, ...]]) -> list[list[str]]:
        return [list(t) for t in sorted(items)]
    return {
        "schema": "koda.reference.aut-comparison.v1",
        "scope": "one top-level trigger; no external abort/reset; projected task and capability events",
        "aut": {"initial": lts.initial, "states": lts.declared_states, "transitions": lts.declared_transitions},
        "environment_assumptions": {"async_actions": sorted(async_actions), "sync_services": sorted(service_actions or ()),
                                  "dezyne_optional_labels_hidden": True,
                                  "external_abort_reset_excluded": True, "invalid_protocol_transitions_excluded": True},
        "trace_counts": {"reference": len(expected), "dezyne": len(actual), "matched": len(common),
                         "reference_only": len(expected - actual), "dezyne_only": len(actual - expected)},
        "matched": lines(common), "reference_only": lines(expected - actual), "dezyne_only": lines(actual - expected),
        "omitted_branches": omitted,
        "cutoff_reached": truncated or reference_cutoff,
        "reference_cutoff_reached": reference_cutoff,
        "dezyne_cutoff_reached": truncated,
        "result": "match_under_assumptions" if actual == expected and not (truncated or reference_cutoff) else
                  ("inconclusive_cutoff" if (truncated or reference_cutoff) else "mismatch"),
        "limitations": ["Finite single-invocation comparison, not universal compiler correctness",
                        "Flat event traces are compared; exact macrostep groupings and timing are not yet checked",
                        "Reference exploration may itself be bounded; inspect its limits",
                        "Environment assumptions must come from actual capability contracts, not chosen to make traces match"],
    }


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--aut", type=Path, required=True)
    parser.add_argument("--reference", type=Path, required=True, help="Python exploration JSON")
    parser.add_argument("--async-action", action="append", default=[], metavar="INSTANCE.EVENT")
    parser.add_argument("--service-action", action="append", default=[], metavar="INSTANCE.EVENT")
    parser.add_argument("--model", type=Path, help="Emitter model; auto-discovered beside exploration by default")
    parser.add_argument("--mocks", type=Path, help="Capability fixture; auto-discovers override/default-mocks sidecar")
    parser.add_argument("--output", type=Path, help="Write JSON comparison report")
    parser.add_argument("--max-hops", type=int, default=60)
    args = parser.parse_args(argv)
    from .contracts import resolve_contracts
    contracts = resolve_contracts(args.reference, model=args.model, mocks=args.mocks,
                                  explicit_async=args.async_action, explicit_services=args.service_action)
    async_actions = set(contracts.async_actions)
    service_actions = set(contracts.service_actions)
    report = compare(args.aut, args.reference, async_actions=async_actions, max_hops=args.max_hops,
                     service_actions=service_actions)
    report['call_contracts'] = {
        'source': contracts.source,
        'model': str(contracts.model_path) if contracts.model_path else None,
        'mocks': str(contracts.mocks_path) if contracts.mocks_path else None,
        'async_actions': sorted(contracts.async_actions),
        'sync_services': sorted(contracts.service_actions),
    }
    print(f"Dezyne AUT: {report['aut']['states']} states, {report['aut']['transitions']} transitions")
    counts = report["trace_counts"]
    print(f"Traces: Python={counts['reference']}, Dezyne={counts['dezyne']}, matched={counts['matched']}, "
          f"reference-only={counts['reference_only']}, dezyne-only={counts['dezyne_only']}")
    print(f"Result: {report['result']}")
    for label, number in report["omitted_branches"].items():
        print(f"Excluded branch {label}: {number}")
    if args.output:
        args.output.write_text(json.dumps(report, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
        print(f"Report: {args.output}")
    return 0 if report['result'] == "match_under_assumptions" else 1


if __name__ == "__main__":
    raise SystemExit(main())
