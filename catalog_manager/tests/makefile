CC = clang++
CCF = -g -Wall

all: test_bins/schm_test

test_bins/schm_test : ./schema_manager_test.cpp ../schema_manager.cpp
	$(CC) $(CCF) $^ -o $@

clean: rm -f ./schm_test
