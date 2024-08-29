# Makefile

# Specify the C++ compiler and compilation flags
CXX := g++
# CXXFLAGS := -Wall -Wextra -O2

# List of source files
SRC_FILES := script/inq.cpp script/kouq.cpp script/islip.cpp

# Corresponding executables
EXE_FILES := $(SRC_FILES:.cpp=.exe)

# Default target: compile all C++ files
all: $(EXE_FILES)

# Rule to compile each C++ file into an executable
script/%.exe: script/%.cpp
	$(CXX) -o $@ $<

# Clean target to remove all .exe and *-output.txt files in the script/ folder
clean:
	@echo "Deleting .exe files in script/ folder..."
	@if exist script\*.exe del /Q script\*.exe
	@echo "Deleting *-output.txt files in script/ folder..."
	@if exist script\*-output.txt del /Q script\*-output.txt
	@echo "Clean up in script/ folder completed."

.PHONY: all clean
