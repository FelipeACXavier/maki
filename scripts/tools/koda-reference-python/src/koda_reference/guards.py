"""Conservative, pure evaluation of the source-level Choose guard subset.

Without a declared data valuation, references and capability data remain UNKNOWN.
This is intentional: the reference interpreter must not invent world state.
"""
from __future__ import annotations
from typing import Any

UNKNOWN = object()


def _literal(text: Any):
    if isinstance(text, (bool, int, float)):
        return text
    if not isinstance(text, str):
        return UNKNOWN
    value = text.strip()
    if value.lower() in ('true', 'false'):
        return value.lower() == 'true'
    try:
        return float(value) if any(x in value.lower() for x in ('.', 'e')) else int(value)
    except ValueError:
        pass
    if len(value) >= 2 and value[0] == value[-1] and value[0] in ('"', "'"):
        return value[1:-1]
    return UNKNOWN


def evaluate(expression: dict, data: dict[str, Any] | None = None):
    """Return a concrete value or UNKNOWN. Never execute user expressions."""
    if not isinstance(expression, dict) or len(expression) != 1:
        return UNKNOWN
    op, value = next(iter(expression.items()))
    if op == 'literal':
        return _literal(value)
    if op in ('data', 'reference'):
        return data.get(value, UNKNOWN) if data is not None and isinstance(value, str) else UNKNOWN
    if op == 'unary' and isinstance(value, dict):
        x = evaluate(value.get('value'), data)
        if x is UNKNOWN: return UNKNOWN
        kind = value.get('op')
        if kind in ('!', 'not'): return not bool(x)
        if kind == '-' and isinstance(x, (int, float)): return -x
        return UNKNOWN
    if op == 'binary' and isinstance(value, dict):
        lhs = evaluate(value.get('lhs'), data)
        rhs = evaluate(value.get('rhs'), data)
        operator = value.get('op')
        if operator in ('and', '&&'):
            if lhs is not UNKNOWN and not bool(lhs): return False
            if rhs is not UNKNOWN and not bool(rhs): return False
            return bool(lhs) and bool(rhs) if lhs is not UNKNOWN and rhs is not UNKNOWN else UNKNOWN
        if operator in ('or', '||'):
            if lhs is not UNKNOWN and bool(lhs): return True
            if rhs is not UNKNOWN and bool(rhs): return True
            return bool(lhs) or bool(rhs) if lhs is not UNKNOWN and rhs is not UNKNOWN else UNKNOWN
        if lhs is UNKNOWN or rhs is UNKNOWN: return UNKNOWN
        try:
            return {'==': lambda: lhs == rhs, '!=': lambda: lhs != rhs,
                    '<': lambda: lhs < rhs, '<=': lambda: lhs <= rhs,
                    '>': lambda: lhs > rhs, '>=': lambda: lhs >= rhs}.get(operator, lambda: UNKNOWN)()
        except (TypeError, ValueError):
            return UNKNOWN
    return UNKNOWN


def eligible_branches(guards: tuple[dict | None, ...], data: dict[str, Any] | None = None) -> list[int]:
    """Possible first-true branches under unknown Boolean valuations.

    Unknown guards allow both true and false, but an earlier definitely-true
    guard prevents later branches. The default (None) is selected only when
    all earlier guards can be false.
    """
    result = []
    can_reach = True
    for index, guard in enumerate(guards):
        if not can_reach: break
        outcome = True if guard is None else evaluate(guard, data)
        if outcome is UNKNOWN:
            result.append(index)
            continue
        if bool(outcome):
            result.append(index)
            can_reach = False
    return result
