#include "../storage_manager/headers/access_methods.hpp"
#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <random>
#include <stdexcept>
#include <vector>

struct root_struct {
    heap_page_types::page_id root_pid;
};

void check(bool condition, const std::string &message) {
    if (!condition) {
        throw std::runtime_error("Validation Failed: " + message);
    }
}

buffer_manager_types::Page *fetch_page(buffer_manager::buffer_pool &buff_pool) {

    /* In order for this function to work pages should be actually written to disk and not changed in memory */

    uintmax_t last_pid = buff_pool.get_last_pid(diskoperator_types::INDEX_PAGE);
    buffer_manager_types::Page *p1 = buff_pool.page_access(last_pid, diskoperator_types::INDEX_PAGE);

    btree_page_types::Node *p1n = reinterpret_cast<btree_page_types::Node *>(p1);
    p1n->init();
    buff_pool.dp_write_page(p1, diskoperator_types::INDEX_PAGE);
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
        buffer_manager_types::Page *new_page = NULL;
        btree_page_types::Node *new_page_node = NULL;
        if (node->key_count == btree_page_types::MAX_KEYS) {
            new_page = fetch_page(buff_pool);
            new_page_node = reinterpret_cast<btree_page_types::Node *>(new_page->page_data);
            new_page_node->init();
            new_page->dirty_bit = true;
        }
        res = access_methods.bptree_leaf_insert(leaf_page, new_page, key, rid);
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
            res = access_methods.bptree_internal_insert(curr_page, right_new_page, child_node_ans->child_pid, child_node_ans->promoted_key);
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
        curr_root->root_pid = new_root_page->page_id;

        new_root_node->data.internal_node.child_nodes[0] = curr_page->page_id;
        new_root_node->data.internal_node.child_nodes[1] = res->child_pid;
        std::cout << curr_page->page_id << " " << res->child_pid;
    } else if (res.has_value() && curr_pid != curr_root->root_pid) {
        return res;
    }

    return std::nullopt;
}
void test_sequential_small(buffer_manager::buffer_pool &buff_pool, access_methods::Access_methods &access_methods,
                           buffer_manager_types::Page *root_page, btree_page_types::Node *root_leaf, root_struct &curr_root) {

    heap_page_types::RID samp_rid = {0, 0};

    // Insert 0..7
    for (int i = 0; i < 8; i++) {
        insert(buff_pool, access_methods, curr_root.root_pid, i, samp_rid, &curr_root);
    }

    // Fetch current root
    buffer_manager_types::Page *rootp = buff_pool.page_access(curr_root.root_pid, diskoperator_types::INDEX_PAGE);

    btree_page_types::Node *root = reinterpret_cast<btree_page_types::Node *>(rootp->page_data);

    check(root->is_leaf == false, "Root should be internal after 0..7");
    check(root->key_count >= 1, "Root should contain at least one key");

    // Left child
    buffer_manager_types::Page *left_page = buff_pool.page_access(root->data.internal_node.child_nodes[0], diskoperator_types::INDEX_PAGE);

    btree_page_types::Node *left_node = reinterpret_cast<btree_page_types::Node *>(left_page->page_data);

    check(left_node->is_leaf == true, "Left child must be leaf");

    // Right child
    buffer_manager_types::Page *right_page = buff_pool.page_access(root->data.internal_node.child_nodes[1], diskoperator_types::INDEX_PAGE);

    btree_page_types::Node *right_node = reinterpret_cast<btree_page_types::Node *>(right_page->page_data);

    check(right_node->is_leaf == true, "Right child must be leaf");

    check(root->data.internal_node.child_nodes[0] != root->data.internal_node.child_nodes[1], "Children must have distinct PIDs");

    std::cout << "test_sequential_small passed\n";
}
void test_sequential_large(buffer_manager::buffer_pool &buff_pool, access_methods::Access_methods &access_methods, root_struct &curr_root) {

    heap_page_types::RID samp_rid = {0, 0};

    // Insert 0..20
    for (int i = 0; i <= 20; i++) {
        insert(buff_pool, access_methods, curr_root.root_pid, i, samp_rid, &curr_root);
    }

    // Fetch current root
    buffer_manager_types::Page *rootp = buff_pool.page_access(curr_root.root_pid, diskoperator_types::INDEX_PAGE);

    btree_page_types::Node *root = reinterpret_cast<btree_page_types::Node *>(rootp->page_data);

    check(root->is_leaf == false, "Root must be internal after inserting 0..20");

    check(root->key_count >= 1, "Root must contain at least one key");

    // Ensure children count = keys + 1
    check(root->key_count + 1 >= 2, "Root must have at least two children");

    // -------- Leaf chain verification --------

    // Go to leftmost leaf
    btree_page_types::Node *node = root;
    buffer_manager_types::Page *page = rootp;

    while (!node->is_leaf) {
        heap_page_types::page_id next = node->data.internal_node.child_nodes[0];

        page = buff_pool.page_access(next, diskoperator_types::INDEX_PAGE);

        node = reinterpret_cast<btree_page_types::Node *>(page->page_data);
    }
    buffer_manager_types::Page *pg = buff_pool.page_access(node->data.leaf_node.next_leaf, diskoperator_types::INDEX_PAGE);
    btree_page_types::Node *pgn = reinterpret_cast<btree_page_types::Node *>(pg->page_data);
    while (pgn->data.leaf_node.next_leaf != buffer_manager_types::INVALID_PAGE_ID) {
        for (int c = 0; c < btree_page_types::MAX_KEYS; c++) {
            std::cout << pgn->keys[c] << ",";
        }
        std::cout << "-->";

        pg = buff_pool.page_access(pgn->data.leaf_node.next_leaf, diskoperator_types::INDEX_PAGE);
        pgn = reinterpret_cast<btree_page_types::Node *>(pg->page_data);
    }

    /* int expected = 0;
    int count = 0;

    while (true) {

        for (int i = 0; i < node->key_count; i++) {
            check(node->keys[i] == expected, "Leaf chain order incorrect");
            expected++;
            count++;
        }

        if (node->data.leaf_node.next_leaf == buffer_manager_types::INVALID_PAGE_ID)
            break;

        page = buff_pool.page_access(node->data.leaf_node.next_leaf, diskoperator_types::INDEX_PAGE);

        node = reinterpret_cast<btree_page_types::Node *>(page->page_data);
    } */

    // check(count == 21, "Expected 21 keys after inserting 0..20");

    std::cout << "test_sequential_large passed\n";
}
int main() {

    std::filesystem::path heap_filepath = "/home/yash-jadhav/db_scratch/heap.bin";
    std::filesystem::path index_filepath = "/home/yash-jadhav/db_scratch/index.bin";

    buffer_manager::buffer_pool buff_pool(heap_filepath, index_filepath);

    access_methods::Access_methods access_methods;

    heap_page_types::RID samp_rid = {0, 0};

    buffer_manager_types::Page *root_page = fetch_page(buff_pool);

    btree_page_types::Node *root_leaf = reinterpret_cast<btree_page_types::Node *>(root_page->page_data);

    struct root_struct curr_root = {root_page->page_id};

    root_leaf->init();
    root_leaf->is_leaf = true;

    std::ofstream file(index_filepath, std::ios::binary | std::ios::out | std::ios::trunc);
    file.close();

    // test_sequential_small(buff_pool, access_methods, root_page, root_leaf, curr_root);
    test_sequential_large(buff_pool, access_methods, curr_root);
}
