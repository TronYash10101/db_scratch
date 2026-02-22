#ifndef TYPES
#define TYPES

#include <assert.h>
#include <cstring>
#include <iostream>
#include <stdint.h>
#include <vector>

// namespace diskoperator_types
namespace diskoperator_types {

enum page_type { HEAP_PAGE, INDEX_PAGE };

}

// namespace buffer_manager_types
namespace buffer_manager_types {

constexpr std::size_t buffer_size = 16;
constexpr std::size_t MAX_HEAP_PAGES = 32;
constexpr std::size_t MAX_INDEX_PAGES = 32;
typedef size_t frame_id;
constexpr frame_id INVALID_FRAME = static_cast<frame_id>(-1);
constexpr frame_id INVALID_PAGE_ID = static_cast<frame_id>(-1);
constexpr std::size_t page_data_size = 4096;

struct Page {
    char page_data[page_data_size];
    size_t page_id = INVALID_PAGE_ID; // gives the max value of size_t
    bool dirty_bit = false;
    int pin_count = 0;
    diskoperator_types::page_type type;
};

} // namespace buffer_manager_types

// namespace heap_page_types
namespace heap_page_types {

typedef size_t page_id;

constexpr int MAX_SLOTS = 10;

#pragma pack(push, 1)
struct PageHeader {
    int free_size = buffer_manager_types::page_data_size;
    int slot_count = 0;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct Slot {
    int slot_size;
    uint16_t slot_offset;
    bool deleted = false;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct HeapPage {
    PageHeader page_header;
    Slot slots[MAX_SLOTS];
    char data[buffer_manager_types::page_data_size];

    // Just because of raw pointer operation
    void initialize() {
        page_header.free_size = buffer_manager_types::page_data_size;
        page_header.slot_count = 0;
    }
};
#pragma pack(pop)
typedef struct {
    page_id pid;
    Slot slot;
} RID;

} // namespace heap_page_types

// namespace btree_page_types
namespace btree_page_types {
constexpr int MAX_KEYS = 3;
typedef size_t node_id;

#pragma pack(push, 1)
struct Node {

  private:
    struct Leaf_Node {
        heap_page_types::RID values[MAX_KEYS];
        node_id next_leaf = buffer_manager_types::INVALID_PAGE_ID;
    };
    struct Internal_Node {
        node_id child_nodes[MAX_KEYS + 1];
    };

  public:
    node_id pid = buffer_manager_types::INVALID_PAGE_ID;
    bool is_leaf = true;
    int key_count = 0;
    int keys[MAX_KEYS];
    union u_data {
        Leaf_Node leaf_node;
        Internal_Node internal_node;

        // Defines how data is initiated when some particular struct is used
        u_data(Leaf_Node ln) : leaf_node(ln){};
        u_data(Internal_Node in) : internal_node(in){};
        u_data() : leaf_node(){};
    } data;
    void init() {
        this->key_count = 0;
        this->is_leaf = true;
    }
    // overall Node constructor based on union type
    Node(bool leaf = true) : is_leaf(leaf), key_count(0), data(){};
};
#pragma pack(pop)

} // namespace btree_page_types

// namespace access_methods_types
namespace access_methods_types {
// Stored in heap page
struct row_t {
    // int primary_key;
    int x = 0;
    int y = 0;
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
} // namespace access_methods_types
#endif
