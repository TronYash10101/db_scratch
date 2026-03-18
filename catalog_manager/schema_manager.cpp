#include "headers/schmea_manager.hpp"
#include <cstdio>
#include <cwchar>
#include <vector>

void schema::schema_manager::create_schema(std::vector<schema_attr> schmea_catalog, schema::schema_attr &schema,
                                           parser_types::SCHEMA_AST &ast) {

    schema.schema_name = ast.schmea_name;
    // schema.page_id
    // can also handle other things like permission
}

void schema::schema_manager::schema_create_table(std::vector<schema_attr> schmea_catalog, schema::schema_attr &curr_schema,
                                                 parser_types::CREATE_TABLE_AST &ast) {

    schema::tables_attrs table_attr;
    std::vector<schema::col_attrs> columns;

    for (size_t i = 0; i < ast.columns.size(); i++) {
        columns[i].type = ast.columns[i].column_type;
        columns[i].column_name = ast.columns[i].column_name;
    }

    table_attr.table_name = ast.table_name;
    table_attr.columns = columns;
    // table.heap_page_offset

    curr_schema.tables.push_back(table_attr);
}

void schema_alter_table(schema::tables_attrs &curr_table, parser_types::ALTER_TABLE_AST &ast) {}

void schema::schema_manager::init() {
    // this is serializer
}
void schema::schema_manager::write() {
    // this is deserializer
}
