#include "headers/access_methods.hpp"
#include <stdexcept>
#include <tuple>

access_methods::HeapTable::HeapTable() : Table(MAX_PAGES) {}

access_methods::row_t *
access_methods::HeapTable::heap_scan(buffer_manager::buffer_pool &buff_pool,
                                     SARG sarg) {

  for (auto i = Table.begin(); i < Table.end(); i++) {
    if (i != Table.end()) {
      char *heap_page_data = buff_pool.page_access(*i)->page_data;
      HeapPage *heap_page = reinterpret_cast<HeapPage *>(heap_page_data);

      // iterate each tuple and if tuple satisfies SARG return
      for (int x = 0; x < heap_page->page_header.slot_count; x++) {
        uint16_t size = heap_page->slots[x].slot_size;
        uint16_t offset = heap_page->slots[x].slot_offset;
        // read
        row_t *match_to_row = reinterpret_cast<row_t *>(
            heap_page_data + offset + heap_page->page_header.h_offset);
        if (sarg.match(*match_to_row)) {
          return match_to_row;
        } else {
          continue;
        };
      }
    } else {
      throw std::runtime_error("Could Not Find Specified Page");
    }
  }
  return NULL;
}
