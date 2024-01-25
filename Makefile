UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	OMPFLAGS += -fopenmp
else ifeq ($(UNAME_S),Darwin)
	OMPFLAGS += -Xpreprocessor -fopenmp -lomp
endif

processor := $(shell uname -m)
ifeq ($(processor),$(filter $(processor),aarch64 arm64))
    ARCHFLAGS+=-march=armv8-a+fp+simd+crc
else ifeq ($(processor),$(filter $(processor),i386 x86_64))
    ARCHFLAGS+=-march=native
endif

CXX=g++

CXXFLAGS=-std=c++17 -O3 -Wall -Wextra -I include/

CXXFLAGS+=#pkg-config flags

LDFLAGS=#dynamically linked libraries

# DEBUGOPTIONS=-fsanitize=address -g -fno-omit-frame-pointer

all: src/abs src/min-max

src/% : src/%.cpp dir
	$(CXX) -o build/$@ $< $(CXXFLAGS) $(LDFLAGS) $(DEBUGOPTIONS) $(OMPFLAGS) $(ARCHFLAGS) $(EXTRA)

.PHONY: dir clean

dir:
	mkdir -p build/src

clean:
	rm -r build/*
