#!/usr/bin/python3

from __future__ import annotations

import argparse
import json
import re
import shutil
import sys
import time
from fnmatch import fnmatch
from dataclasses import dataclass
from datetime import datetime
from pathlib import Path


@dataclass
class Config:
    debug: bool
    backup: bool
    dry_run: bool
    print_stream: bool
    source: bool


HEADER_EXTENSIONS = {'.h', '.hpp', '.hh', '.hxx'}
SOURCE_EXTENSIONS = {'.cpp', '.cc', '.cxx', '.c++'}

SOURCE_PROMPT = """

Use the SOURCE file only as context.
Never modify the SOURCE file.

SOURCE FILE STARTS BELOW:
{source_code}
SOURCE FILE ENDS ABOVE.
"""

PROMPT_TEMPLATE = """You are an automated C++ refactoring tool.

You must output ONLY the complete updated HEADER file.
Do not explain anything.
Do not summarise anything.
Do not use Markdown.
Do not wrap the result in ```cpp fences.

Your only allowed edit is to insert Doxygen comments into the HEADER file.

If declarations are missing useful Doxygen comments, add them.
If no comments are needed, output the original HEADER file exactly.

Required Doxygen style:
- Use /** ... */ comments.
- Use @brief for classes, structs, enums, and functions.
- Use @param for non-obvious parameters.
- Use @return for non-void return values.
- Keep comments concise and factual.
- Return the complete updated header file.
- Prefer concise comments unless it is a class description.

Hard constraints:
- Preserve all existing code.
- Preserve ordering.
- Preserve signatures.
- Preserve includes.
- Preserve namespaces.
- Preserve formatting as much as possible.
- Do not invent behaviour.
- Do not remove any classes, methods, signals, slots, enums, structs, or members.
- Do not remove implementations from the header description.
- Document all classes, methods, signals, slots, enums, structs, and members.

Struct and member documentation rules:
- Document every struct with a Doxygen block comment using @brief.
- Document struct fields and class member variables with one-line Doxygen comments using ///.
- Place member comments immediately after the member.
- Do not skip members with default values.
- Do not stop after the first struct or member; process the entire header file.
- Continue until the full header has been returned.
- For simple data members, prefer this style:

Type name = defaultValue;  /// Short factual description.

HEADER FILE STARTS BELOW:
{header_code}

HEADER FILE ENDS ABOVE.
{source_code}
"""

PATCH_PROMPT_TEMPLATE = """You are a C++ Doxygen documentation assistant.

Return ONLY valid JSON.
Do not return Markdown.
Do not explain anything.
Do not return the full file.

Your task is to propose Doxygen comments to insert into the HEADER file.

The JSON must be an array of objects.
Each object must have:
- "line": the 1-based line number with the destination of the comment
- "comment": the exact Doxygen comment text to insert

Rules:
- Only insert comments.
- Never modify existing code.
- Do not suggest comments for declarations that already have useful Doxygen comments.
- Document classes, structs, enums, public methods, signals, slots, properties, fields, and members.
- Document structs with block comments using /** ... */.
- Document struct/class member variables with one-line comments using ///.
- Keep comments concise and factual.
- Use the SOURCE file only as context.
- Do not invent behaviour.
- Process the entire header from top to bottom.
- The "line" value is the ORIGINAL line number before which the comment will be inserted.
- Always use the line number of the declaration itself.
- For a struct, class, enum, or function, use the line containing `struct`, `class`, `enum`, or the function signature.
    - Do NOT use the line containing only `{{`.
    - Do NOT use the first member line.
    - Do NOT account for comments inserted earlier.
    - Do NOT shift line numbers.
    - Every insertion line must refer to the original numbered header exactly as shown.
    - The script applies insertions from bottom to top.
- For long comments, new lines must be defined using double slashes: \\\\n.

EXAMPLE INPUT STARTS BELOW:

22: struct TranslatableWidget
23: {{
24:   QObject* widget;
25:   const char* text;
26: }};
27:
28: void addClass(QWidget* object, const QString& className);
29: QPointF snapToGrid(const QPointF& point, const int gridSize);
EXAMPLE INPUT ENDS ABOVE.

EXAMPLE OUTPUT STARTS BELOW:
[
  {{
    "line": 22,
    "comment": "/**\\n * @brief Stores a widget with translatable text.\\n */"
  }},
  {{
    "line": 24,
    "comment": "/// Widget associated with the translatable text."
  }},
  {{
    "line": 28,
    "comment": "/**\\n * @brief Adds a class name to a QWidget.\\n * @param object The widget to which the class name will be added.\\n * @param className The class name to add.\\n */"
  }},
  {{
    "line": 29,
    "comment": "/**\\n * @brief Snaps a point to the nearest grid position.\\n * @param point The input point.\\n * @param gridSize The grid size.\\n * @return The snapped point.\\n */"
  }}
]
EXAMPLE OUTPUT ENDS ABOVE.

HEADER FILE WITH LINE NUMBERS STARTS BELOW:
{numbered_header}

HEADER FILE WITH LINE NUMBERS ENDS ABOVE.

SOURCE FILE STARTS BELOW:
{source_code}

SOURCE FILE ENDS ABOVE.
"""


