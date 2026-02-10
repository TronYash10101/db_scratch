#ifndef ACCESS_METHODS
#define ACCESS_METHODS

#include "buffer_manager.hpp"
#include <cstdint>
#include <cstdio>
#include <iostream>
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

  bool match(const row_t &match_to_row) const {
    int value = (col == X) ? match_to_row.x : match_to_row.y;
    switch (op) {
    case EQ:
      return (value == constant);
    case GT:
      return (value >= constant);
    case LS:
      return (value <= constant);
    }
  };
};
struct PageHeader {
  uint8_t slot_count = 0;
};

struct Slot {
  // uint8_t primary_key;
  std::size_t slot_size;
  uint16_t slot_offset;
};

struct HeapPage {
  PageHeader page_header;
  Slot slots[MAX_SLOTS];
  char data[page_data_size];
};

class HeapTable {
private:
  std::vector<page_id> Table;

public:
  explicit HeapTable();

  // Only scans the heap page for matching tuple, if found return an iterator
  // to it else NULL (Don't try to mix data storing logic here, assume correct
  // is present)
  row_t *heap_scan(buffer_manager::buffer_pool &buff_pool, SARG sarg);
};

} // namespace access_methods

#endif
