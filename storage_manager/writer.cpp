#include "headers/writer.hpp"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <stdexcept>

void heap_write(char *raw_buffer, const access_methods::row_t &row) {

  HeapPage *heap_page = reinterpret_cast<HeapPage *>(raw_buffer);

  if (heap_page->page_header.free_size == page_data_size) {
    heap_page->initialize();
  }

  // header fill
  if (heap_page->page_header.slot_count < MAX_SLOTS) {
    heap_page->page_header.slot_count += 1;
  } else {
    throw std::runtime_error("Slots Full");
  }

  // Slot fill
  heap_page->slots[heap_page->page_header.slot_count - 1].slot_size =
      sizeof(row);

  heap_page->page_header.free_size -= sizeof(row);

  heap_page->slots[heap_page->page_header.slot_count - 1].slot_offset =
      heap_page->page_header.free_size;

  heap_page->slots[heap_page->page_header.slot_count - 1].deleted = false;

  // Data fill
  memcpy(
      heap_page->data +
          heap_page->slots[heap_page->page_header.slot_count - 1].slot_offset,
      &row, sizeof(access_methods::row_t));
}
