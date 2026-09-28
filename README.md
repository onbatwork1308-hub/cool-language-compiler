# COOL Compiler

A from-scratch compiler for **COOL** (Classroom Object-Oriented Language), written in C.

Pipeline: `source (.cl) → Lexer → Parser → Semantic Analysis → IR → Optimizer → Codegen → assembly`

## Status

- [x] Lexer
- [ ] Parser
- [ ] Semantic analysis
- [ ] IR
- [ ] Optimizer
- [ ] Code generation
- [ ] Automated test harness

## Features

- **Lexer** — full COOL token set, case-insensitive keywords, maximal-munch identifiers,
  nested block comments, string escapes, and non-fatal categorized error reporting.
  See `docs/tables.pdf`, `docs/internTables.pdf`, `docs/lexer_token_keyword.pdf` for design details.

## Requirements

- C11 compiler (`gcc`)
- A **POSIX** environment — `strncasecmp()` (`<strings.h>`) is used for keyword matching and is
  not available on plain Windows/MSVC without a POSIX layer (WSL, MinGW, Cygwin).
- `glibc-static` (or equivalent) for the default static build.

## Build

```bash
chmod +x compile.sh
./compile.sh
```

## Usage

```bash
./build/lexer <path-to-source.cl>
```

Example:

```bash
./build/lexer tests/frontend/lexer/inputs/test_lexer.cl
```

Prints any lexical errors, then every recognized token as `#<line>  <TYPE>  <text>`. Exits `0` if
no lexical errors were found, `1` otherwise.

## 📁 Repository Structure

```text
cool-language-compiler/
│
├── build
│   └── lexer
├── compile.sh
├── docs
│   ├── interntables.pdf
│   ├── lexer_token_keyword.pdf
│   └── tables.pdf
├── include
│   ├── backend
│   ├── common
│   │   ├── internTables.h
│   │   ├── table.h
│   │   └── utils.h
│   ├── frontend
│   │   ├── lexer
│   │   │   ├── keyword.h
│   │   │   ├── lexer.h
│   │   │   ├── token.h
│   │   │   └── tokenlist.h
│   │   ├── parser
│   │   └── semant
│   └── ir
├── MAKEFILE
├── output
├── README.md
├── src
│   ├── backend
│   │   ├── codegen
│   │   └── optimizer
│   ├── common
│   │   ├── internTables.c
│   │   ├── table.c
│   │   └── utils.c
│   ├── frontend
│   │   ├── lexer
│   │   │   ├── keyword.c
│   │   │   ├── lexer.c
│   │   │   ├── token.c
│   │   │   └── tokenlist.c
│   │   ├── parser
│   │   └── semant
│   ├── ir
│   └── main.c
└── tests
    ├── backend
    ├── common
    └── frontend
        ├── lexer
        │   ├── expected
        │   └── inputs
        │       └── test_lexer.cl
        ├── parser
        └── semant

```

## Documentation

Design rationale for each phase lives in `docs/`, one document (or set) per phase, added as that
phase is built. This README stays a short index — build/usage instructions and a feature/status
summary, see `docs/` for depth.

## Author
onbatwork1308-hub

From-scratch compiler project, built one phase at a time.
