# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -O2

# Target and source files
TARGET := time_test
SRC := time_test.cpp ChessBoard.cpp

# Build target
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

# Clean up build files
clean:
	rm -f $(TARGET)