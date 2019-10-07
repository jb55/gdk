#! /usr/bin/env bash
set -e

BUILDTYPE="$1"
shift

OUTPUT="$1"
shift

SOURCE_ROOT="$1"
shift

BUILD_ROOT="$1"
shift

cp -r "$SOURCE_ROOT/subprojects/gdk_rpc" "$BUILD_ROOT/subprojects"

export CC_i686_linux_android=i686-linux-android19-clang
export CC_x86_64_linux_android=x86_64-linux-android21-clang
export CC_armv7_linux_androideabi=armv7a-linux-androideabi19-clang
export CC_aarch64_linux_android=aarch64-linux-android21-clang

cd "$BUILD_ROOT/subprojects/gdk_rpc"

if [ \( "$1" = "--ndk" \) ]; then
    if [ "$(uname)" = "Darwin" ]; then
        export PATH=${PATH}:${ANDROID_NDK}/toolchains/llvm/prebuilt/darwin-x86_64/bin
    else
        export PATH=${PATH}:${ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/bin
    fi
    if [ $HOST_ARCH = "armeabi-v7a" ]; then
        RUSTTARGET=armv7-linux-androideabi
    elif [ $HOST_ARCH = "arm64-v8a" ]; then
        RUSTTARGET=aarch64-linux-android
    elif [ $HOST_ARCH = "x86" ]; then
        RUSTTARGET=i686-linux-android
    elif [ $HOST_ARCH = "x86_64" ]; then
        RUSTTARGET=x86_64-linux-android
    else
        echo "Unkown android platform"
        exit -1
    fi
elif [ \( "$1" = "--windows" \) ]; then
    RUSTTARGET=x86_64-pc-windows-gnu
elif [ \( "$1" = "--iphone" \) ]; then
    RUSTTARGET=aarch64-apple-ios
elif [ \( "$1" = "--iphonesim" \) ]; then
    RUSTTARGET=x86_64-apple-ios
fi

CARGO_ARGS=()
if [ "$BUILDTYPE" == "release" ]; then
    CARGO_ARGS+=("--release")
fi

if [ -n "$RUSTTARGET" ]; then
    CARGO_ARGS+=("--target=$RUSTTARGET")
fi

printf "cargo args: ${CARGO_ARGS[*]}\n"
cargo build "${CARGO_ARGS[@]}"

if [ -z "$RUSTTARGET" ]; then
    cp "target/${BUILDTYPE}/libgdk_rpc.a" "${BUILD_ROOT}/$OUTPUT"
else
    mkdir -p "target/${BUILDTYPE}"
    cp "target/${RUSTTARGET}/$BUILDTYPE/libgdk_rpc.a" "${BUILD_ROOT}/$OUTPUT"
fi

KEEP="${SOURCE_ROOT}/subprojects/gdk_rpc/exported-symbols"
WEAKEN="${SOURCE_ROOT}/subprojects/gdk_rpc/weaken-symbols"

if [ $(command -v objcopy) ]; then
    objcopy --strip-unneeded --keep-symbols="$KEEP" --weaken-symbols="$WEAKEN" "${BUILD_ROOT}/$OUTPUT"
fi
