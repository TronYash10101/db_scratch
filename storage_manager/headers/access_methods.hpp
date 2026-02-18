#ifndef ACCESS_METHODS
#define ACCESS_METHODS

#include "buffer_manager.hpp"
#include "types.hpp"
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <vector>
namespace access_methods {

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

    void init_btree();
    void index_scan();
};

} // namespace access_methods

#endif
