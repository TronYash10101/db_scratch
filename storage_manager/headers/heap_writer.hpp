#ifndef WRITER
#define WRITER

#include "access_methods.hpp"
#include "types.hpp"
#include <cstring>
#include <stdexcept>
namespace heap_writer {

heap_page_types::Slot heap_write(char *raw_heap_page, const access_methods_types::row_t &row);
void delete_slot(buffer_manager::buffer_pool &buff_pool, heap_page_types::RID rid);

} // namespace heap_writer
#endif
