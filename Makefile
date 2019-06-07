CXX=g++-8
CPPFLAGS=--std=c++2a -Wall -Werror -DPRINT_DEBUG -O3
RM=rm -rf
BUILDDIR=./build
TESTS=$(BUILDDIR)/test_oak
BENCHMARK=$(BUILDDIR)/benchmark_oak

all: 
	$(MAKE) -C tests
	$(MAKE) -C benchmark

test:
	$(TESTS)

benchmark:
	$(BENCHMARK)

	
.PHONY: clean test benchmark
clean:
	$(MAKE) clean -C benchmark
	$(MAKE) clean -C tests
	$(MAKE) clean -C src

