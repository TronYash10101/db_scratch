#ifndef WRITER
#define WRITER

#include "access_methods.hpp"
#include <cstring>
#include <stdexcept>

void write_page(char *raw_buffer, const access_methods::row_t &row);
#endif
