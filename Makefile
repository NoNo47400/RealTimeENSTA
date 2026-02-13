##############################################################################
# Makefile for RealTimeENSTA
#
# - Compiles library sources in src/ into object files stored in build/
# - Builds one executable per testbench/*.cpp into bin/<name>
# - Uses include/ for headers
# - Compiler flags: -O3 -std=c++11 -Wall -Wextra
# - Links with -lrt and -pthread
##############################################################################

CXX := g++
CXXFLAGS := -O3 -std=c++11 -Wall -Wextra -Iinclude
LDFLAGS :=
LDLIBS := -lrt -pthread

.PHONY: all clean dirs list

# Library sources (in src/) and corresponding object files in build/
LIB_SRCS := $(wildcard src/*.cpp)
LIB_OBJS := $(patsubst src/%.cpp,build/%.o,$(LIB_SRCS))

# Testbench sources (each contains a main) and targets
TB_SRCS := $(wildcard testbench/*.cpp)
TB_OBJS := $(patsubst testbench/%.cpp,build/%_tb.o,$(TB_SRCS))
TB_EXES := $(patsubst testbench/%.cpp,bin/%,$(TB_SRCS))

all: dirs $(TB_EXES)
	@echo "Build complete. Removing intermediate object files in build/ to keep only bin/"
	@rm -rf build

# Link each testbench executable: bin/name depends on its tb object and all lib objs
bin/%: build/%_tb.o $(LIB_OBJS) | dirs
	@echo "Linking $@"
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# Compile testbench sources to build/<name>_tb.o
build/%_tb.o: testbench/%.cpp | dirs
	@echo "Compiling testbench $< -> $@"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile library sources to build/*.o
build/%.o: src/%.cpp | dirs
	@echo "Compiling lib $< -> $@"
	$(CXX) $(CXXFLAGS) -c $< -o $@

dirs:
	mkdir -p build
	mkdir -p bin

clean:
	rm -rf build bin testbench/logs

# Useful phony targets
list:
	@echo "Library sources:"
	@printf "  %s\n" $(LIB_SRCS)
	@echo "\nTestbench sources:"
	@printf "  %s\n" $(TB_SRCS)
	@echo "\nExecutables to be built:"
	@printf "  %s\n" $(TB_EXES)