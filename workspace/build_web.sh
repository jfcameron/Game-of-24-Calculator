#!/usr/bin/env bash

WEBSRC_ROOT_DIR="../websrc"

SOURCE_ROOT_DIR="../src"
PUBLIC_HEADER_DIR="../include"
PRIVATE_HEADER_DIR="${SOURCE_ROOT_DIR}/include"

#NOTE: c++1z not 17, https://github.com/emscripten-core/emscripten/issues/5513
emcc \
    -O3 \
    "${SOURCE_ROOT_DIR}"/*.cpp \
    -I"${PUBLIC_HEADER_DIR}" \
    -I"${PRIVATE_HEADER_DIR}" \
    -std=c++1z \
    -o output.html \
    -s DISABLE_EXCEPTION_CATCHING=0 \
    --pre-js "${WEBSRC_ROOT_DIR}/pre-js.js" \
    --post-js "${WEBSRC_ROOT_DIR}/post-js.js" \
    -DBUILD_WEB

cp ../websrc/index.html .
