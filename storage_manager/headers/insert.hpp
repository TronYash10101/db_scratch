#ifndef INSERT
#define INSERT

#include "heap_writer.hpp"
#include "index_writer.hpp"

namespace insert {
heap_page_types::page_id create_entry(buffer_manager::buffer_pool &buff_pool, access_methods::Access_methods &access_methods,
                                      const access_methods_types::row_t &row, index_write::root_struct &curr_root);
}

#endif
