#include "../storage_manager/headers/access_methods.hpp"
#include <filesystem>
#include <iostream>

int main() {

    // Call page_access to get a new page into RAM
    // Fill index_table, which maps node_id --> frame_id
    // Optional: also get index_replacement queue

    std::filesystem::path heap_filepath = std::filesystem::current_path() / "heap.bin";
    std::filesystem::path index_filepath = std::filesystem::current_path() / "index.bin";

    buffer_manager::buffer_pool buff_pool(heap_filepath, index_filepath);

    access_methods::Access_methods access_methods;

    buffer_manager_types::Page *raw_page = buff_pool.page_access(3, diskoperator_types::INDEX_PAGE);
    buffer_manager_types::Page *raw_page_next = buff_pool.page_access(2, diskoperator_types::INDEX_PAGE);
    heap_page_types::RID sample_rid = {raw_page->page_id, 1};

    btree_page_types::Node *leaf_page = reinterpret_cast<btree_page_types::Node *>(raw_page->page_data);
    btree_page_types::Node *leaf_page_next = reinterpret_cast<btree_page_types::Node *>(raw_page_next->page_data);

    leaf_page->init();
    leaf_page_next->init();

    for (int x = 0; x < 4; x++) {

        access_methods.bptree_leaf_insert(raw_page->page_data, raw_page_next->page_data, raw_page->page_id, x, raw_page->page_id,
                                          sample_rid);

        std::cout << "\n--- After inserting key " << x << " ---\n";

        std::cout << "Leaf Page ID: " << raw_page->page_id << "\n";
        std::cout << "Key Count: " << leaf_page->key_count << "\n";

        std::cout << "Right Page ID: " << raw_page_next->page_id << "\n";
        std::cout << "Key Count: " << leaf_page_next->key_count << "\n";

        std::cout << "[ ";
        for (int i = 0; i < leaf_page->key_count; i++) {
            std::cout << leaf_page->keys[i] << " ";
        }
        std::cout << "]\n";

        std::cout << "[ ";
        for (int i = 0; i < leaf_page_next->key_count; i++) {
            std::cout << leaf_page_next->keys[i] << " ";
        }
        std::cout << "]\n";
    }
    // std::cout << "Next Leaf: " << leaf_page_next->data.leaf_node.next_leaf;
}
