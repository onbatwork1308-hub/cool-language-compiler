#!/usr/bin/env bash
# compile.sh — static build of the COOL lexer front-end
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$ROOT/build"
BIN="$BUILD_DIR/lexer"

mkdir -p "$BUILD_DIR"

CC=gcc
# -D_POSIX_C_SOURCE enables strdup/strncasecmp, which aren't part of plain C11
CFLAGS="-std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -g -static"

INCLUDES=(
    -I"$ROOT/include/common"
    -I"$ROOT/include/frontend/lexer"
)

SOURCES=(
    "$ROOT/src/common/table.c"
    "$ROOT/src/common/internTables.c"    # capital T — matches your actual filename
    "$ROOT/src/common/utils.c"
    "$ROOT/src/frontend/lexer/token.c"
    "$ROOT/src/frontend/lexer/tokenlist.c"
    "$ROOT/src/frontend/lexer/keyword.c"
    "$ROOT/src/frontend/lexer/lexer.c"

    "$ROOT/src/main.c"
)

echo "Compiling COOL lexer (static build)..."
$CC $CFLAGS "${INCLUDES[@]}" "${SOURCES[@]}" -o "$BIN"

echo "Build succeeded: $BIN"
