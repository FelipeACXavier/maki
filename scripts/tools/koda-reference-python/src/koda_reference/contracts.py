"""Resolve KODA capability call contracts for the Dezyne comparison adapters.

The compiler-emitted reference model is authoritative about call *kind*;
the sidecar mocks describe permitted outcomes.  Auto-discover both next to a
reference exploration file.  For legacy projects without model metadata we
can infer unambiguous kinds from the mock fixture, but never guess from a
FAILURE-only mock.
"""
from __future__ import annotations

from dataclasses import dataclass
import json
from pathlib import Path
import re
from typing import Iterable

from .aut_compare import read_call_contracts_from_model


@dataclass(frozen=True)
class CallContracts:
    async_actions: frozenset[str]
    service_actions: frozenset[str]
    model_path: Path | None
    mocks_path: Path | None
    source: str


def _stem(path: Path) -> str:
    name = path.stem
    for suffix in ('.abort-exploration', '.abort-traces', '.exploration', '.traces', '.trace'):
        match = re.fullmatch(r'(.+)' + re.escape(suffix) + r'(?:\(\d+\))?', name)
        if match:
            return match.group(1)
    return name


def _discover(path: Path, suffixes: tuple[str, ...]) -> Path | None:
    stem = _stem(path)
    for suffix in suffixes:
        candidate = path.with_name(stem + suffix)
        if candidate.is_file():
            return candidate
    return None


def _mocks_data(path: Path) -> dict:
    data = json.loads(path.read_text(encoding='utf-8'))
    if not isinstance(data, dict):
        raise ValueError(f'{path}: expected mock object')
    schema = data.get('schema')
    if schema is not None and schema != 'koda.reference.mocks.v1':
        raise ValueError(f'{path}: unsupported mock schema {schema!r}')
    mocks = data.get('mocks', data)
    if not isinstance(mocks, dict):
        raise ValueError(f'{path}: mocks must be an object')
    return mocks


def infer_contracts_from_mocks(path: Path) -> tuple[set[str], set[str]]:
    """Fallback inference when emitter model metadata is unavailable.

    This only identifies mock shapes with informative replies.  In general a
    mock does NOT determine whether a call is a service, so users should
    prefer the compiler-emitted event_kinds field.
    """
    async_actions: set[str] = set()
    services: set[str] = set()
    for instance, events in _mocks_data(path).items():
        if not isinstance(events, dict):
            raise ValueError(f'{path}: {instance} mocks must be an object')
        for name, cfg in events.items():
            label = f'{instance}.{name}'
            if not isinstance(cfg, dict):
                raise ValueError(f'{path}: {label} must be an object')
            outcomes = cfg.get('outcomes', cfg.get('trigger'))
            if not isinstance(outcomes, list) or not outcomes:
                raise ValueError(f'{path}: {label} requires nonempty outcomes')
            if any(not isinstance(item, dict) for item in outcomes):
                raise ValueError(f'{path}: invalid outcomes for {label}')
            replies = {item.get('reply') for item in outcomes}
            if not replies <= {'ACCEPTED', 'DONE', 'FAILURE'}:
                raise ValueError(f'{path}: invalid outcomes for {label}')
            if 'DONE' in replies and 'ACCEPTED' in replies:
                raise ValueError(f'{path}: {label} mixes DONE and ACCEPTED; cannot infer call kind')
            if 'ACCEPTED' in replies:
                async_actions.add(label)
            elif 'DONE' in replies and not ('abort' in cfg or 'reset' in cfg):
                # Generated synchronous service mocks have no abort/reset.
                # This is a fallback convention, not a proof of call kind.
                services.add(label)
            elif replies == {'FAILURE'} and not ('abort' in cfg or 'reset' in cfg):
                raise ValueError(f'{path}: {label} has only FAILURE outcomes; pass --model to determine its kind')
    return async_actions, services


def resolve_contracts(reference: Path, *, model: Path | None = None, mocks: Path | None = None,
                      explicit_async: Iterable[str] = (), explicit_services: Iterable[str] = ()) -> CallContracts:
    if model is None:
        model = _discover(reference, ('.reference.json',))
    elif not model.is_file():
        raise FileNotFoundError(f'Model not found: {model}')
    if mocks is None:
        # Explicit --model wins over possibly unrelated sidecars beside a
        # reference exploration from a different checkout or filename suffix.
        # In particular, do not combine new emitter contracts with old mock
        # scripts discovered near an uploaded exploration file.
        if model is not None and model.name.endswith('.reference.json'):
            prefix = model.name[:-len('.reference.json')]
            for suffix in ('.mocks.json', '.default-mocks.json'):
                candidate = model.with_name(prefix + suffix)
                if candidate.is_file():
                    mocks = candidate
                    break
        if mocks is None:
            mocks = _discover(reference, ('.mocks.json', '.default-mocks.json'))
    elif not mocks.is_file():
        raise FileNotFoundError(f'Mock fixture not found: {mocks}')

    declared_async: set[str] = set()
    declared_services: set[str] = set()
    if model is not None:
        declared_async, declared_services = read_call_contracts_from_model(model)
        if mocks is not None:
            # Validate all fixture outcomes against model-declared kinds, without
            # interpreting outcomes as a substitute for their declarations.
            from .model import attach_mocks
            attach_mocks(json.loads(model.read_text(encoding='utf-8')),
                         json.loads(mocks.read_text(encoding='utf-8')))
        if declared_async or declared_services:
            source = 'emitter_model'
        elif mocks is not None:
            declared_async, declared_services = infer_contracts_from_mocks(mocks)
            source = 'mock_inference_ambiguous_sync_actions'
        else:
            source = 'explicit_only_missing_model_metadata'
    elif mocks is not None:
        declared_async, declared_services = infer_contracts_from_mocks(mocks)
        source = 'mock_inference_ambiguous_sync_actions'
    else:
        source = 'explicit_flags_only'

    explicitly_async = set(explicit_async)
    explicitly_services = set(explicit_services)
    if declared_async & explicitly_services or declared_services & explicitly_async:
        raise ValueError('Explicit --async-action/--service-action contradicts the inferred or declared contract')
    async_actions = declared_async | explicitly_async
    services = declared_services | explicitly_services
    if async_actions & services:
        raise ValueError('Operation declared both asynchronous action and synchronous service')
    if not (async_actions or services):
        raise ValueError('No call contracts found. Pass --model Task.reference.json (preferred), '
                         '--mocks Task.default-mocks.json, or explicit call-type flags.')
    return CallContracts(frozenset(async_actions), frozenset(services), model, mocks, source)
