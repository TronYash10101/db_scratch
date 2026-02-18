#ifndef ACCESS_METHODS
#define ACCESS_METHODS

#include "buffer_manager.hpp"
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <vector>

namespace access_methods {

constexpr int node_size = 4096; // bytes
constexpr int MAX_KEYS = (node_size / 2) - 1;
constexpr int CHILD_NODES = (node_size / 2);

// Stored in heap page
struct row_t {
    // int primary_key;
    int x = 0;
    int y = 0;
};

typedef struct {
    buffer_manager::page_id pid;
    buffer_manager::Slot slot;
} RID;

struct internal_node_page {
    int key_count = 0;
    int keys[MAX_KEYS];
    buffer_manager::Page child_node[MAX_KEYS + 1];
};
struct leaf_node {
    int keys[MAX_KEYS];
    row_t rids[MAX_KEYS];
    buffer_manager::Page adjecent_node_id;
};
typedef enum { X, Y } col_type;
typedef enum { EQ, GT, LS } op_type;

struct SARG {
    col_type col;
    op_type op;
    int constant;

    bool match(row_t &match_to_row) const {
        int value = (col == X) ? match_to_row.x : match_to_row.y;
        switch (op) {
        case EQ:
            return (value == constant);
        case GT:
            return (value > constant);
        case LS:
            return (value < constant);
        default:
            return false;
        }
    };
};

class Access_methods {
  private:
    std::vector<buffer_manager::page_id> HeapTable;

  public:
    explicit Access_methods();

    // Only scans the heap page for matching tuple, if found return an iterator
    // to it else NULL (Don't try to mix data storing logic here, assume correct
    // is present)
    std::optional<RID> heap_scan(buffer_manager::buffer_pool &buff_pool, SARG sarg);

    void heap_table_push(buffer_manager::page_id pid);

    void init_btree();
    void index_scan();

    // Pair this with heap_writer
};

} // namespace access_methods

#endif
