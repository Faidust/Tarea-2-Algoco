CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra

SRC = grid_pathfinding.cpp algorithms/greedy.cpp algorithms/brute_force.cpp algorithms/dynamic_programming.cpp

BIN = grid_pathfinding

.PHONY: all test small full clean

all: $(BIN)

$(BIN): $(SRC) path_result.h
	$(CXX) $(CXXFLAGS) $(SRC) -o $(BIN)

test: $(BIN)
	./$(BIN) greedy data/grid_input/4_4_mixta_D10_a.txt
	./$(BIN) dp data/grid_input/4_4_mixta_D10_a.txt
	./$(BIN) bruteforce data/grid_input/4_4_mixta_D10_a.txt

small: $(BIN)
	rm -f data/measurements/measurements.csv
	@for archivo in data/grid_input/4_4_*.txt data/grid_input/4_8_*.txt; do \
		for algoritmo in greedy dp bruteforce; do \
			./$(BIN) $$algoritmo "$$archivo" || exit 1; \
		done; \
	done

full: $(BIN)
	rm -f data/measurements/measurements.csv
	@for archivo in data/grid_input/*.txt; do \
		read n m v p < "$$archivo"; \
		./$(BIN) greedy "$$archivo" || exit 1; \
		./$(BIN) dp "$$archivo" || exit 1; \
		if [ $$((n + m)) -le 32 ]; then \
			./$(BIN) bruteforce "$$archivo" || exit 1; \
		fi; \
	done

clean:
	rm -f $(BIN)
