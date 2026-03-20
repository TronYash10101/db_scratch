#include "../headers/schmea_manager.hpp"
#include <filesystem>
#include <iostream>
#include <vector>

int main() {
    std::filesystem::path filepath = "/home/yash-jadhav/db_scratch/catalog_manager/schema_file.bin";

    schema::schema_manager sch_manager(filepath);

    parser_types::SCHEMA_AST s1;
    parser_types::SCHEMA_AST s2;

    s1.schmea_name = "abc";
    s2.schmea_name = "xyz";

    sch_manager.create_schema(s1);
    sch_manager.create_schema(s2);

    parser_types::CREATE_TABLE_AST st1;
    parser_types::CREATE_TABLE_AST st2;
    parser_types::CREATE_TABLE_AST st3;

    st1.table_name = "t1";
    st2.table_name = "t2";
    st3.table_name = "t3";
    st3.columns.push_back({"AGE", parser_types::INTEGER});

    sch_manager.schema_create_table("abc", st1);
    sch_manager.schema_create_table("xyz", st2);
    sch_manager.schema_create_table("xyz", st3);

    sch_manager.iterate_catalog();
}
