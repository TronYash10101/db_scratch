#include "headers/writer.hpp"
#include <cstring>

void write_page(char &raw_buffer, access_methods::HeapPage &abstract_page,
                const access_methods::row_t row) {
  access_methods::HeapPage *page =
      reinterpret_cast<access_methods::HeapPage *>(raw_buffer);
  abstract_page.page_header.slot_count += 1;

  if ((abstract_page.page_header.slot_count - 1) == 0) {
    abstract_page.slots[abstract_page.page_header.slot_count - 1].slot_offset =
        abstract_page.page_header.h_offset;
  } else {
    abstract_page.slots[abstract_page.page_header.slot_count - 1].slot_offset =
        abstract_page.page_header.h_offset +
        abstract_page.slots[abstract_page.page_header.slot_count - 2]
            .slot_offset;
  }

  abstract_page.slots[abstract_page.page_header.slot_count - 1].slot_size =
      sizeof(row);

  memmove(
      abstract_page
          .data[abstract_page.page_header.h_offset +
                abstract_page.slots[abstract_page.page_header.slot_count - 1]
                    .slot_offset],
      &row, sizeof(row));
  memcpy(raw_buffer, abstract_page, sizeof(abstract_page));
}
