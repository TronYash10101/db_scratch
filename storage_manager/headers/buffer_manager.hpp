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
#include <unordered_map>
#include <vector>

namespace buffer_manager {

class buffer_pool {
  private:
    std::vector<buffer_manager_types::Page> frames;
    std::unordered_map<heap_page_types::page_id, buffer_manager_types::frame_id> page_table;
    std::queue<buffer_manager_types::frame_id> replacement_check_queue;
    Disk_operator disk_operator;

  public:
    // MUST declare the constructor here if you define it in the cpp
    buffer_pool(const std::string &db_filename, const std::string &index_filename);

    buffer_manager_types::frame_id page_replacement_policy();
    buffer_manager_types::Page *page_access(heap_page_types::page_id pid);
    void un_pin(int pid);
};
}; // namespace buffer_manager

#endif
