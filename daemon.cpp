#include "storage_manager/headers/index_writer.hpp"
#include "storage_manager/headers/insert.hpp"
#include "storage_manager/headers/types.hpp"
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

int main() {
    std::filesystem::path heap_filepath = std::filesystem::current_path() / "heap.bin";
    std::filesystem::path index_filepath = std::filesystem::current_path() / "index.bin";

    std::ofstream file1(index_filepath, std::ios::binary | std::ios::out | std::ios::trunc);
    std::ofstream file2(heap_filepath, std::ios::binary | std::ios::out | std::ios::trunc);
    file1.close();
    file2.close();

    buffer_manager::buffer_pool buff_pool(heap_filepath, index_filepath);
    access_methods::Access_methods access_methods;

    // key on column x
    // access_methods_types::row_t row1 = {5, 6};

    index_write::root_struct curr_root;
    curr_root.root_pid = buffer_manager_types::INVALID_PAGE_ID;
    std::vector<access_methods_types::row_t> entries = {{5, 6}, {1, 2}, {3, 4}, {34, 23}, {12, 90}};
    heap_page_types::page_id root_id;
    buffer_manager_types::Page *root_page = index_write::fetch_page(buff_pool);
    for (const access_methods_types::row_t row : entries) {
        root_id = insert::create_entry(buff_pool, access_methods, row, curr_root);
    }

    heap_page_types::RID res_rid = access_methods.bptree_scan(buff_pool, root_id, 3);

    buffer_manager_types::Page *to_pg = buff_pool.page_access(res_rid.pid, diskoperator_types::HEAP_PAGE);
    heap_page_types::HeapPage *xz = reinterpret_cast<heap_page_types::HeapPage *>(to_pg->page_data);
    access_methods_types::row_t *res_row = reinterpret_cast<access_methods_types::row_t *>(xz->data + res_rid.slot.slot_offset);

    std::cout << "X: " << res_row->x << "Y: " << res_row->y;

    return 0;
}
