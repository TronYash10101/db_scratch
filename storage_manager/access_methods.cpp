#include "headers/access_methods.hpp"
#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <vector>

access_methods::Access_methods::Access_methods() : HeapTable() {}

std::optional<heap_page_types::RID> access_methods::Access_methods::heap_scan(buffer_manager::buffer_pool &buff_pool,
                                                                              access_methods_types::SARG sarg) {

    for (auto it = HeapTable.begin(); it != HeapTable.end(); ++it) {
        heap_page_types::page_id pid = *it;
        char *heap_page_data = buff_pool.page_access(pid, diskoperator_types::HEAP_PAGE)->page_data;
        heap_page_types::HeapPage *heap_page = reinterpret_cast<heap_page_types::HeapPage *>(heap_page_data);

        // iterate each tuple and if tuple satisfies SARG return
        for (int slot = 0; slot < heap_page->page_header.slot_count; slot++) {

            if (heap_page->slots[slot].deleted) {
                continue;
            }

            uint16_t size = heap_page->slots[slot].slot_size;
            uint16_t offset = heap_page->slots[slot].slot_offset;
            // read
            access_methods_types::row_t *match_to_row = reinterpret_cast<access_methods_types::row_t *>(heap_page->data + offset);
            if (sarg.match(*match_to_row)) {
                heap_page_types::RID row_id;
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

void access_methods::Access_methods::heap_table_push(heap_page_types::page_id pid) { HeapTable.push_back(pid); }
