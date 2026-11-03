#!/usr/bin/env bash
# Builds the unmodified TobiOS C++ sources to WebAssembly in the emscripten/emsdk image,
# then assembles the static site in demo/dist. Run from anywhere; needs Docker and npm.
set -euo pipefail
DEMO="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$DEMO/.." && pwd)"
DIST="$DEMO/dist"
EMSDK_IMAGE="emscripten/emsdk:3.1.74"

rm -rf "$DIST"
mkdir -p "$DIST/vendor"

MOUNT="$ROOT"
command -v cygpath >/dev/null && MOUNT="$(cygpath -w "$ROOT")"

MSYS_NO_PATHCONV=1 docker run --rm -v "$MOUNT:/src" -w /src "$EMSDK_IMAGE" em++ \
  -std=c++20 -O2 -fexceptions -I include \
  main.cpp src/kernel/kernel.cpp src/kernel/logger.cpp src/memory/memory_manager.cpp \
  src/process/process_manager.cpp src/filesystem/filesystem.cpp src/shell/shell.cpp \
  --js-library demo/tty-lib.js \
  -sASYNCIFY -sASYNCIFY_STACK_SIZE=65536 \
  -sEXPORTED_FUNCTIONS=_main -sEXPORTED_RUNTIME_METHODS=ccall \
  -sMODULARIZE=1 -sEXPORT_ES6=1 -sEXPORT_NAME=createTobiOS \
  -sENVIRONMENT=web,node -sALLOW_MEMORY_GROWTH=1 -sINITIAL_MEMORY=16MB \
  -sINCOMING_MODULE_JS_API=noInitialRun,print,printErr,ttyWrite,ttyRead,ttyUnread,locateFile \
  -o demo/dist/tobios.mjs

cp "$DEMO"/web/* "$DIST"/
cd "$DEMO"
npm ci --no-audit --no-fund --silent
cp node_modules/@xterm/xterm/lib/xterm.js node_modules/@xterm/xterm/css/xterm.css \
   node_modules/@xterm/addon-fit/lib/addon-fit.js "$DIST/vendor/"
cp node_modules/@xterm/xterm/LICENSE "$DIST/vendor/xterm.LICENSE.txt"
ls -la "$DIST"
