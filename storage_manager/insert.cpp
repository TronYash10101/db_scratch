#include "headers/insert.hpp"
#include "headers/heap_writer.hpp"
#include "headers/index_writer.hpp"
#include "headers/types.hpp"
#include <iostream>

heap_page_types::page_id insert::create_entry(buffer_manager::buffer_pool &buff_pool, access_methods::Access_methods &access_methods,
                                              const access_methods_types::row_t &row) {
    uintmax_t last_heap_pid = buff_pool.get_last_pid(diskoperator_types::HEAP_PAGE);

    buffer_manager_types::Page *raw_heap_page = buff_pool.page_access(last_heap_pid, diskoperator_types::HEAP_PAGE);

    // heap_page_types::HeapPage *heap_page = reinterpret_cast<heap_page_types::HeapPage *>(raw_heap_page->page_data);

    heap_page_types::Slot res_slot = heap_writer::heap_write(raw_heap_page->page_data, row);
    heap_page_types::RID res_rid = {raw_heap_page->page_id, res_slot};

    index_write::root_struct curr_root;
    buffer_manager_types::Page *index_root_page = index_write::fetch_page(buff_pool);
    curr_root.root_pid = index_root_page->page_id;
    index_write::index_insert(buff_pool, access_methods, curr_root.root_pid, row.x, res_rid, &curr_root);

    return curr_root.root_pid;
}
