#include "storage_manager/headers/writer.hpp"
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

    access_methods::HeapPage *heap_page_t =
        reinterpret_cast<access_methods::HeapPage *>(page);

    heap_page_t->page_header.slot_count = 0;
    heap_page_t->page_header.free_offset = page_data_size;
    /*access_methods::row_t row2 = {4, 5};
    write_page(page->page_data, row2);

    access_methods::HeapPage *hp =
        reinterpret_cast<access_methods::HeapPage *>(page->page_data);

    std::cout << "slot_count = " << hp->page_header.slot_count << "\n";
    std::cout << "free_offset = " << hp->page_header.free_offset << "\n";

    for (int i = 0; i < hp->page_header.slot_count; i++) {
      uint16_t off = hp->slots[i].slot_offset;
      access_methods::row_t *r =
          reinterpret_cast<access_methods::row_t *>(page->page_data + off);
      std::cout << "row[" << i << "] = {" << r->x << ", " << r->y << "}\n";
    } */

    access_methods::row_t row1 = {2, 7};

    write_page(page->page_data, row1);

    access_methods::SARG s1 = {access_methods::X, access_methods::EQ, 2};
    std::optional<access_methods::RID> res =
        heap_table->heap_scan(*buff_pool, s1);
    if (res) {
      buffer_manager::Page *res_page = buff_pool->page_access(res->pid);

      access_methods::HeapPage *hp =
          reinterpret_cast<access_methods::HeapPage *>(res_page->page_data);

      access_methods::row_t *pd = reinterpret_cast<access_methods::row_t *>(
          hp->data + res->slot.slot_offset);
      std::cout << pd->x << pd->y;
    } else {
      std::cout << "No match found";
    }
  } else {
    std::cout << "Path does not exists";
  }
  return 0;
}
