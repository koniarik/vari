
.PHONY: build configure test clang-tidy

PRESET ?= debug

build:
	cmake --build --preset "$(PRESET)"

configure:
	cmake --preset "$(PRESET)"

test: build
	ctest --preset "$(PRESET)" --output-on-failure --verbose

clang-tidy:
	find include/ \( -iname "*.h" -or -iname "*.cpp" \) -print0 | parallel -0 clang-tidy -p _build/$(PRESET) {}
