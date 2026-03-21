#include "../headers/schmea_manager.hpp"
#include <filesystem>
#include <iostream>
#include <vector>

int main() {
    std::filesystem::path filepath = "/home/yash-jadhav/db_scratch/catalog_manager/schema_file.bin";

    schema::schema_manager sch_manager(filepath);

    parser_types::SCHEMA_AST schema;
    schema.schmea_name = "s1";

    sch_manager.create_schema(schema);

    parser_types::CREATE_TABLE_AST table;
    table.table_name = "test";

    sch_manager.schema_create_table("s1", table);

    // sch_manager.iterate_catalog();
}
