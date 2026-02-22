#include "headers/access_methods.hpp"
#include "headers/types.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <vector>

access_methods::Access_methods::Access_methods() : HeapTable() {}

std::optional<heap_page_types::RID> access_methods::Access_methods::heap_scan(buffer_manager::buffer_pool &buff_pool,
                                                                              access_methods_types::SARG sarg) {

    for (auto it = HeapTable.begin(); it != HeapTable.end(); ++it) {
        heap_page_types::page_id pid = *it;
        char *heap_page_data = buff_pool.page_access(pid, diskoperator_types::HEAP_PAGE)->page_data;
        heap_page_types::HeapPage *heap_page = reinterpret_cast<heap_page_types::HeapPage *>(heap_page_data);

        // iterate each tuple and if tuple satisfies SARG return
        for (int slot = 0; slot < heap_page->page_header.slot_count; slot++) {

            if (heap_page->slots[slot].deleted) {
                continue;
            }

            uint16_t size = heap_page->slots[slot].slot_size;
            uint16_t offset = heap_page->slots[slot].slot_offset;
            // read
            access_methods_types::row_t *match_to_row = reinterpret_cast<access_methods_types::row_t *>(heap_page->data + offset);
            if (sarg.match(*match_to_row)) {
                heap_page_types::RID row_id;
                row_id.pid = pid;
                row_id.slot = heap_page->slots[slot];
                buff_pool.un_pin(pid, diskoperator_types::HEAP_PAGE);
                return row_id;
            } else {
                std::cout << "No rid ";
                continue;
            };
        }
        buff_pool.un_pin(pid, diskoperator_types::HEAP_PAGE);
    }
    return std::nullopt;
}

void access_methods::Access_methods::heap_table_push(heap_page_types::page_id pid) { HeapTable.push_back(pid); }

void access_methods::Access_methods::bptree_leaf_insert(char *raw_index_page, char *new_index_page, heap_page_types::page_id pid, int key,
                                                        btree_page_types::node_id new_pid, heap_page_types::RID rid) {

    btree_page_types::Node *index_page = reinterpret_cast<btree_page_types::Node *>(raw_index_page);
    if (index_page->key_count < btree_page_types::MAX_KEYS) {
        if (index_page->key_count == 0) {
            index_page->key_count += 1;
            index_page->keys[index_page->key_count - 1] = key;
            if (index_page->pid == buffer_manager_types::INVALID_PAGE_ID) {
                index_page->pid = pid;
            }
        } else if (index_page->key_count != 0) {
            index_page->key_count += 1;
            int i = index_page->key_count - 2;

            while (i >= 0 && index_page->keys[i] > key) {
                index_page->keys[i + 1] = index_page->keys[i];
                index_page->data.leaf_node.values[i + 1] = index_page->data.leaf_node.values[i];
                i--;
            }

            index_page->keys[i + 1] = key;
            index_page->data.leaf_node.values[i + 1] = rid;
        }

    } else if (index_page->key_count >= btree_page_types::MAX_KEYS) {
        int temp_keys[btree_page_types::MAX_KEYS + 1];
        heap_page_types::RID temp_rids[btree_page_types::MAX_KEYS + 1];

        memcpy(temp_keys, index_page->keys, sizeof(int) * (index_page->key_count));
        memcpy(temp_rids, index_page->data.leaf_node.values, sizeof(heap_page_types::RID) * (index_page->key_count));

        int i = index_page->key_count - 1; // index review

        while (i >= 0 && temp_keys[i] > key) {
            temp_keys[i + 1] = temp_keys[i];
            temp_rids[i + 1] = temp_rids[i];
            i--;
        }
        temp_keys[i + 1] = key;
        temp_rids[i + 1] = rid;
        bptree_leaf_split(raw_index_page, new_index_page, new_pid, temp_keys, temp_rids);
    }
}

