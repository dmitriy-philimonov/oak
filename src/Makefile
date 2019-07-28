CXX=g++
AR=ar
CPPFLAGS=--std=c++2a -Wall -Werror -DPRINT_DEBUG -O3
RM=rm -rf
BUILDDIR=../build
LIB=$(BUILDDIR)/liboak.a

SRC=$(wildcard *.cpp)
OBJ=$(SRC:%.cpp=$(BUILDDIR)/%.o)
DEP=$(OBJ:%.o=%.d)

all: $(BUILDDIR) $(LIB)
	@echo "ALL DONE!!!"

$(LIB): $(OBJ)
	$(AR) rcs $@ $^

$(BUILDDIR)/%.o: %.cpp
	$(CXX) $(CPPFLAGS) -MMD -c $< -o $@

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

-include $(DEP)

.PHONY: clean
clean:
	$(RM) $(LIB) $(OBJ) $(DEP)

