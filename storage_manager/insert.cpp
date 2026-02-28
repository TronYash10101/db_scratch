#include "headers/insert.hpp"
#include "headers/heap_writer.hpp"
#include "headers/types.hpp"

void create_entry(buffer_manager::buffer_pool &buff_pool, access_methods::Access_methods &access_methods) {
    uintmax_t last_heap_pid = buff_pool.get_last_pid(diskoperator_types::HEAP_PAGE);

    buffer_manager_types::Page *raw_heap_page = buff_pool.page_access(last_heap_pid, diskoperator_types::HEAP_PAGE);
    heap_page_types::HeapPage *heap_page = reinterpret_cast<heap_page_types::HeapPage *>(raw_heap_page->page_data);
}
