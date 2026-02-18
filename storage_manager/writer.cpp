#include "headers/writer.hpp"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <stdexcept>

void heap_write(char *raw_buffer, const access_methods_types::row_t &row) {

    heap_page_types::HeapPage *heap_page = reinterpret_cast<heap_page_types::HeapPage *>(raw_buffer);

    if (heap_page->page_header.free_size == buffer_manager_types::page_data_size) {
        heap_page->initialize();
    }

    // header fill
    if (heap_page->page_header.slot_count < heap_page_types::MAX_SLOTS) {
        heap_page->page_header.slot_count += 1;
    } else {
        throw std::runtime_error("Slots Full");
    }

    // Slot fill
    heap_page->slots[heap_page->page_header.slot_count - 1].slot_size = sizeof(row);

    heap_page->page_header.free_size -= sizeof(row);

    heap_page->slots[heap_page->page_header.slot_count - 1].slot_offset = heap_page->page_header.free_size;

    heap_page->slots[heap_page->page_header.slot_count - 1].deleted = false;

    // Data fill
    memcpy(heap_page->data + heap_page->slots[heap_page->page_header.slot_count - 1].slot_offset, &row,
           sizeof(access_methods_types::row_t));
}
void delete_slot(buffer_manager::buffer_pool &buff_pool, heap_page_types::RID rid) {
    heap_page_types::HeapPage *heap_page_data = reinterpret_cast<heap_page_types::HeapPage *>(buff_pool.page_access(rid.pid)->page_data);

    uint16_t offset = rid.slot.slot_offset;

    heap_page_types::Slot *del_slot = reinterpret_cast<heap_page_types::Slot *>(heap_page_data->data + offset);

    if (del_slot->deleted != true) {
        del_slot->deleted = true;
    } else {
        throw std::runtime_error("Already Deleted(double delete)");
    }
}
