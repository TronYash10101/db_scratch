CXX = clang++
CXX_FLAGS = -Wall -g -O0 -std=c++20 -I../headers

BIN_DIR = test_bins

all : $(BIN_DIR)/lexer_test

$(BIN_DIR)/lexer_test: lexer_test.cpp ../lexer.cpp
	$(CXX) $(CXX_FLAGS) $^ -o $@

clean:
	rm -f $(BIN_DIR)/lexer_test

.PHONY: clean
