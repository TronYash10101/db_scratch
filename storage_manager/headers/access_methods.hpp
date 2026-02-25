#ifndef ACCESS_METHODS
#define ACCESS_METHODS

#include "buffer_manager.hpp"
#include "types.hpp"
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <vector>
namespace access_methods {
struct leaf_split_res {
    int promoted_key;
    btree_page_types::node_id new_pid;
};
struct internal_split_res {
    int promoted_key;
    btree_page_types::node_id new_pid;
};
class Access_methods {
  private:
    std::vector<heap_page_types::page_id> HeapTable;

  public:
    explicit Access_methods();

    // Only scans the heap page for matching tuple, if found return an iterator
    // to it else NULL (Don't try to mix data storing logic here, assume correct
    // is present)
    std::optional<heap_page_types::RID> heap_scan(buffer_manager::buffer_pool &buff_pool, access_methods_types::SARG sarg);

    void heap_table_push(heap_page_types::page_id pid);

    /* Pages can be NULL */
    std::optional<access_methods::leaf_split_res> bptree_leaf_insert(buffer_manager_types::Page *left_raw_page,
                                                                     buffer_manager_types::Page *right_raw_page,
                                                                     heap_page_types::page_id left_pid, btree_page_types::node_id right_pid,
                                                                     int key, heap_page_types::RID rid);

    std::optional<access_methods::internal_split_res> bptree_internal_insert(buffer_manager_types::Page *left_raw_page,
                                                                             buffer_manager_types::Page *right_raw_page,
                                                                             btree_page_types::node_id left_pid,
                                                                             heap_page_types::page_id right_pid,
                                                                             btree_page_types::node_id child_pid, int key);

    access_methods::internal_split_res bptree_internal_split(buffer_manager_types::Page *left_raw_page,
                                                             buffer_manager_types::Page *right_raw_page, heap_page_types::page_id right_pid,
                                                             int *temp_keys, heap_page_types::page_id *temp_child_id);

    access_methods::leaf_split_res bptree_leaf_split(buffer_manager_types::Page *left_raw_page, buffer_manager_types::Page *right_raw_page,
                                                     heap_page_types::page_id right_pid, int *temp_keys, heap_page_types::RID *temp_rids);

    void index_scan();
};
} // namespace access_methods
#endif
