CXX=g++-8
RM=rm -rf
BUILDDIR=../build
SRCLIBDIR=../src
GOOGLE=../google
SRCLIB=$(BUILDDIR)/liboak.a
INCLUDE=-I$(SRCLIBDIR) -I$(GOOGLE)/include
LDPATH=-L$(GOOGLE)/lib -L$(BUILDDIR)
LIBS=-loak -lgtest -lpthread
CPPFLAGS=--std=c++2a -Wall -Werror $(INCLUDE) $(LDPATH) $(LIBS) -O3

SRC=$(wildcard *.cpp)
OBJ=$(SRC:%.cpp=%.o)
BIN=$(BUILDDIR)/test_oak

all: $(BIN)
	@echo ALL DONE!!!

$(BIN): $(SRCLIB) $(OBJ)
	$(CXX) $(OBJ) $(CPPFLAGS) -o $(BIN)

$(SRCLIB):
	$(MAKE) -C $(SRCLIBDIR)

clean:
	$(RM) $(BIN) $(OBJ)