def add_line_numbers(code: str) -> str:
    return '\n'.join(f'{i + 1}: {line}' for i, line in enumerate(code.splitlines()))


def parse_json_response(response: str) -> list[dict]:
    text = response.strip()

    # Remove accidental markdown fences.
    if text.startswith('```'):
        text = re.sub(r'^```(?:json)?', '', text).strip()
        text = re.sub(r'```$', '', text).strip()

    # Best-effort extraction if the model added prose anyway.
    if not text.startswith('['):
        start = text.find('[')
        end = text.rfind(']')
        if start != -1 and end != -1 and end > start:
            text = text[start: end + 1]

    data = json.loads(text)

    if not isinstance(data, list):
        raise ValueError('Patch response must be a JSON array.')

    for item in data:
        if not isinstance(item, dict):
            raise ValueError('Each patch item must be an object.')

        if 'line' not in item or 'comment' not in item:
            raise ValueError('Each patch item must contain "liner" and "comment".')

    return data


def apply_insertions(header_code: str, insertions: list[dict]) -> str:
    lines = header_code.splitlines()

    cleaned: list[dict] = []

    for item in insertions:
        line = int(item['line'])
        comment = str(item['comment']).rstrip()

        if not comment:
            continue

        line = max(1, min(line, len(lines) + 1))
        cleaned.append({'line': line, 'comment': comment})

    # Apply bottom-to-top so original line numbers remain valid.
    for item in sorted(cleaned, key=lambda x: x['line'], reverse=True):
        index = item['line'] - 1
        comment_lines = item['comment'].splitlines()
        lines[index:index] = comment_lines

    return '\n'.join(lines) + '\n'


def find_header_source_pairs(root: Path) -> list[tuple[Path, Path]]:
    headers = [p for p in root.rglob('*') if p.is_file() and p.suffix.lower() in HEADER_EXTENSIONS]
    sources = {
        p.with_suffix('').name: p
        for p in root.rglob('*')
        if p.is_file() and p.suffix.lower() in SOURCE_EXTENSIONS
    }

    pairs: list[tuple[Path, Path]] = []

    for header in headers:
        stem = header.with_suffix('').name
        source = sources.get(stem)

        if source is not None:
            pairs.append((header, source))

    return pairs


def call_ollama(prompt: str, model: str, host: str, config: Config) -> str:
    from ollama import Client

    timeout = 600
    client = Client(host=host, timeout=timeout)
    chunks: list[str] = []
    print('\n--- LLM response start ---\n')

    try:
        stream = client.generate(
            model=model,
            prompt=prompt,
            stream=True,
            options={
                'temperature': 0.0,
                'top_p': 0.2,
                'num_ctx': 16384,
                'num_predict': 16384,
            },
        )

        for chunk in stream:
            text = chunk.get('response', '')
            if config.print_stream:
                print(text, end='', flush=True)
            chunks.append(text)

        print('\n\n--- LLM response end ---\n')

    except Exception as exc:
        print('\n\n--- LLM response interrupted ---\n')
        raise RuntimeError(f'Ollama call failed or timed out after {timeout}s: {exc}')

    return ''.join(chunks).strip()


