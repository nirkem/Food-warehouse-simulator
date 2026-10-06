CXX      := g++
CXXFLAGS := -g -Wall -Weffc++ -std=c++11 -Iinclude
SOURCES  := $(wildcard src/*.cpp)
OBJECTS  := $(patsubst src/%.cpp,bin/%.o,$(SOURCES))

all: bin/warehouse

bin/warehouse: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

bin/%.o: src/%.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

test: bin/warehouse
	bash tests/run.sh

clean:
	rm -f bin/*.o bin/warehouse

.PHONY: all test clean
