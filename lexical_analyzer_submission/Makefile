CXX = g++
CXXFLAGS =  -Wall -Wextra -std=c++23

TARGET = lexical_analyzer
SOURCES = lexical_analyzer.cpp
OBJECTS = lexical_analyzer.o

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

lexical_analyzer.o: lexical_analyzer.cpp fsm.hpp
	$(CXX) $(CXXFLAGS) -c lexical_analyzer.cpp

clean:
	rm -f $(TARGET) $(OBJECTS)
