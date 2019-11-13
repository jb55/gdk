#!/usr/bin/env bash
set -e

clang-format -i src/*.{c,h}pp include/*.h

if [ $(command -v cargo) ]; then
    pushd subprojects/gdk_rpc
	  cargo fmt --all
    popd
fi
