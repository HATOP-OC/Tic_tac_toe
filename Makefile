# Makefile for cross-platform build (Linux and Windows)

CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -pthread

# Cross-compiler for Windows (install mingw-w64)
WIN_CC := x86_64-w64-mingw32-g++
WIN_CXXFLAGS := -std=c++17 -O2 -static -static-libgcc -static-libstdc++ -Wall -Wextra -pthread
WIN_LDFLAGS := -lwinpthread

SRC := main.cpp
TARGET_LINUX := tic_tac_toe
TARGET_WINDOWS := tic_tac_toe.exe

all: linux windows

# Build for Linux
tlinux: linux

linux: $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET_LINUX)

# Build for Windows using mingw-w64
windows: $(SRC)
	$(WIN_CC) $(WIN_CXXFLAGS) $(SRC) $(WIN_LDFLAGS) -o $(TARGET_WINDOWS)

clean:
	rm -f $(TARGET_LINUX) $(TARGET_WINDOWS)
