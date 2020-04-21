CC=$(CROSS)gcc
CXX=$(CROSS)g++
LD=$(CROSS)ld
AR=$(CROSS)ar
PKG_CONFIG=$(CROSS)pkg-config

RESDIR = ./res
DATADIR = ./shaders
BINDIR = ./bin
OBJDIR = ./bin

CXXSRC = $(wildcard src/*.cpp)\
         $(wildcard src/opengl/*.cpp)\
         $(wildcard src/imgui/*.cpp)

CSRC = $(wildcard src/*.c)

CXXOBJ = $(CXXSRC:%.cpp=$(OBJDIR)/%.o)

COBJ = $(CSRC:.cpp=.o)

DEPS = $(CXXOBJ:.o=.d)

BIN = application

BINDEPS = sdl2 epoxy

BUILDFLAGS =  -pthread -O3 -flto -fopenmp -Wall -std=c++17
LDFLAGS = $(shell $(PKG_CONFIG) --libs $(BINDEPS))
CFLAGS = $(shell $(PKG_CONFIG) --cflags $(BINDEPS))
CXXFLAGS = $(BUILDFLAGS) $(CFLAGS)

$(BINDIR)/$(BIN): $(CXXOBJ)
	mkdir -p $(@D)
	$(CXX) $(BUILDFLAGS) $(CFLAGS) -o $@ $^ $(LDFLAGS)

#-include $(DEPS) # include all dep files in the makefile

$(CXXOBJ): $(OBJDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(BUILDFLAGS) $(CFLAGS) -c -o $@ $<

# rule to generate a dep file by using the C preprocessor
# (see man cpp for details on the -MM and -MT options)
%.d: %.c
	mkdir -p $(@D)
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

%.d: %.cpp
	mkdir -p $(@D)
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

%.d: %.cpp
	mkdir -p $(@D)
	@$(CPP) $(CFLAGS) $(CXXFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: all build release debug clean

#all: cleandeps
all: build $(BINDIR)/$(BIN)

debug: BUILDFLAGS = -ggdb -O0 -Wall -pthread -fopenmp
debug: all

clean:
	rm -rf $(BINDIR) $(COBJ) $(CXXOBJ) $(DEPS)

cleandeps:
	rm -f $(DEPS)
