#
# Makefile for building unit tests with Google Test.
#

CXX = g++

SRC = ../../src
TESTS = ../../tests

GUNIT_BASE += ../../gtest-1.7.0
GUNIT_PATH += $(GUNIT_BASE)/include

COMPILER_FLAGS += \
    -I$(GUNIT_PATH) \
    -I$(SRC) -I$(SRC)/travelsim \
    -g -std=c++11 \
    -Wall \
    -Wno-unused-function

LIBS = $(GUNIT_BASE)/make/gtest_main.a -lpthread

# Tests
FILES += $(TESTS)/travelsim/TravelTests.cxx

main: $(FILES)
	$(CXX) $(COMPILER_FLAGS) $(FILES) $(LIBS) -o unittests


all: main

clean:
	rm -rf unittests
