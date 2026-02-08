#include "headers/disk_operator.hpp"
#include <cstdio>

// create a db file(of n*PAGE_SIZE(4kb)), read a page based on pid, write to
// that page

// as this may involve copy/move hence implementing class

// definition in header file to avoid "Incomplete type" error
