#include "storage_manager/headers/writer.hpp"
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <memory>

int main() {
    std::filesystem::path heap_filepath = std::filesystem::current_path() / "heap.bin";
    std::filesystem::path index_filepath = std::filesystem::current_path() / "index.bin";

    std::cout << heap_filepath << "\n";

    auto buff_pool = std::make_unique<buffer_manager::buffer_pool>(heap_filepath.string(), index_filepath.string());

    /* auto heap_table = std::make_unique<access_methods::Access_methods>();

    buffer_manager::Page *page = buff_pool->page_access(4);
    heap_table->heap_table_push(4);

    buffer_manager::HeapPage *heap_page_t = reinterpret_cast<buffer_manager::HeapPage *>(page);

    access_methods::row_t row1 = {1, 5};

    heap_write(page->page_data, row1);

    // testing sargs
    access_methods::SARG s1 = {access_methods::X, access_methods::EQ, 1};
    std::optional<access_methods::RID> res = heap_table->heap_scan(*buff_pool, s1);
    if (res.has_value()) {
        buffer_manager::Page *res_page = buff_pool->page_access(res->pid);

        buffer_manager::HeapPage *hp = reinterpret_cast<buffer_manager::HeapPage *>(res_page->page_data);

        if (res->slot.deleted != false) {
            access_methods::row_t *pd = reinterpret_cast<access_methods::row_t *>(hp->data + res->slot.slot_offset);
            std::cout << "x: " << pd->x << "y: " << pd->y;
        } else {

            std::cout << "deleted";
        }
    } else {
        std::cout << "No match found";
    } */
    return 0;
}