def extract_cpp_from_response(response: str) -> str:
    """
    Extract a C++ header from the LLM response.

    Handles:
    - raw output (expected)
    - fenced code blocks (fallback)
    - noisy responses (best-effort)
    """
    text = response.strip()

    # Case 1: No code fences, assume raw header (expected case)
    if '```' not in text:
        return text + '\n'

    # Case 2: Try to extract from fenced blocks
    parts = text.split('```')
    for i in range(1, len(parts), 2):
        block = parts[i].strip()

        if block.startswith('cpp'):
            return block[len('cpp'):].strip() + '\n'

        if block.startswith('c++'):
            return block[len('c++'):].strip() + '\n'

        # fallback: looks like C++ code
        if any(k in block for k in ['#include', '#pragma once', 'class ', 'namespace ']):
            return block.strip() + '\n'

    # Case 3: fallback → return entire response
    return text + '\n'


def backup_file(path: Path) -> Path:
    backup = path.with_suffix(path.suffix + '.bak')
    shutil.copy2(path, backup)
    return backup


# TODO: This is not working properly yet
def process_pair_patch_mode(header: Path, source: Path, model: str, host: str, config: Config) -> None:
    header_code = header.read_text(encoding='utf-8')
    # source_code = source.read_text(encoding='utf-8')

    header_with_lines = add_line_numbers(header_code)
    prompt = PATCH_PROMPT_TEMPLATE.format(
        numbered_header=header_with_lines,
        # source_code=source_code,
    )

    print(f'Processing in patch mode: {header}  +  {source}')
    if config.debug:
        print(f'Sending prompt:\n{prompt}')

    response = call_ollama(prompt, model=model, host=host, config=config)
    # response = RESPONSE

    if config.debug:
        print(f'Response:\n{response}')

    insertions = parse_json_response(response)

    if not insertions:
        print('  No insertions suggested.')
        return

    updated_header = apply_insertions(header_code, insertions)

    if updated_header == header_code:
        print('  No changes.')
        return

    if config.dry_run:
        out = header.with_suffix(header.suffix + '.llm')
        out.write_text(updated_header, encoding='utf-8')
        print(f'  Dry run output written to: {out}')
        return

    if config.backup:
        backup_path = backup_file(header)
        print(f'  Backup written to: {backup_path}')

    header.write_text(updated_header, encoding='utf-8')
    print(f'  Header updated with {len(insertions)} insertion(s).')


def get_source_code(source: Path, use_path: bool) -> str:
    if use_path:
        if source is None:
            raise Exception('Source requested but no source provided')

        source_code = source.read_text(encoding='utf-8')
        return SOURCE_PROMPT.format(
            source_code=source_code
        )

    return str()


def process_pair(header: Path, source: Path, model: str, host: str, config: Config) -> None:
    header_code = header.read_text(encoding='utf-8')

    prompt = PROMPT_TEMPLATE.format(
        header_code=header_code,
        source_code=get_source_code(source, config.source),
    )

    print(f'Processing: {header}  +  {source}')
    if config.debug:
        print(f'Sending prompt: {prompt}')

    response = call_ollama(prompt, model=model, host=host, config=config)
    if config.debug:
        print(f'Response:\n{response}')

    updated_header = extract_cpp_from_response(response)

    if updated_header.strip() == header_code.strip():
        print('  No changes.')
        return

    if config.dry_run:
        out = header.with_suffix(header.suffix + '.llm')
        out.write_text(updated_header, encoding='utf-8')
        print(f'  Dry run output written to: {out}')
        return

    if config.backup:
        backup_path = backup_file(header)
        print(f'  Backup written to: {backup_path}')

    header.write_text(updated_header, encoding='utf-8')
    print('  Header updated.')


