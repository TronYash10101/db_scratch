CXX = clang++
CXX_FLAGS = -Wall -g -O0 -std=c++20 -I../headers

BIN_DIR = test_bins
OBJ_DIR = objects

# -------- Core sources (outside folder) --------
CORE_SRC = \
	../renderer.cpp \
	../input_handler.cpp \
	../components.cpp \
	# ../screen.cpp

CORE_OBJ = $(patsubst ../%.cpp,$(OBJ_DIR)/%.o,$(CORE_SRC))

# -------- Test sources (inside this folder) --------
TEST_RENDERER = ./renderer_tests.cpp
# TEST_SCREEN   = screen_tests.cpp

# -------- Targets --------
TARGETS = \
	$(BIN_DIR)/renderer_tests \
	# $(BIN_DIR)/screen_tests

all: $(TARGETS)

# -------- Linking --------

$(BIN_DIR)/renderer_tests: $(CORE_OBJ) $(TEST_RENDERER)
	mkdir -p $(BIN_DIR)
	$(CXX) $(CXX_FLAGS) $^ -o $@

# $(BIN_DIR)/screen_tests: $(CORE_OBJ) $(TEST_SCREEN)
# 	mkdir -p $(BIN_DIR)
# 	$(CXX) $(CXX_FLAGS) $^ -o $@

# -------- Compile core → objects --------

$(OBJ_DIR)/%.o: ../%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXX_FLAGS) -c $< -o $@

# -------- Clean --------

clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)

.PHONY: all clean
