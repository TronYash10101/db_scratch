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
    /* parser_types::SCHEMA_AST schema;
    schema.schmea_name = "s1";

    sch_manager.create_schema(schema);

    parser_types::CREATE_TABLE_AST table;
    parser_types::CREATE_TABLE_AST table2;
    table.table_name = "test";
    table2.table_name = "test2";

    sch_manager.schema_create_table("s1", table);
    sch_manager.schema_create_table("s1", table2); */
    std::string tn = "test";
    std::string sn = "abc";
    std::optional<std::vector<schema::ENTITY_TYPE>> column = schema_manager.entity_find(schema::COLUMN, "age", tn, sn);
    if (column.has_value()) {
        if (auto *found_col = std::get_if<schema::col_attrs>(&column.value()[0])) {
            std::cout << found_col->column_name;
        }
    } else {
        throw std::runtime_error("NO SUCH COLUMN FOUND");
    }
    schema_manager.iterate_catalog();
}
