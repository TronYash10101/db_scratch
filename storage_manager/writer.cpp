#include "headers/writer.hpp"
#include <cstdio>
#include <iostream>

void write_page(char *raw_buffer, const access_methods::row_t &row) {

  if (raw_buffer == NULL) {
    throw std::runtime_error("NULL buffer recieved by writer");
  }

  access_methods::HeapPage *abstract_page =
      reinterpret_cast<access_methods::HeapPage *>(raw_buffer);

  // reinterpret cast does not initialize, explict initialize
  /* if (abstract_page->page_header.slot_count == 0 &&
      abstract_page->page_header.free_offset == 0) {
    abstract_page->page_header.free_offset = page_data_size;
    abstract_page->page_header.slot_count = 0;
  } */

  if (abstract_page->page_header.slot_count >= access_methods::MAX_SLOTS) {
    throw std::runtime_error("no free slots");
  }

  abstract_page->page_header.slot_count += 1;

  if (sizeof(row) <= abstract_page->page_header.free_offset) {
    abstract_page->page_header.free_offset -= sizeof(row);
  } else {
    throw std::runtime_error(
        "Remaining Free Size: " +
        std::to_string(abstract_page->page_header.free_offset));
  }

  abstract_page->slots[abstract_page->page_header.slot_count - 1].slot_size =
      sizeof(row);
  abstract_page->slots[abstract_page->page_header.slot_count - 1].slot_offset =
      abstract_page->page_header.free_offset;

  memcpy(abstract_page->data +
             abstract_page->slots[abstract_page->page_header.slot_count - 1]
                 .slot_offset,
         &row, sizeof(row));
}
