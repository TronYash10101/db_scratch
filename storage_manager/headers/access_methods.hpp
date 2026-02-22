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

class Access_methods {
  private:
    struct leaf_split_res {
        int promoted_key;
        btree_page_types::node_id new_pid;
    };
    struct internal_split_res {
        int promoted_key;
        btree_page_types::node_id new_pid;
    };
    std::vector<heap_page_types::page_id> HeapTable;

  public:
    explicit Access_methods();

    // Only scans the heap page for matching tuple, if found return an iterator
    // to it else NULL (Don't try to mix data storing logic here, assume correct
    // is present)
    std::optional<heap_page_types::RID> heap_scan(buffer_manager::buffer_pool &buff_pool, access_methods_types::SARG sarg);

    void heap_table_push(heap_page_types::page_id pid);

    void bptree_leaf_insert(char *raw_index_page, char *new_index_page, btree_page_types::node_id pid, int key,
                            btree_page_types::node_id new_pid, heap_page_types::RID rid);

    /* Caller should use the value returned by this function to create a parent node pointing to children */
    access_methods::Access_methods::leaf_split_res bptree_leaf_split(char *old_raw_index_page, char *new_raw_index_page,
                                                                     btree_page_types::node_id new_leaf_pid, int *temp_keys,
                                                                     heap_page_types::RID *temp_rids);

    void bptree_internal_insert(char *raw_index_page, char *new_index_page, btree_page_types::node_id pid,
                                btree_page_types::node_id child_pid, btree_page_types::node_id new_pid, int key);

    access_methods::Access_methods::internal_split_res bptree_internal_split(char *old_raw_index_page, char *new_raw_index_page,
                                                                             btree_page_types::node_id new_internal_pid, int *temp_keys,
                                                                             btree_page_types::node_id *temp_child_id);
    void index_scan();
};
} // namespace access_methods
#endif
