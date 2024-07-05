
.PHONY: build configure test clang-tidy

build:
	cmake --build --preset "debug"

configure:
	cmake --preset "debug" $(if $(SANITIZER), -DCMAKE_CXX_FLAGS="-fsanitize=$(SANITIZER)")

test: build
	ctest --preset "debug" --output-on-failure

clang-tidy:
	find include/ \( -iname "*.h" -or -iname "*.cpp" \) -print0 | parallel -0 clang-tidy -p build {}
