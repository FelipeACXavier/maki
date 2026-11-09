"""Evaluate the finite DEFINE subset emitted by KODA for observation projection.

This is NOT an SMV model checker. Actual reachable transitions must be exported
by nuXmv; this evaluator only derives labels from each exported state/input pair.
Unsupported syntax and missing signals are fatal, rather than silently hidden.
"""
from __future__ import annotations

from dataclasses import dataclass
import re
from pathlib import Path
from typing import Any

TOKEN = re.compile(r'\s*(->|!=|<=|>=|:=|[():;=<>!&|+*/%,\-]|\d+|[A-Za-z_][A-Za-z_0-9.]*)')


def tokenize(expression: str) -> list[str]:
    i, tokens = 0, []
    while i < len(expression):
        if expression[i:].strip() == '':
            break
        match = TOKEN.match(expression, i)
        if not match:
            raise ValueError(f'Unsupported SMV syntax at {expression[i:i+30]!r}')
        tokens.append(match.group(1))
        i = match.end()
    return tokens


class Parser:
    prec = {'->': 1, '|': 2, '&': 3, '=': 4, '!=': 4, '<': 4,
            '>': 4, '<=': 4, '>=': 4, '+': 5, '-': 5,
            '*': 6, '/': 6, 'mod': 6}

    def __init__(self, text: str):
        self.tok, self.i = tokenize(text), 0

    def peek(self) -> str | None:
        return self.tok[self.i] if self.i < len(self.tok) else None

    def pop(self, token: str | None = None) -> str:
        got = self.peek()
        if got is None or token is not None and got != token:
            raise ValueError(f'Expected {token or "token"}, got {got}')
        self.i += 1
        return got

    def parse(self):
        result = self.expr(0)
        if self.peek() is not None:
            raise ValueError(f'Unexpected SMV token {self.peek()}')
        return result

    def expr(self, bound: int):
        current = self.pop()
        if current == '(':
            lhs = self.expr(0)
            self.pop(')')
        elif current == '!':
            lhs = ('not', self.expr(7))
        elif current == '-':
            lhs = ('neg', self.expr(7))
        elif current == 'case':
            clauses = []
            while self.peek() != 'esac':
                condition = self.expr(0)
                self.pop(':')
                value = self.expr(0)
                self.pop(';')
                clauses.append((condition, value))
            self.pop('esac')
            lhs = ('case', tuple(clauses))
        elif current.isdigit():
            lhs = ('value', int(current))
        else:
            lhs = ('identifier', current)
        while self.peek() in self.prec and self.prec[self.peek()] >= bound:
            op = self.pop()
            priority = self.prec[op]
            rhs = self.expr(priority if op == '->' else priority + 1)
            lhs = ('binary', op, lhs, rhs)
        return lhs


def parse_formula(text: str):
    return Parser(text).parse()


def _eval(tree, lookup) -> Any:
    tag = tree[0]
    if tag == 'value':
        return tree[1]
    if tag == 'identifier':
        if tree[1] == 'TRUE':
            return True
        if tree[1] == 'FALSE':
            return False
        return lookup(tree[1])
    if tag == 'not':
        return not bool(_eval(tree[1], lookup))
    if tag == 'neg':
        return -_eval(tree[1], lookup)
    if tag == 'case':
        for cond, value in tree[1]:
            if bool(_eval(cond, lookup)):
                return _eval(value, lookup)
        raise ValueError('SMV case expression has no true branch')
    op, lhs, rhs = tree[1:]
    a = _eval(lhs, lookup)
    if op == '&' and not bool(a):
        return False
    if op == '|' and bool(a):
        return True
    if op == '->' and not bool(a):
        return True
    b = _eval(rhs, lookup)
    operations = {
        '&': lambda: bool(a) and bool(b), '|': lambda: bool(a) or bool(b),
        '->': lambda: not bool(a) or bool(b), '=': lambda: a == b,
        '!=': lambda: a != b, '<': lambda: a < b, '>': lambda: a > b,
        '<=': lambda: a <= b, '>=': lambda: a >= b,
        '+': lambda: a + b, '-': lambda: a - b, '*': lambda: a * b,
        '/': lambda: a // b, 'mod': lambda: a % b,
    }
    return operations[op]()


