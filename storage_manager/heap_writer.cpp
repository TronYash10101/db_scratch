#include "headers/heap_writer.hpp"
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <numeric>
#include <stdexcept>

heap_page_types::Slot heap_writer::heap_write(char *raw_heap_page, const access_methods_types::row_t &row,
                                              const std::vector<size_t> &row_data_size_arr) {

    heap_page_types::HeapPage *heap_page = reinterpret_cast<heap_page_types::HeapPage *>(raw_heap_page);

    if (heap_page->page_header.is_initialized == false) {
        std::cout << "INITIALIZED PAGE";
        heap_page->initialize();
    }
    // header fill
    if (heap_page->page_header.slot_count < heap_page_types::MAX_SLOTS) {
        heap_page->page_header.slot_count += 1;
    } else {
        throw std::runtime_error("Slots Full");
    }

    size_t row_size = std::accumulate(row_data_size_arr.begin(), row_data_size_arr.end(), size_t(0));

    // Slot fill
    heap_page->slots[heap_page->page_header.slot_count - 1].slot_size = row_size;

    if (row_size <= heap_page->page_header.free_size) {
        heap_page->page_header.free_size -= row_size;
    } else {
        throw std::runtime_error("HEAP PAGE SPACE FULL");
    }

    heap_page->slots[heap_page->page_header.slot_count - 1].slot_offset = heap_page->page_header.free_size;

    heap_page->slots[heap_page->page_header.slot_count - 1].deleted = false;

    // Data fill
    size_t cum_offset = 0;
    for (int i = 0; i < row.row.size(); i++) {
        memcpy(heap_page->data + heap_page->slots[heap_page->page_header.slot_count - 1].slot_offset + cum_offset, &row.row[i],
               row_data_size_arr[i]);
        cum_offset += row_data_size_arr[i];
    }

    return heap_page->slots[heap_page->page_header.slot_count - 1];
}

void heap_writer::delete_slot(buffer_manager::buffer_pool &buff_pool, heap_page_types::RID rid) {
    buffer_manager_types::Page *page = buff_pool.page_access(rid.pid, diskoperator_types::HEAP_PAGE);
    heap_page_types::HeapPage *heap_page_data = reinterpret_cast<heap_page_types::HeapPage *>(page->page_data);

    uint16_t offset = rid.slot.slot_offset;

    heap_page_types::Slot *del_slot = reinterpret_cast<heap_page_types::Slot *>(heap_page_data->data + offset);

    if (del_slot->deleted != true) {
        del_slot->deleted = true;
        page->dirty_bit = true;
    } else {
        buff_pool.un_pin(rid.pid, diskoperator_types::HEAP_PAGE);
        throw std::runtime_error("Already Deleted(double delete)");
    }

    buff_pool.un_pin(rid.pid, diskoperator_types::HEAP_PAGE);
}
