#ifndef BUFFER_MANAGER
#define BUFFER_MANAGER

#include "disk_operator.hpp"
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

constexpr std::size_t page_data_size = 4096;
constexpr std::size_t page_size =
    page_data_size + 65; // look out for hardcoding
constexpr std::size_t buffer_size = 16;
constexpr std::size_t MAX_PAGES = 32;
typedef size_t page_id;
typedef size_t frame_id;
constexpr frame_id INVALID_FRAME = static_cast<frame_id>(-1);
constexpr frame_id INVALID_PAGE_ID = static_cast<frame_id>(-1);

namespace buffer_manager {

struct Page {
  char page_data[page_size];
  size_t page_id = INVALID_PAGE_ID; // gives the max value of size_t
  bool dirty_bit = false;
  int pin_count = 0;
};

class buffer_pool {
private:
  std::vector<Page> frames;
  std::unordered_map<page_id, frame_id> page_table;
  std::queue<frame_id> replacement_check_queue;
  Disk_operator disk_operator;

public:
  // MUST declare the constructor here if you define it in the cpp
  buffer_pool(const std::string &filename);

  frame_id page_replacement_policy();
  buffer_manager::Page *page_access(page_id pid);
  void un_pin(int pid);
};

}; // namespace buffer_manager

#endif
