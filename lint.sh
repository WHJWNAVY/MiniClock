#!/bin/sh

SRC_DIR=./src

find ${SRC_DIR} -iname "*.[ch]" | xargs clang-format-6.0 -i -style=file
