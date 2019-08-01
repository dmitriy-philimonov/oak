CXX=g++
RM=rm -rf
BUILDDIR=../build
SRCLIBDIR=../src
GOOGLE=../google
SRCLIB=$(BUILDDIR)/liboak.a
INCLUDE=-I$(SRCLIBDIR) -I$(GOOGLE)/include
LDPATH=-L$(GOOGLE)/lib -L$(BUILDDIR)
LIBS=-loak -lbenchmark -lpthread
CPPFLAGS=--std=c++2a -Wall -Werror -O3 -march=native $(INCLUDE) $(LDPATH) $(LIBS)

SRC=$(wildcard *.cpp)
OBJ=$(SRC:%.cpp=%.o)
BIN=$(BUILDDIR)/benchmark_oak

all: $(BIN)
	@echo ALL DONE!!!

$(SRCLIB):
	$(MAKE) -C $(SRCLIBDIR)

$(BIN): $(OBJ) $(SRCLIB)
	$(CXX) $(OBJ) $(CPPFLAGS) -o $(BIN)

clean:
	$(RM) $(BIN) $(OBJ)
