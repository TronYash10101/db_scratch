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

void access_methods::Access_methods::bptree_insert(char *raw_index_page, btree_page_types::node_id pid, int key, heap_page_types::RID rid) {

    btree_page_types::Node *index_page = reinterpret_cast<btree_page_types::Node *>(raw_index_page);
    // int data_shift_idx = 0;
    if (index_page->key_count < btree_page_types::MAX_KEYS) {
        if (index_page->key_count == 0) {
            index_page->key_count += 1;
            index_page->keys[index_page->key_count - 1] = key;
        } else if (index_page->key_count != 0) {
            index_page->key_count += 1;
            index_page->keys[index_page->key_count - 1] = key;
            int j = index_page->key_count - 1;

            while (j > 0 && key < index_page->keys[j - 1]) {
                // ! can shift right too
                int temp_key = index_page->keys[j - 1];
                heap_page_types::RID temp_value = index_page->data.leaf_node.values[j - 1];
                index_page->keys[j - 1] = index_page->keys[j];
                index_page->keys[j] = temp_key;
                index_page->data.leaf_node.values[j - 1] = rid;
                index_page->data.leaf_node.values[j] = temp_value;
                j--;
            }
            // index_page->keys[j] = key;
            // index_page->data.leaf_node.values[j] = rid;
        }

        if (index_page->pid == buffer_manager_types::INVALID_PAGE_ID) {
            index_page->pid = pid;
        }

    } else if (index_page->key_count >= btree_page_types::MAX_KEYS) {
        // bptree_split();
        // bptree_insert(btree_page_types::node_id pid, int key, heap_page_types::RID rid);
    }
}
void access_methods::Access_methods::bptree_split(char *old_raw_index_page, char *new_raw_index_page, btree_page_types::node_id pid,
                                                  int new_key, heap_page_types::RID new_rid) {

    // As internal node and new leaf are created in this process, new frame should be brought into index_frame and managed here, also
    // writing to file should be done

    btree_page_types::Node *old_index_page = reinterpret_cast<btree_page_types::Node *>(old_raw_index_page);
    old_index_page->key_count += 1;
    old_index_page->keys[old_index_page->key_count - 1] = new_key;
    if (old_index_page->pid == buffer_manager_types::INVALID_PAGE_ID) {
        old_index_page->pid = pid;
    }
    if (old_index_page->is_leaf) {
        old_index_page->data.leaf_node.values[old_index_page->key_count - 1] = new_rid;
    }

    btree_page_types::Node *new_index_page = reinterpret_cast<btree_page_types::Node *>(new_raw_index_page);
    int total = old_index_page->key_count;
    int split_idx = (total + 1) / 2;
    int left_size = split_idx;
    int right_size = total - left_size;

    // update new leaf
    new_index_page->pid = pid;
    new_index_page->key_count = right_size;
    memcpy(new_index_page->keys, old_index_page->keys + left_size, sizeof(int) * right_size);
    memcpy(new_index_page->data.leaf_node.values, old_index_page->data.leaf_node.values + left_size,
           sizeof(heap_page_types::RID) * right_size);

    //  upadate previous leaf
    new_index_page->data.leaf_node.next_leaf = old_index_page->data.leaf_node.next_leaf; // revise this part
    old_index_page->data.leaf_node.next_leaf = new_index_page->pid;

    old_index_page->key_count = left_size;
    memset(old_index_page->keys + left_size, 0, sizeof(int) * right_size);
    memset(old_index_page->data.leaf_node.values + left_size, 0, sizeof(heap_page_types::RID) * right_size);

    // internal node work
}