def setup_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description='Add Doxygen comments to C++ headers using a local LLM.')
    parser.add_argument(
        '--folder',
        type=Path,
        help='Root folder to scan recursively.',
    )
    parser.add_argument(
        '--model',
        default='mistral',
        help='Local model name. Default: mistral',
    )
    parser.add_argument(
        '--host',
        default='http://localhost:11434',
        help='Ollama host. Default: http://localhost:11434',
    )
    parser.add_argument(
        '--dry-run',
        action='store_true',
        help='Write .llm files instead of modifying headers.',
    )
    parser.add_argument(
        '--no-backup',
        action='store_true',
        help='Do not create .bak files before modifying headers.',
    )
    parser.add_argument(
        '--debug',
        action='store_true',
        help='Enable debug prints',
    )
    parser.add_argument(
        '--no-stream',
        action='store_true',
        help='Do not stream model response',
    )
    parser.add_argument(
        '--sleep',
        type=float,
        default=0.0,
        help='Optional delay between requests.',
    )
    parser.add_argument(
        '--header',
        type=Path,
        help='Path to a specific header file.',
    )
    parser.add_argument(
        '--source',
        type=Path,
        help='Path to a specific source file.',
    )
    parser.add_argument(
        '--patch-mode',
        action='store_true',
        help='Ask the model for JSON comment insertions instead of rewriting the full header.',
    )
    parser.add_argument(
        '--exclude',
        action='append',
        default=[],
        help='Glob pattern to exclude. E.g. --exclude "*/build/*" --exclude "*_test.*"',
    )
    parser.add_argument(
        '--use_source',
        action='store_true',
        help='Use source files for context',
    )

    return parser.parse_args()


def is_excluded(path: Path, patterns: list[str]) -> bool:
    name = path.name
    return any(fnmatch(name, pattern) for pattern in patterns)


def get_files(args: argparse.Namespace) -> list[tuple[Path, Path]]:
    pairs: list[tuple[Path, Path]] = []

    # If user provided a specific pair, use only that
    if args.header:
        header = args.header.resolve()
        if not header.exists():
            print(f'Header not found: {header}', file=sys.stderr)
            return []

        if header.suffix.lower() not in HEADER_EXTENSIONS:
            print(f'Not a header file: {header}', file=sys.stderr)
            return []

        source = None
        if args.source:
            source = args.source.resolve()
            if not source.exists():
                print(f'Source not found: {source}', file=sys.stderr)
                return []

            if source.suffix.lower() not in SOURCE_EXTENSIONS:
                print(f'Not a source file: {source}', file=sys.stderr)
                return []

        pairs = [(header, source)]

    elif args.source:
        print('You must provide both --header and --source together.', file=sys.stderr)
        return []

    elif args.folder:
        root = args.folder.resolve()
        if not root.exists() or not root.is_dir():
            print(f'Not a valid folder: {root}', file=sys.stderr)
            return []

        else:
            pairs = find_header_source_pairs(root)
            print(f'Found {len(pairs)} header/source pair(s).')

    return pairs


def main() -> int:
    # Get all the arguments
    args = setup_args()

    # Setup the script configuration
    config = Config(
        debug=args.debug,
        backup=not args.no_backup,
        dry_run=args.dry_run,
        print_stream=not args.no_stream,
        source=(args.source or args.use_source)
    )

    pairs = get_files(args)
    if len(pairs) == 0:
        return 1

    print(f'Running in {"patch" if args.patch_mode else "full header"} mode')
    print(f'Running with {"" if config.backup else "no "}backup')
    if config.debug:
        print('Running in debug mode')
    if config.dry_run:
        print('Dry run')

    errors = 0
    processed = 0
    process_call = process_pair_patch_mode if args.patch_mode else process_pair

    # Generate the comments for all the found pairs
    for header, source in pairs:
        cleaned_name = f'../{header.parent.name}/{header.name}'
        if is_excluded(header, args.exclude):
            print(f'Filtering out {cleaned_name}')
            continue

        try:
            process_call(
                header=header,
                source=source,
                model=args.model,
                host=args.host,
                config=config,
            )
        except Exception as exc:
            print(f'Error while processing {cleaned_name}: {exc}', file=sys.stderr)
            errors += 1
            pass

        processed += 1
        print(f'{datetime.now().strftime("%H:%M:%S")} Done with {cleaned_name} '
              f'(Success: {processed} - Errors: {errors} - Total: {len(pairs)})')
        if args.sleep > 0:
            time.sleep(args.sleep)

    return 0


if __name__ == '__main__':
    raise SystemExit(main())
