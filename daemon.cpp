#include "storage_manager/headers/insert.hpp"
#include "storage_manager/headers/types.hpp"
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>

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
    access_methods_types::row_t row1 = {5, 6};

    heap_page_types::page_id root_id = insert::create_entry(buff_pool, access_methods, row1);

    buffer_manager_types::Page *root_pg = buff_pool.page_access(root_id, diskoperator_types::INDEX_PAGE);
    btree_page_types::Node *root_n = reinterpret_cast<btree_page_types::Node *>(root_pg);

    while (!root_n->is_leaf) {
        root_pg = buff_pool.page_access(root_n->data.internal_node.child_nodes[0], diskoperator_types::INDEX_PAGE);
        root_n = reinterpret_cast<btree_page_types::Node *>(root_pg->page_data);
    }
    heap_page_types::page_id to_find_pid = root_n->data.leaf_node.values[0].pid;
    heap_page_types::Slot to_find_slot = root_n->data.leaf_node.values[0].slot;

    buffer_manager_types::Page *to_pg = buff_pool.page_access(to_find_pid, diskoperator_types::HEAP_PAGE);
    heap_page_types::HeapPage *xz = reinterpret_cast<heap_page_types::HeapPage *>(to_pg->page_data);
    access_methods_types::row_t *res_row = reinterpret_cast<access_methods_types::row_t *>(to_pg->page_data + to_find_slot.slot_offset);

    std::cout << res_row->x;

    return 0;
}
