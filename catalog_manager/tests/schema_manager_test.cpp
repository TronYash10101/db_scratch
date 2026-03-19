#include "../headers/schmea_manager.hpp"
#include <filesystem>
#include <iostream>

int main() {
    std::filesystem::path filepath = "/home/yash-jadhav/db_scratch/catalog_manager/schema_file.bin";
    schema::schema_manager sch_manager(filepath);
}
