#include "headers/index_writer.hpp"

buffer_manager_types::Page *index_write::start(buffer_manager::buffer_pool &buff_pool, buffer_manager_types::Page *root_page,
                                               index_write::root_struct curr_root) {
    heap_page_types::page_id root_pid = curr_root.root_pid;

    root_page = buff_pool.page_access(root_pid, diskoperator_types::INDEX_PAGE);

    return root_page;
}
buffer_manager_types::Page *index_write::fetch_page(buffer_manager::buffer_pool &buff_pool) {

    /* Fetches init INDEX page */

    uintmax_t last_pid = buff_pool.get_last_pid(diskoperator_types::INDEX_PAGE);
    buffer_manager_types::Page *p1 = buff_pool.page_access(last_pid, diskoperator_types::INDEX_PAGE);

    btree_page_types::Node *p1n = reinterpret_cast<btree_page_types::Node *>(p1);
    p1n->init();
    buff_pool.dp_write_page(p1, diskoperator_types::INDEX_PAGE);
    return p1;
}

std::optional<access_methods::split_res> index_write::index_insert(buffer_manager::buffer_pool &buff_pool,
                                                                   access_methods::Access_methods &access_methods,
                                                                   heap_page_types::page_id curr_pid, int key, heap_page_types::RID rid,
                                                                   index_write::root_struct *curr_root) {

    buffer_manager_types::Page *curr_page = buff_pool.page_access(curr_pid, diskoperator_types::INDEX_PAGE);
    btree_page_types::Node *node = reinterpret_cast<btree_page_types::Node *>(curr_page->page_data);
    heap_page_types::page_id next_pid;
    std::optional<access_methods::split_res> res;
    std::optional<access_methods::split_res> child_node_ans;
    if (node->is_leaf) {

        // buffer_manager_types::Page *leaf_page = buff_pool.page_access(curr_pid, diskoperator_types::INDEX_PAGE);
        buffer_manager_types::Page *new_page = NULL;
        btree_page_types::Node *new_page_node = NULL;
        if (node->key_count == btree_page_types::MAX_KEYS) {
            new_page = fetch_page(buff_pool);
            new_page_node = reinterpret_cast<btree_page_types::Node *>(new_page->page_data);
            new_page_node->init();
            new_page->dirty_bit = true;
        }
        res = access_methods.bptree_leaf_insert(curr_page, new_page, key, rid);
        curr_page->dirty_bit = true;

    } else if (node->is_leaf == false) {

        int i = 0;
        while (i < node->key_count && key > node->keys[i]) {
            i++;
        }

        next_pid = node->data.internal_node.child_nodes[i];

        child_node_ans = index_write::index_insert(buff_pool, access_methods, next_pid, key, rid, curr_root);

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
        // std::cout << curr_page->page_id << " " << res->child_pid;
    } else if (res.has_value() && curr_pid != curr_root->root_pid) {
        return res;
    }

    return std::nullopt;
}
