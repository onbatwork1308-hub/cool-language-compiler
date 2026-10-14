# COOL Compiler

A from-scratch compiler for **COOL** (Classroom Object-Oriented Language), written in C.

Pipeline: `source (.cl) → Lexer → Parser → Semantic Analysis → IR → Optimizer → Codegen → assembly`

## Status

- [x] Lexer
- [x] Parser
- [ ] Semantic analysis
- [ ] IR
- [ ] Optimizer
- [ ] Code generation

## Features

- **Lexer** — full COOL token set, case-insensitive keywords, maximal-munch identifiers,
  nested block comments, string escapes, and non-fatal categorized error reporting.
  See `docs/tables.pdf`, `docs/internTables.pdf`, `docs/lexer_token_keyword.pdf` for design details.

- **Parser** — hand-written LL(1) recursive descent parser producing an AST, with precedence and
  associativity encoded in the call structure, and panic-mode error recovery so one run reports
  every syntax error. See `docs/parser/` for the grammar and the design tradeoffs.

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
./build/coolc [--tokens] <path-to-source.cl>
```

- Lexical errors are printed first. If there are any, parsing is skipped.
- Otherwise the source is parsed, and any syntax errors are reported.
- If the program is valid, the AST is printed to stdout.
- `--tokens` additionally dumps the token stream as `#<line>  <TYPE>  <text>`.
- Exit status: `0` if the program is valid, `1` on any lexical or syntax error.
  Errors go to stderr and the tree goes to stdout, so `./build/coolc f.cl > tree.txt` captures only the tree.

Example input:

```cool
class Main inherits IO {
    x : Int <- 5;
    main() : Object { out_int(x) };
};
```

Output:

```text
PROGRAM #1
  CLASS Main inherits IO #1
    ATTR x : Int #2
      INT_CONST 5 #2
    METHOD main : Object #3
      DISPATCH out_int (self) #3
        OBJECT_ID x #3
```

## Testing

Test inputs live in `tests/frontend/<phase>/inputs/`, with expected output in `expected/`.
Run the parser tests with:

```bash
./build/coolc tests/frontend/parser/*.cl 
```

Debugging: build with `-g -O0` (without `-static`) and use `gdb` or `valgrind`.
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

- Lexer: `docs/lexer/tables.pdf`, `docs/lexer/interntables.pdf`, `docs/lexer/lexer_token_keyword.pdf`
- Parser: `docs/parser/grammar_rules.pdf`, `docs/parser/parser_implementation_tradeoffs_and_decisions.pdf`

## Known limitations

- No cap on expression nesting depth: extremely deeply nested input (hundreds of thousands of
  levels) overflows the stack. A nesting limit with a proper error message is planned.
- The parser has not yet been checked against COOL's reference compiler on every edge case.

## Author
onbatwork1308-hub

From-scratch compiler project, built one phase at a time.
