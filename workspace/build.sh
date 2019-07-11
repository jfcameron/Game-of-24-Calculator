#!/usr/bin/env bash

SOURCE_ROOT_DIR="../src"
PUBLIC_HEADER_DIR="../include"
PRIVATE_HEADER_DIR="${SOURCE_ROOT_DIR}/include"

g++ \
    "${SOURCE_ROOT_DIR}"/*.cpp \
    -I"${PUBLIC_HEADER_DIR}" \
    -I"${PRIVATE_HEADER_DIR}" \
    -std=c++17