@dataclass
class SmvModel:
    defines: dict[str, Any]
    init: dict[str, str]
    state_vars: set[str]
    input_vars: set[str]
    constants: set[str]
    next_expressions: dict[str, Any]
    constraints: list[Any]
    domains: dict[str, tuple[Any,...]]

    @classmethod
    def read(cls, path: Path) -> 'SmvModel':
        content = re.sub(r'--[^\n]*', '', path.read_text())
        sections = re.split(r'(?m)^\s*(IVAR|VAR|DEFINE|ASSIGN)\s*$', content)
        chunks = {}
        for i in range(1, len(sections), 2):
            name = sections[i]
            text = sections[i+1]
            # ASSIGN content ends at TRANS or property; DEFINE ends at next section.
            if name == 'ASSIGN':
                text = re.split(r'(?m)^\s*(?:TRANS|LTLSPEC|CTLSPEC|INVARSPEC)\b', text)[0]
            chunks[name] = text
        fields: dict[str,set[str]] = {'IVAR': set(), 'VAR': set()}
        enums = set()
        domains = {}
        for part in fields:
            for name, dom in re.findall(r'(?m)^\s*(\w+)\s*:\s*([^;]+);', chunks.get(part,'')):
                fields[part].add(name)
                dom = dom.strip()
                if dom.startswith('{'):
                    values=tuple(typed_values({'value':v.strip()})['value'] for v in dom[1:-1].split(','))
                    enums.update(x for x in values if isinstance(x,str))
                    domains[name]=values
                elif re.fullmatch(r'-?\d+\.\.-?\d+',dom):
                    lo,hi=map(int,dom.split('..'))
                    if hi-lo>1000:
                        raise ValueError(f'{name} input domain too large for explicit exploration')
                    domains[name]=tuple(range(lo,hi+1))
                elif dom=='boolean':
                    domains[name]=(False,True)
                else:
                    raise ValueError(f'Unsupported or infinite SMV domain {name}: {dom}')
        # More robust DEFINE extraction: a CASE includes semicolons, so parse statements
        # by tracking case nesting rather than simply splitting on ';'.
        definitions = {}
        lines = chunks.get('DEFINE','').splitlines()
        entry, buf, depth = None, [], 0
        for line in lines:
            stripped = line.strip()
            if not stripped:
                continue
            if entry is None:
                m = re.match(r'(\w+)\s*:=\s*(.*)', stripped)
                if not m:
                    raise ValueError(f'Unexpected DEFINE line: {line}')
                entry, stripped = m.group(1), m.group(2)
            buf.append(stripped)
            depth += len(re.findall(r'\bcase\b', stripped))
            depth -= len(re.findall(r'\besac\b', stripped))
            if depth == 0 and stripped.endswith(';'):
                definitions[entry] = parse_formula(' '.join(buf)[:-1])
                entry, buf = None, []
        if entry:
            raise ValueError(f'Incomplete DEFINE {entry!r}')
        assign_text=chunks.get('ASSIGN','')
        init = dict(re.findall(r'init\((\w+)\)\s*:=\s*(\w+)\s*;', assign_text))
        next_expr = {}
        for name, expr in re.findall(r'(?s)next\((\w+)\)\s*:=\s*(case\b.*?esac)\s*;',assign_text):
            next_expr[name]=parse_formula(expr)
        constraints=[]
        for line in content.splitlines():
            if re.match(r'^\s*TRANS\s+',line):
                expr=re.sub(r'^\s*TRANS\s+','',line).strip()
                if not expr.endswith(';'):
                    raise ValueError('Multiline TRANS is not supported by this projection validator')
                constraints.append(parse_formula(expr[:-1]))
        return cls(definitions, init, fields['VAR'], fields['IVAR'], enums,
                   next_expr, constraints, domains)

    def _check_domains(self,state,inputs,next_state):
        for group, expected in ((state,self.state_vars),(inputs,self.input_vars),
                                (next_state,self.state_vars) if next_state is not None else ({},set())):
            if set(group)!=expected:
                raise ValueError(f'SMV assignments mismatch. Missing={sorted(expected-set(group))}, '
                                 f'extra={sorted(set(group)-expected)}')
            for name,value in group.items():
                if value not in self.domains[name]:
                    raise ValueError(f'SMV variable {name}={value!r} is outside domain '
                                     f'{self.domains[name]!r}')

    def next_if_valid(self, state: dict[str, Any], inputs: dict[str, Any]):
        self._check_domains(state,inputs,None)
        get=self.evaluator(state,inputs)
        if any(not bool(_eval(expression,get)) for expression in self.constraints):
            return None
        result={name:_eval(expr,get) for name,expr in self.next_expressions.items()}
        if result.keys()!=self.state_vars:
            raise ValueError('SMV next assignments incomplete; cannot explore safely')
        return result

    def validate_transition(self, state: dict[str, Any], inputs: dict[str, Any],
                            next_state: dict[str, Any]):
        self._check_domains(state,inputs,next_state)
        get=self.evaluator(state,inputs)
        for expression in self.constraints:
            if not bool(_eval(expression,get)):
                raise ValueError('Explicit transition violates SMV TRANS constraint; '
                                 f'state={state}, input={inputs}, next={next_state}')
        for name,expr in self.next_expressions.items():
            value=_eval(expr,get)
            if next_state.get(name)!=value:
                raise ValueError(f'Explicit transition violates next({name}): '
                                 f'expected {value!r}, found {next_state.get(name)!r}; '
                                 f'state={state}, input={inputs}')

    def evaluator(self, state: dict[str, Any], inputs: dict[str, Any]):
        unknown = (self.state_vars - state.keys()) | (self.input_vars - inputs.keys())
        if unknown:
            raise ValueError(f'Missing SMV variable assignments: {sorted(unknown)}')
        env = dict(state)
        env.update(inputs)
        cache = {}
        in_progress = set()
        def lookup(name: str):
            if name in env:
                return env[name]
            if name in self.constants:
                return name
            if name in cache:
                return cache[name]
            if name not in self.defines:
                raise ValueError(f'Unknown SMV name {name}; cannot derive observation')
            if name in in_progress:
                raise ValueError(f'Recursive SMV DEFINE {name}')
            in_progress.add(name)
            result = _eval(self.defines[name], lookup)
            in_progress.remove(name)
            cache[name] = result
            return result
        return lookup


def typed_values(obj: dict[str, Any]):
    """CSV/JSON assignments are strings, but SMV numeric inputs are integers."""
    result = {}
    for name, value in obj.items():
        if isinstance(value, str) and re.fullmatch(r'-?\d+', value.strip()):
            result[name] = int(value.strip())
        elif isinstance(value, str) and value.strip().upper() in ('TRUE', 'FALSE'):
            result[name] = value.strip().upper() == 'TRUE'
        else:
            result[name] = value
    return result
