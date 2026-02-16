#include "headers/access_methods.hpp"
#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <vector>

access_methods::HeapTable::HeapTable() : Table() {}

std::optional<access_methods::RID>
access_methods::HeapTable::heap_scan(buffer_manager::buffer_pool &buff_pool,
                                     SARG sarg) {

  for (auto it = Table.begin(); it != Table.end(); ++it) {
    page_id pid = *it;
    char *heap_page_data = buff_pool.page_access(pid)->page_data;
    HeapPage *heap_page = reinterpret_cast<HeapPage *>(heap_page_data);

    // iterate each tuple and if tuple satisfies SARG return
    for (int slot = 0; slot < heap_page->page_header.slot_count; slot++) {

      if (heap_page->slots[slot].deleted) {
        continue;
      }

      uint16_t size = heap_page->slots[slot].slot_size;
      uint16_t offset = heap_page->slots[slot].slot_offset;
      // read
      row_t *match_to_row = reinterpret_cast<row_t *>(heap_page->data + offset);
      if (sarg.match(*match_to_row)) {
        RID row_id;
        row_id.pid = pid;
        row_id.slot = heap_page->slots[slot];
        buff_pool.un_pin(pid);
        return row_id;
      } else {
        std::cout << "No rid ";
        continue;
      };
    }
    buff_pool.un_pin(pid);
  }
  return std::nullopt;
}

void access_methods::HeapTable::heap_table_push(page_id pid) {
  Table.push_back(pid);
}

void access_methods::HeapTable::delete_slot(
    buffer_manager::buffer_pool &buff_pool, RID rid) {
  HeapPage *heap_page_data =
      reinterpret_cast<HeapPage *>(buff_pool.page_access(rid.pid)->page_data);

  uint16_t offset = rid.slot.slot_offset;

  Slot *del_slot = reinterpret_cast<Slot *>(heap_page_data->data + offset);

  if (del_slot->deleted != true) {
    del_slot->deleted = true;
  } else {
    throw std::runtime_error("Already Deleted(double delete)");
  }
}
