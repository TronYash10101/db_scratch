#include "../headers/schmea_manager.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char **argv) {
    std::filesystem::path filepath = "/home/yash-jadhav/db_scratch/catalog_manager/schema_file.bin";

    int mode = std::stoi(argv[1]);
    if (mode == 1) {
        std::ofstream file1(filepath, std::ios::binary | std::ios::out | std::ios::trunc);
        file1.close();
    }
    schema::schema_manager schema_manager(filepath);

    // schema_manager.iterate_catalog();

    std::optional<std::vector<schema::ENTITY_TYPE>> table_match = schema_manager.entity_find(schema::TABLE, "students", "school");

    // schema_manager.iterate_catalog();
    //
    table_match.value();
}
