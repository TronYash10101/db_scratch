#include "storage_manager/headers/buffer_manager.hpp"
#include <filesystem>
#include <iostream>
#include <memory>
#include <stdio.h>

int main() {
  std::filesystem::path filepath = std::filesystem::current_path() / "data.bin";
  if (std::filesystem::exists(filepath)) {

    std::cout << filepath << "\n";
    auto buff_pool =
        std::make_unique<buffer_manager::buffer_pool>(filepath.string());
    for (int i = 1; i <= 15; i++) {
      buff_pool->page_access(i);
    }
    buff_pool->un_pin(1);

  } else {

    std::cout << "Path does not exists";
  }

  return 0;
}
