
test-build: fake
	ninja -C build-clang test_rpc && ./build-clang/test_rpc

android-build: fake
	ninja -C build-clang-android-x86_64 all

TAGS: fake
	find src -type f -name '*.hpp' -o -name '*.cpp' | xargs etags

format: fake
	./tools/format.sh

lint: fake
	./tools/lint.sh

.PHONY: fake
