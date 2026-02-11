#include "headers/buffer_manager.hpp"
#include "headers/disk_operator.hpp"
#include "headers/writer.hpp"
#include <cstring>
#include <iostream>
#include <stdexcept>

buffer_manager::buffer_pool::buffer_pool(const std::string &filename)
    : disk_operator(filename, page_size), frames(buffer_size) {
  std::cout << "\nBUFFER CREATED\n";
}

buffer_manager::Page *buffer_manager::buffer_pool::page_access(page_id pid) {
  auto page = page_table.find(pid);
  if (page != page_table.end()) {
    frames[page->second].pin_count += 1;
    frames[page->second].dirty_bit = true;
    std::cout << "\nPage Found\n";
    return &frames[page->second];
  } else {
    for (auto i = 0; i != frames.size(); ++i) {
      if (frames[i].page_id == INVALID_PAGE_ID) {
        frames[i].page_id = pid;
        frames[i].pin_count = 1;
        page_table[pid] = i;
        replacement_check_queue.push(i);
        disk_operator.read_page(pid, frames[i].page_data);
        std::cout << "\nNew Page Created\n";
        return &frames[i];
      }
    }
  }
  frame_id free_frame_id = page_replacement_policy();
  std::cout << "Page Replaced Successfully";
  return page_access(free_frame_id);
}

void buffer_manager::buffer_pool::un_pin(int pid) {
  auto page = page_table.find(pid);
  frame_id frame_idx;

  if (page != page_table.end()) {
    frame_idx = page->second;
  } else {
    throw std::runtime_error("page not found");
  }
  if (frame_idx > buffer_size) {
    std::cout << "fault";
  }
  if (frames[frame_idx].pin_count > 0) {
    frames[frame_idx].pin_count -= 1;
  }
}

frame_id buffer_manager::buffer_pool::page_replacement_policy() {
  int idx;
  for (idx = 0; idx < replacement_check_queue.size(); idx++) {
    frame_id id = replacement_check_queue.front();
    replacement_check_queue.pop();
    if (frames[id].pin_count == 0) {
      auto it = page_table.find(frames[id].page_id);

      if (it != page_table.end()) {
        // write data here
        write_page(frames[id].page_data, {(int)id + 1, (int)id + 2});
        disk_operator.write_page(it->first, frames[id].dirty_bit,
                                 frames[id].page_data);
        page_table.erase(it->first);
      }

      frames[id].page_id = INVALID_PAGE_ID;
      return id;
    } else if (frames[id].pin_count > 0) {
      replacement_check_queue.push(id);
      continue;
    } else {
      throw std::runtime_error("Page replacement error, pin count invalid");
    }
  }
  throw std::runtime_error("All pages are pinned");
};
