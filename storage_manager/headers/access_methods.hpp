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
