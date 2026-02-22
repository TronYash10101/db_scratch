#ifndef BUFFER_MANAGER
#define BUFFER_MANAGER

#include "disk_operator.hpp"
#include "types.hpp"
#include <chrono>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <queue>
#include <stdexcept>
#include <stdio.h>
#include <unordered_map>
#include <vector>

namespace buffer_manager {

typedef std::unordered_map<heap_page_types::page_id, buffer_manager_types::frame_id> Table_t;
typedef std::queue<buffer_manager_types::frame_id> Queue_t;
typedef std::vector<buffer_manager_types::Page> Frame_t;

class buffer_pool {
  private:
    Frame_t frames;

    Table_t table;

    Queue_t replacement_check_queue;
    Disk_operator disk_operator;

  public:
    // MUST declare the constructor here if you define it in the cpp
    buffer_pool(const std::string &db_filename, const std::string &index_filename);

    buffer_manager_types::Page *page_access(heap_page_types::page_id pid, diskoperator_types::page_type type);
    buffer_manager_types::frame_id page_replacement_policy(diskoperator_types::page_type type);
    void un_pin(heap_page_types::page_id pid, diskoperator_types::page_type type);
};
}; // namespace buffer_manager

#endif
