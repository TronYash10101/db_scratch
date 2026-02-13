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
constexpr int MAX_SLOTS = 10;

struct row_t {
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
    std::cout << " match value " << value;
    switch (op) {
    case EQ:
      std::cout << " equality condition " << (value == constant) << "eq";
      std::cout << " constwx " << constant;
      return (value == constant);
    case GT:
      std::cout << " equality condition " << (value > constant) << "gt";
      return (value > constant);
    case LS:
      std::cout << " equality condition " << (value < constant) << "ls";
      return (value < constant);
    default:
      return false;
    }
  };
};
struct PageHeader {
  int free_offset = page_data_size;
  uint16_t slot_count = 0;
};

struct Slot {
  int slot_size;
  uint16_t slot_offset;
  bool deleted = false;
};

struct HeapPage {
  PageHeader page_header;
  Slot slots[MAX_SLOTS];
  char data[page_data_size];
};

typedef struct {
  page_id pid;
  Slot slot;
} RID;

class HeapTable {
private:
  std::vector<page_id> Table;

public:
  explicit HeapTable();

  // Only scans the heap page for matching tuple, if found return an iterator
  // to it else NULL (Don't try to mix data storing logic here, assume correct
  // is present)
  std::optional<RID> heap_scan(buffer_manager::buffer_pool &buff_pool,
                               SARG sarg);

  void heap_table_push(page_id pid);
  void delete_slot(buffer_manager::buffer_pool &buff_pool, RID rid);
};

} // namespace access_methods

#endif
