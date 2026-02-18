#ifndef WRITER
#define WRITER

#include "access_methods.hpp"
#include <cstring>
#include <stdexcept>

void heap_write(char *raw_buffer, const access_methods::row_t &row);
void delete_slot(buffer_manager::buffer_pool &buff_pool, access_methods::RID rid);
#endif
