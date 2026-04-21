#include "../headers/schmea_manager.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char **argv) {
    std::filesystem::path filepath = "/home/yash-jadhav/db_scratch/schema_file.bin";

    int mode = std::stoi(argv[1]);
    if (mode == 1) {
        std::ofstream file1(filepath, std::ios::binary | std::ios::out | std::ios::trunc);
        file1.close();
    }
    schema::schema_manager schema_manager(filepath);

    parser_types::SCHEMA_AST sast;
    sast.schmea_name = "abc";

    parser_types::SCHEMA_AST sast2;
    sast2.schmea_name = "pqr";

    schema_manager.create_schema(sast);
    schema_manager.create_schema(sast2);

    parser_types::CREATE_TABLE_AST cast;
    cast.table_name = "t1";

    parser_types::CREATE_TABLE_AST cast2;
    cast2.table_name = "t2";
    parser_types::CREATE_TABLE_AST cast3;
    cast3.table_name = "t3";

    schema_manager.schema_create_table("abc", cast);
    schema_manager.schema_create_table("pqr", cast2);
    schema_manager.schema_create_table("pqr", cast3);

    schema_manager.iterate_catalog();
}
