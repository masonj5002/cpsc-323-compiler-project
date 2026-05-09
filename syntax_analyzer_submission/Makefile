CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++23

TARGET = main

all: $(TARGET)

$(TARGET): main.cpp fsm.hpp lexical_analyzer.hpp prdp.hpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.cpp

# usage: make run INPUT=input_test_file_2.txt OUTPUT=output.txt
run: $(TARGET)
	./$(TARGET) $(INPUT) $(OUTPUT)

clean:
	rm -f $(TARGET)
