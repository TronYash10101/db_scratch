#include "storage_manager/headers/writer.hpp"
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <memory>

int main() {
  std::filesystem::path filepath = std::filesystem::current_path() / "data.bin";
  if (std::filesystem::exists(filepath)) {

    std::cout << filepath << "\n";

    auto buff_pool =
        std::make_unique<buffer_manager::buffer_pool>(filepath.string());

    auto heap_table = std::make_unique<access_methods::HeapTable>();

    buffer_manager::Page *page = buff_pool->page_access(4);
    heap_table->heap_table_push(4);

    HeapPage *heap_page_t = reinterpret_cast<HeapPage *>(page);

    access_methods::row_t row1 = {1, 5};

    heap_write(page->page_data, row1);

    // testing sargs
    access_methods::SARG s1 = {access_methods::X, access_methods::EQ, 1};
    std::optional<access_methods::RID> res =
        heap_table->heap_scan(*buff_pool, s1);
    heap_table->delete_slot(*buff_pool, res.value());
    if (res.has_value()) {
      buffer_manager::Page *res_page = buff_pool->page_access(res->pid);

      HeapPage *hp = reinterpret_cast<HeapPage *>(res_page->page_data);

      if (res->slot.deleted != false) {
        access_methods::row_t *pd = reinterpret_cast<access_methods::row_t *>(
            hp->data + res->slot.slot_offset);
        std::cout << "x: " << pd->x << "y: " << pd->y;
      } else {

        std::cout << "deleted";
      }
    } else {
      std::cout << "No match found";
    }
  } else {
    std::cout << "Path does not exists";
  }
  // scanf("%d", NULL);
  return 0;
}
