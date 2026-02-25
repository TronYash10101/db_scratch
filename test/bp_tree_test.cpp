#include "../storage_manager/headers/access_methods.hpp"
#include <cassert>
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
    // p1n->init();
    return p1;
}

buffer_manager_types::Page *start(buffer_manager::buffer_pool &buff_pool, buffer_manager_types::Page *root_page, root_struct curr_root) {
    heap_page_types::page_id root_pid = curr_root.root_pid;

    root_page = buff_pool.page_access(root_pid, diskoperator_types::INDEX_PAGE);

    return root_page;
}

std::optional<access_methods::split_res> insert(buffer_manager::buffer_pool &buff_pool, access_methods::Access_methods &access_methods,
                                                heap_page_types::page_id curr_pid, int key, heap_page_types::RID rid,
                                                root_struct *curr_root) {

    buffer_manager_types::Page *curr_page = buff_pool.page_access(curr_pid, diskoperator_types::INDEX_PAGE);
    btree_page_types::Node *node = reinterpret_cast<btree_page_types::Node *>(curr_page->page_data);
    heap_page_types::page_id next_pid;
    std::optional<access_methods::split_res> res;
    std::optional<access_methods::split_res> child_node_ans;

    if (node->is_leaf) {

        buffer_manager_types::Page *leaf_page = buff_pool.page_access(curr_pid, diskoperator_types::INDEX_PAGE);
        buffer_manager_types::Page *new_page = nullptr;

        if (node->key_count == btree_page_types::MAX_KEYS) {
            new_page = fetch_page(buff_pool);
            new_page->dirty_bit = true;
        }
        res = access_methods.bptree_leaf_insert(leaf_page, new_page, leaf_page->page_id,
                                                new_page ? new_page->page_id : buffer_manager_types::INVALID_PAGE_ID, key, rid);
        leaf_page->dirty_bit = true;

    } else if (node->is_leaf == false) {

        int i = 0;
        while (i < node->key_count && key >= node->keys[i])
            i++;
        next_pid = node->data.internal_node.child_nodes[i];

        child_node_ans = insert(buff_pool, access_methods, next_pid, key, rid, curr_root);

        if (child_node_ans.has_value()) {
            buffer_manager_types::Page *right_new_page = fetch_page(buff_pool);
            btree_page_types::Node *right_new_node = reinterpret_cast<btree_page_types::Node *>(right_new_page->page_data);
            right_new_node->init();
            res = access_methods.bptree_internal_insert(curr_page, right_new_page, curr_page->page_id, right_new_page->page_id,
                                                        child_node_ans->child_pid, child_node_ans->promoted_key);
            right_new_page->dirty_bit = true;
        }
    }
    curr_page->dirty_bit = true;
    if (res.has_value() && curr_pid == curr_root->root_pid) {
        buffer_manager_types::Page *new_root_page = fetch_page(buff_pool);
        btree_page_types::Node *new_root_node = reinterpret_cast<btree_page_types::Node *>(new_root_page->page_data);
        new_root_node->init();
        new_root_node->is_leaf = false;
        new_root_node->key_count = 1;
        new_root_node->keys[0] = res->promoted_key;
        // heap_page_types::page_id old_root = curr_pid;
        curr_root->root_pid = new_root_page->page_id;

        new_root_node->data.internal_node.child_nodes[0] = curr_page->page_id;
        new_root_node->data.internal_node.child_nodes[1] = res->child_pid;
    } else if (res.has_value() && curr_pid != curr_root->root_pid) {
        return res;
    }

    return std::nullopt;
}

int main() {

    std::filesystem::path heap_filepath = "/home/yash-jadhav/db_scratch/heap.bin";
    std::filesystem::path index_filepath = "/home/yash-jadhav/db_scratch/index.bin";

    buffer_manager::buffer_pool buff_pool(heap_filepath, index_filepath);

    access_methods::Access_methods access_methods;

    heap_page_types::RID samp_rid = {0, 0};

    buffer_manager_types::Page *root_page = fetch_page(buff_pool);

    btree_page_types::Node *root_leaf = reinterpret_cast<btree_page_types::Node *>(root_page->page_data);

    root_leaf->init();
    root_leaf->is_leaf = true;

    struct root_struct curr_root = {root_page->page_id};

    // -------- INSERT --------
    for (int i = 0; i < 4; i++) {
        insert(buff_pool, access_methods,
               curr_root.root_pid, // IMPORTANT
               i, samp_rid, &curr_root);
    }

    // -------- FETCH CURRENT ROOT --------
    buffer_manager_types::Page *new_root_page = buff_pool.page_access(curr_root.root_pid, diskoperator_types::INDEX_PAGE);

    btree_page_types::Node *new_root = reinterpret_cast<btree_page_types::Node *>(new_root_page->page_data);

    // -------- ASSERT ROOT STRUCTURE --------
    assert(new_root->is_leaf == false);
    std::cout << "assert is_leaf false passed\n";

    assert(new_root->key_count == 1);
    std::cout << "assert key_count == 1 passed\n";

    assert(new_root->keys[0] == 2);
    std::cout << "assert promoted key == 2 passed\n";

    // -------- ASSERT CHILDREN EXIST --------
    heap_page_types::page_id left_pid = new_root->data.internal_node.child_nodes[0];

    heap_page_types::page_id right_pid = new_root->data.internal_node.child_nodes[1];

    assert(left_pid != right_pid);
    std::cout << "assert two distinct children passed\n";

    // -------- VERIFY LEAF CONTENTS --------
    buffer_manager_types::Page *left_page = buff_pool.page_access(left_pid, diskoperator_types::INDEX_PAGE);

    buffer_manager_types::Page *right_page = buff_pool.page_access(right_pid, diskoperator_types::INDEX_PAGE);

    btree_page_types::Node *left_leaf = reinterpret_cast<btree_page_types::Node *>(left_page->page_data);

    btree_page_types::Node *right_leaf = reinterpret_cast<btree_page_types::Node *>(right_page->page_data);

    assert(left_leaf->is_leaf);
    assert(right_leaf->is_leaf);

    assert(left_leaf->key_count == 2);
    assert(right_leaf->key_count == 2);

    assert(left_leaf->keys[0] == 0);
    assert(left_leaf->keys[1] == 1);

    assert(right_leaf->keys[0] == 2);
    assert(right_leaf->keys[1] == 3);

    std::cout << "leaf distribution correct\n";
}