void access_methods::Access_methods::bptree_internal_insert(char *raw_index_page, char *new_index_page, btree_page_types::node_id pid,
                                                            heap_page_types::page_id new_pid, btree_page_types::node_id child_pid,
                                                            int key) {
    btree_page_types::Node *internal_node = reinterpret_cast<btree_page_types::Node *>(raw_index_page);
    if (internal_node->key_count < btree_page_types::MAX_KEYS) {
        if (internal_node->key_count == 0) {
            internal_node->key_count += 1;
            internal_node->is_leaf = false;
            internal_node->keys[internal_node->key_count - 1] = key;
            if (internal_node->pid == buffer_manager_types::INVALID_PAGE_ID) {
                internal_node->pid = pid;
            }
        } else if (internal_node->key_count != 0) {
            internal_node->key_count += 1;
            int i = internal_node->key_count - 2;

            while (i >= 0 && internal_node->keys[i] > key) {
                internal_node->keys[i + 1] = internal_node->keys[i];
                internal_node->data.internal_node.child_nodes[i + 2] = internal_node->data.internal_node.child_nodes[i + 1];
                i--;
            }

            internal_node->keys[i + 1] = key;
            internal_node->data.internal_node.child_nodes[i + 2] = child_pid;
        }
    } else {
        int temp_keys[btree_page_types::MAX_KEYS + 1];
        btree_page_types::node_id temp_child_id[btree_page_types::MAX_KEYS + 2];

        memcpy(temp_keys, internal_node->keys, sizeof(int) * (internal_node->key_count));
        memcpy(temp_child_id, internal_node->data.internal_node.child_nodes,
               sizeof(btree_page_types::node_id) * (internal_node->key_count + 1));

        int i = internal_node->key_count - 1;

        while (i >= 0 && temp_keys[i] > key) {
            temp_keys[i + 1] = temp_keys[i];
            temp_child_id[i + 2] = temp_child_id[i + 1];
            i--;
        }
        temp_keys[i + 1] = key;
        temp_child_id[i + 2] = child_pid;
        bptree_internal_split(raw_index_page, new_index_page, new_pid, temp_keys, temp_child_id);
    }
};

access_methods::Access_methods::internal_split_res
access_methods::Access_methods::bptree_internal_split(char *old_raw_index_page, char *new_raw_index_page,
                                                      heap_page_types::page_id new_internal_pid, int *temp_keys,
                                                      heap_page_types::page_id *temp_child_id) {

    btree_page_types::Node *old_internal_node = reinterpret_cast<btree_page_types::Node *>(old_raw_index_page);
    btree_page_types::Node *new_internal_node = reinterpret_cast<btree_page_types::Node *>(new_raw_index_page);

    new_internal_node->is_leaf = false;
    new_internal_node->pid = new_internal_pid;

    memset(old_internal_node->keys, 0, sizeof(int) * btree_page_types::MAX_KEYS);
    memset(old_internal_node->data.internal_node.child_nodes, 0, sizeof(btree_page_types::node_id) * (btree_page_types::MAX_KEYS + 1));

    int total = old_internal_node->key_count + 1;
    int split_idx = (total) / 2;
    int left_size = split_idx;
    int right_size = total - left_size - 1;

    memcpy(old_internal_node->keys, temp_keys, sizeof(int) * left_size);
    memcpy(new_internal_node->keys, temp_keys + left_size, sizeof(int) * right_size);
    memcpy(old_internal_node->data.internal_node.child_nodes, temp_child_id, sizeof(btree_page_types::node_id) * (left_size + 1));
    memcpy(new_internal_node->data.internal_node.child_nodes, temp_child_id + left_size + 1,
           sizeof(btree_page_types::node_id) * (right_size + 1));

    old_internal_node->key_count = left_size;
    new_internal_node->key_count = right_size;

    struct internal_split_res res = {temp_keys[split_idx], new_internal_pid};
    return res;
}

access_methods::Access_methods::leaf_split_res
access_methods::Access_methods::bptree_leaf_split(char *old_raw_index_page, char *new_raw_index_page, heap_page_types::page_id new_leaf_pid,
                                                  int *temp_keys, heap_page_types::RID *temp_rids) {

    // As internal node and new leaf are created in this process, new frame should be brought into index_frame and managed here, also
    // writing to file should be done

    btree_page_types::Node *old_leaf_page = reinterpret_cast<btree_page_types::Node *>(old_raw_index_page);
    btree_page_types::Node *new_leaf_page = reinterpret_cast<btree_page_types::Node *>(new_raw_index_page);

    new_leaf_page->is_leaf = true;
    new_leaf_page->pid = new_leaf_pid;

    memset(old_leaf_page->keys, 0, sizeof(int) * btree_page_types::MAX_KEYS);
    memset(old_leaf_page->data.leaf_node.values, 0, sizeof(heap_page_types::RID) * btree_page_types::MAX_KEYS);

    int total = old_leaf_page->key_count + 1;
    int split_idx = (total + 1) / 2;
    int left_size = split_idx;
    int right_size = total - left_size;

    memcpy(old_leaf_page->keys, temp_keys, sizeof(int) * left_size);
    memcpy(new_leaf_page->keys, temp_keys + left_size, sizeof(int) * right_size);
    memcpy(old_leaf_page->data.leaf_node.values, temp_rids, sizeof(heap_page_types::RID) * left_size);
    memcpy(new_leaf_page->data.leaf_node.values, temp_rids + left_size, sizeof(heap_page_types::RID) * right_size);

    new_leaf_page->data.leaf_node.next_leaf = old_leaf_page->data.leaf_node.next_leaf;
    old_leaf_page->data.leaf_node.next_leaf = new_leaf_page->pid;

    old_leaf_page->key_count = left_size;
    new_leaf_page->key_count = right_size;

    struct leaf_split_res res = {new_leaf_page->keys[0], new_leaf_pid};
    return res;
}
