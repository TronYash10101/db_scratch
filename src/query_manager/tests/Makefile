CXX = clang++
CXX_FLAGS = -Wall -g -O0 -std=c++20 -I../headers -I../../catalog_manager/headers

BIN_DIR = test_bins

all : $(BIN_DIR)/lexer_test $(BIN_DIR)/parser_test $(BIN_DIR)/planner_test

$(BIN_DIR)/lexer_test: lexer_test.cpp ../lexer.cpp ../../catalog_manager/schema_manager.cpp
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXX_FLAGS) $^ -o $@

$(BIN_DIR)/parser_test: parser_test.cpp ../parser.cpp ../lexer.cpp ../../catalog_manager/schema_manager.cpp
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXX_FLAGS) $^ -o $@

$(BIN_DIR)/planner_test: planner_test.cpp ../planner.cpp ../parser.cpp ../lexer.cpp ../../storage_manager/access_methods.cpp ../../storage_manager/buffer_manager.cpp ../../storage_manager/insert.cpp ../../storage_manager/index_writer.cpp ../../storage_manager/heap_writer.cpp ../../catalog_manager/schema_manager.cpp
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXX_FLAGS) $^ -o $@
			

clean:
	rm -rf $(BIN_DIR)

.PHONY: clean all

