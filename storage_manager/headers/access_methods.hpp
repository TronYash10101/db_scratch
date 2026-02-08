#ifndef ACCESS_METHODS
#define ACCESS_METHODS

#include "buffer_manager.hpp"
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <vector>

namespace access_methods {
constexpr int MAX_SLOTS = 10;

typedef struct {
  int x;
  int y;
} row_t;

typedef enum { X, Y } col_type;
typedef enum { EQ, GT, LS } op_type;

struct SARG {
  col_type col;
  op_type op;
  char constant;

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
typedef struct {
  uint8_t h_offset; // free space offset
  uint8_t slot_count;
} PageHeader;

typedef struct {
  uint8_t primary_key;
  uint16_t slot_size;
  uint16_t slot_offset;
} Slot;

struct HeapPage {
  PageHeader page_header;
  Slot slots[MAX_SLOTS];
  char data[page_size];
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
