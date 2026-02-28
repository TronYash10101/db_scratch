#ifndef INDEX_WRITER
#define INDEX_WRITER

#include "access_methods.hpp"
#include "types.hpp"
#include <iostream>

namespace index_write {

struct root_struct {
    heap_page_types::page_id root_pid;
};
buffer_manager_types::Page *start(buffer_manager::buffer_pool &buff_pool, buffer_manager_types::Page *root_page, root_struct curr_root);
buffer_manager_types::Page *fetch_page(buffer_manager::buffer_pool &buff_pool);
std::optional<access_methods::split_res> index_insert(buffer_manager::buffer_pool &buff_pool,
                                                      access_methods::Access_methods &access_methods, heap_page_types::page_id curr_pid,
                                                      int key, heap_page_types::RID rid, root_struct *curr_root);

} // namespace index_write

#endif
