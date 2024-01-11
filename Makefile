processor := $(shell uname -m)
ifeq ($(processor),$(filter $(processor),aarch64 arm64))
    ARCH_C_FLAGS+=-march=armv8-a+fp+simd+crc
else ifeq ($(processor),$(filter $(processor),i386 x86_64))
    ARCH_C_FLAGS+=-march=native
endif

CXX=g++ -std=c++14 -O3 -I include/

all: abs min-max-bench

src/% : src/%.cpp dir
	$(CXX) -o build/$@ $< $(CXXFLAGS) $(OMPFLAGS) $(ARCH_C_FLAGS) $(OPT)

clean:
	rm -r build/*

.PHONY: dir

dir:
	mkdir -p build/src


