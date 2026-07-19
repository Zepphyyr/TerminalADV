# Desktop test build for KODZIMIM.
#   make        -> builds ./kodzimim
#   make run    -> builds and plays the prologue in your terminal
#   make clean
CXX      ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra
SRC      := src/desktop/main.cpp src/core/game.cpp

# On Windows g++ produces kodzimim.exe; everywhere else just kodzimim.
ifeq ($(OS),Windows_NT)
  BIN := kodzimim.exe
else
  BIN := kodzimim
endif

$(BIN): $(SRC) src/core/game.h src/core/platform.h src/desktop/platform_desktop.h
	$(CXX) $(CXXFLAGS) $(SRC) -o $(BIN)

run: $(BIN)
	./$(BIN) content save.dat

clean:
	rm -f $(BIN) save.dat

.PHONY: run clean
