#!/usr/bin/env bash
# compile.sh — static build of the COOL lexer front-end
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$ROOT/build"
BIN="$BUILD_DIR/coolc_debug"

mkdir -p "$BUILD_DIR"

CC=gcc
# -D_POSIX_C_SOURCE enables strdup/strncasecmp, which aren't part of plain C11
CFLAGS="-std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -g -O0"

INCLUDES=(
    -I"$ROOT/include/common"
    -I"$ROOT/include/frontend/lexer"
    -I"$ROOT/include/frontend/parser"
)

SOURCES=(
    "$ROOT/src/common/table.c"
    "$ROOT/src/common/internTables.c"    # capital T — matches your actual filename
    "$ROOT/src/common/utils.c"
    "$ROOT/src/common/dynarray.c"
    "$ROOT/src/frontend/lexer/token.c"
    "$ROOT/src/frontend/lexer/tokenlist.c"
    "$ROOT/src/frontend/lexer/keyword.c"
    "$ROOT/src/frontend/lexer/lexer.c"
    "$ROOT/src/frontend/parser/ast.c"
    "$ROOT/src/frontend/parser/syntax_error.c"
    "$ROOT/src/frontend/parser/parser.c"
    "$ROOT/src/main.c"
)

echo "Compiling COOL lexer (static build)..."
$CC $CFLAGS "${INCLUDES[@]}" "${SOURCES[@]}" -o "$BIN"

echo "Build succeeded: $BIN"

