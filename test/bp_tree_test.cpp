#include "../storage_manager/headers/access_methods.hpp"
#include <filesystem>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <vector>

struct root_struct {
    heap_page_types::page_id root_pid;
};

buffer_manager_types::Page *fetch_page(buffer_manager::buffer_pool &buff_pool) {

    /* In order for this function to work pages should be actually written to disk and not changed in memory */

    uintmax_t last_pid = buff_pool.get_last_pid(diskoperator_types::INDEX_PAGE);

    buffer_manager_types::Page *p1 = buff_pool.page_access(last_pid, diskoperator_types::INDEX_PAGE);

    btree_page_types::Node *p1n = reinterpret_cast<btree_page_types::Node *>(p1);
    p1n->init();
    return p1;
}

buffer_manager_types::Page *start(buffer_manager::buffer_pool &buff_pool, buffer_manager_types::Page *root_page, root_struct curr_root) {
    heap_page_types::page_id root_pid = curr_root.root_pid;

    root_page = buff_pool.page_access(root_pid, diskoperator_types::INDEX_PAGE);

    return root_page;
}

std::optional<access_methods::internal_split_res> insert(buffer_manager::buffer_pool &buff_pool,
                                                         access_methods::Access_methods &access_methods, heap_page_types::page_id curr_pid,
                                                         heap_page_types::page_id prev_pid, int key, heap_page_types::RID rid) {

    buffer_manager_types::Page *curr_page = buff_pool.page_access(curr_pid, diskoperator_types::INDEX_PAGE);
    btree_page_types::Node *node = reinterpret_cast<btree_page_types::Node *>(curr_page->page_data);
    heap_page_types::page_id next_pid;
    std::optional<access_methods::internal_split_res> internal_res;

    if (node->is_leaf) {
        buffer_manager_types::Page *new_page = fetch_page(buff_pool);
        buffer_manager_types::Page *leaf_page = buff_pool.page_access(curr_pid, diskoperator_types::INDEX_PAGE);

        std::optional<access_methods::leaf_split_res> leaf_res =
                access_methods.bptree_leaf_insert(leaf_page, new_page, leaf_page->page_id, new_page->page_id, key, rid);
        std::cout << "\n This call \n" << leaf_res.has_value();
        if (leaf_res.has_value()) {
            // std::cout << "I got here" << internal_res->promoted_key;
            buffer_manager_types::Page *prev_new_page = fetch_page(buff_pool);
            buffer_manager_types::Page *prev_page = buff_pool.page_access(prev_pid, diskoperator_types::INDEX_PAGE);
            std::optional<access_methods::internal_split_res> internal_res = access_methods.bptree_internal_insert(
                    prev_page, prev_new_page, prev_page->page_id, prev_new_page->page_id, leaf_res->new_pid, leaf_res->promoted_key);
            return internal_res;
        }
    } else if (node->is_leaf == false) {
        /* for (int i = 1; i <= node->key_count - 2; i++) {
            if (key < node->keys[i - 1]) {
                next_pid = node->data.internal_node.child_nodes[i - 1];
            } else if (node->keys[i] <= key && key < node->keys[i + 1]) {
                next_pid = node->data.internal_node.child_nodes[i];
            } else if (node->keys[i + 1] <= key) {
                next_pid = node->data.internal_node.child_nodes[i + 1];
            }
        } */
        int i = 0;
        while (i < node->key_count && key >= node->keys[i])
            i++;

        next_pid = node->data.internal_node.child_nodes[i];
        std::optional<access_methods::internal_split_res> child_node_ans = insert(buff_pool, access_methods, next_pid, curr_pid, key, rid);
        if (child_node_ans.has_value()) {
            buffer_manager_types::Page *prev_new_page = fetch_page(buff_pool);
            buffer_manager_types::Page *prev_page = buff_pool.page_access(prev_pid, diskoperator_types::INDEX_PAGE);
            std::optional<access_methods::internal_split_res> internal_res =
                    access_methods.bptree_internal_insert(prev_page, prev_new_page, prev_page->page_id, prev_new_page->page_id,
                                                          child_node_ans->new_pid, child_node_ans->promoted_key);
            // update root
            if ((prev_pid == curr_pid) && internal_res.has_value()) {
                buffer_manager_types::Page *new_root = fetch_page(buff_pool);
                btree_page_types::Node *new_node = reinterpret_cast<btree_page_types::Node *>(curr_page->page_data);
                new_node->init();
                // look out for NULLs
                access_methods.bptree_internal_insert(new_root, NULL, new_root->page_id, buffer_manager_types::INVALID_PAGE_ID,
                                                      internal_res->new_pid, internal_res->promoted_key);
            }
        }
    }
    return std::nullopt;
}

int main() {

    std::filesystem::path heap_filepath = "/home/yash-jadhav/db_scratch/heap.bin";
    std::filesystem::path index_filepath = "/home/yash-jadhav/db_scratch/index.bin";

    buffer_manager::buffer_pool buff_pool(heap_filepath, index_filepath);

    access_methods::Access_methods access_methods;

    buffer_manager_types::Page *root_page = fetch_page(buff_pool);

    heap_page_types::RID samp_rid = {0, 0};
    btree_page_types::Node *root_leaf = reinterpret_cast<btree_page_types::Node *>(root_page);

    // dont see rid
}

/* for (int x = 0; x < 4; x++) {
    std::optional<access_methods::leaf_split_res> res = access_methods.bptree_leaf_insert(
            samp_page[0]->page_data, samp_page[1]->page_data, samp_page[1]->page_id, x, samp_page[0]->page_id, sample_rid1);

    std::cout << "\n--- After inserting key " << x << " ---\n";

    std::cout << "Leaf Page ID: " << samp_page[0]->page_id << "\n";
    std::cout << "Key Count: " << leaf_page->key_count << "\n";

    std::cout << "Right Page ID: " << samp_page[1]->page_id << "\n";
    std::cout << "Key Count: " << leaf_page_next->key_count << "\n";

    std::cout << "[ ";
    for (int i = 0; i < leaf_page->key_count; i++) {
        std::cout << leaf_page->keys[i] << " ";
    }
    std::cout << "]\n";

    std::cout << "[ ";
} */
