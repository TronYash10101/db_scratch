CC = clang++
# Added -std=c++17 because std::variant and std::filesystem require it!
CCF = -g -Wall -std=c++17 

# Ensure the directory exists before compiling
$(shell mkdir -p test_bins)

all: test_bins/schm_test

test_bins/schm_test : ./schema_manager_test.cpp ../schema_manager.cpp
	$(CC) $(CCF) $^ -o $@

clean:
	rm -rf test_bins/
	
.PHONY: all clean
