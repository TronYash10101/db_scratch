#ifndef DISK_OPERATOR
#define DISK_OPERATOR

#include "types.hpp"
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <utility>
#include <vector>

class Disk_operator {
  private:
    std::filesystem::path db_path;
    std::filesystem::path index_path;
    FILE *db_file;
    FILE *index_file;
    int PAGE_SIZE;

  public:
    Disk_operator(const std::string &db_filename, const std::string &index_filename, int page_size) {

        if (!std::filesystem::exists(db_filename)) {
            db_file = fopen(db_filename.c_str(), "w+b");
        } else {
            db_file = fopen(db_filename.c_str(), "r+b");
        }

        if (!std::filesystem::exists(index_filename)) {
            index_file = fopen(index_filename.c_str(), "w+b");
        } else {
            index_file = fopen(index_filename.c_str(), "r+b");
        }
        db_path = db_filename;
        index_path = index_filename;
        PAGE_SIZE = page_size;
    }

    void read_page(int pid, char *buffer, diskoperator_types::page_type type) {
        int offset = pid * PAGE_SIZE;

        if (type == diskoperator_types::HEAP_PAGE) {
            fseek(db_file, offset, SEEK_SET);
            if (fread(buffer, PAGE_SIZE, 1, db_file) == -1) {
                throw std::runtime_error("Could not read page");
            };
            std::cout << "\nRead a page\n";
        } else if (type == diskoperator_types::INDEX_PAGE) {
            fseek(index_file, offset, SEEK_SET);
            if (fread(buffer, PAGE_SIZE, 1, index_file) == -1) {
                throw std::runtime_error("Could not read page");
            };
            std::cout << "\nRead a page\n";
        }
    }

    void write_page(int pid, bool &dirty_bit, const char *write_data, diskoperator_types::page_type type) {
        FILE *file = (type == diskoperator_types::HEAP_PAGE) ? db_file : index_file;

        int offset = pid * PAGE_SIZE;

        fseek(file, offset, SEEK_SET);
        if (fwrite(write_data, PAGE_SIZE, 1, file) == 0) {
            throw std::runtime_error("Could not write page");
        }

        /* if (dirty_bit) {
            dirty_bit = false;
        } */
        fflush(file);
    }

    uintmax_t last_pid(diskoperator_types::page_type type) {
        if (type == diskoperator_types::HEAP_PAGE) {

            return std::filesystem::file_size(db_path) / PAGE_SIZE;
        } else if (type == diskoperator_types::INDEX_PAGE) {
            return std::filesystem::file_size(index_path) / PAGE_SIZE;
        }
        throw std::runtime_error("ERROR GETTING LAST PID");
    }
    ~Disk_operator() {
        if (db_file) {
            fclose(db_file);
        }
    }
};

#endif
