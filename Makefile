CXX = clang++
CXXFLAGS =  -Wall -Wextra -std=c++23

TARGET = main
SOURCES = main.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)
