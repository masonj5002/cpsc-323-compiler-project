CXX = clang++
CXXFLAGS =  -Wall -Wextra -std=c++23

TARGET = main
SOURCES = main.cpp lexer.cpp
OBJECTS = main.o lexer.o

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

main.o: main.cpp lexer.h
	$(CXX) $(CXXFLAGS) -c main.cpp

lexer.o: lexer.cpp lexer.h
	$(CXX) $(CXXFLAGS) -c lexer.cpp

clean:
	rm -f $(TARGET) $(OBJECTS)
