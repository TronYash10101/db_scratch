#include "headers/buffer_manager.hpp"
#include "headers/disk_operator.hpp"
#include "headers/writer.hpp"
#include <cstring>
#include <iostream>
#include <stdexcept>

buffer_manager::buffer_pool::buffer_pool(const std::string &db_filename, const std::string &index_filename)
    : disk_operator(db_filename, index_filename, buffer_manager_types::page_data_size), heap_frames(buffer_manager_types::page_data_size) {
    std::cout << "\nBUFFER CREATED\n";
}

buffer_manager_types::Page *buffer_manager::buffer_pool::page_access(heap_page_types::page_id pid, diskoperator_types::page_type type) {
    if (type == diskoperator_types::HEAP_PAGE) {
        return page_access_logic(page_table, heap_frames, index_replacement_check_queue, pid, diskoperator_types::HEAP_PAGE);
    } else if (type == diskoperator_types::INDEX_PAGE) {
        return page_access_logic(index_table, index_frames, index_replacement_check_queue, pid, diskoperator_types::INDEX_PAGE);
    }
    return NULL;
}

void buffer_manager::buffer_pool::un_pin(int pid, diskoperator_types::page_type type) {
    if (type == diskoperator_types::HEAP_PAGE) {
        unpin_logic(page_table, heap_frames, pid, diskoperator_types::HEAP_PAGE);
    } else if (type == diskoperator_types::INDEX_PAGE) {
        unpin_logic(index_table, index_table, pid, diskoperator_types::INDEX_PAGE);
    }
}

buffer_manager_types::frame_id buffer_manager::buffer_pool::page_replacement_policy(diskoperator_types::page_type type) {
    if (type == diskoperator_types::HEAP_PAGE) {
        return page_replacement_policy_logic(page_table, heap_frames, diskoperator_types::HEAP_PAGE);
    } else if (type == diskoperator_types::INDEX_PAGE) {
        return page_replacement_policy_logic(index_table, index_frames, diskoperator_types::INDEX_PAGE);
    }
    throw std::runtime_error("Some error in replacing page");
};
