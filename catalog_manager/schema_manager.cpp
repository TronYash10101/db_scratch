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

void schema::schema_manager::deserialize() {
    // this is serializer

    while (ftell(schmea_file) < schema_file_size) {

        schema_attr curr_schema;

        fread(&curr_schema.schema_name_size, sizeof(size_t), 1, schmea_file);

        curr_schema.schema_name.resize(curr_schema.schema_name_size);
        fread(curr_schema.schema_name.data(), curr_schema.schema_name_size, 1, schmea_file);

        fread(&curr_schema.tables_size, sizeof(size_t), 1, schmea_file);
        curr_schema.tables.resize(curr_schema.tables_size);

        size_t j = 0;
        while (j < curr_schema.tables_size) {
            fread(&curr_schema.tables[j].heap_page_offset, sizeof(size_t), 1, schmea_file);
            fread(&curr_schema.tables[j].page_id, sizeof(heap_page_types::page_id), 1, schmea_file);

            fread(&curr_schema.tables[j].table_name_size, sizeof(size_t), 1, schmea_file);

            curr_schema.tables[j].table_name.resize(curr_schema.tables[j].table_name_size);
            fread(curr_schema.tables[j].table_name.data(), curr_schema.tables[j].table_name_size, 1, schmea_file);
            curr_schema.tables[j].columns.resize(curr_schema.tables[j].table_name_size);

            size_t k = 0;
            while (k < curr_schema.tables[j].columns_size) {
                fread(&curr_schema.tables[j].columns[k].column_name_size, sizeof(size_t), 1, schmea_file);

                curr_schema.tables[j].columns[k].column_name.resize(curr_schema.tables[j].columns[k].column_name_size);
                fread(curr_schema.tables[j].columns[k].column_name.data(), curr_schema.tables[j].table_name_size, 1, schmea_file);

                fread(&curr_schema.tables[j].columns[k].type, sizeof(int), 1, schmea_file);

                k++;
            }
            j++;
        }
        schema_catalog.push_back(curr_schema);
    }
}

void schema::schema_manager::serialize() {
    // this is deserializer

    size_t i = 0;
    while (i < schema_catalog.size()) {
        schema_attr curr_schema = schema_catalog[i];
        curr_schema.tables_size = curr_schema.tables.size() * sizeof(tables_attrs);
        curr_schema.schema_name_size = curr_schema.schema_name.size();

        fwrite(&curr_schema.schema_name_size, sizeof(size_t), 1, schmea_file);
        fwrite(curr_schema.schema_name.data(), sizeof(char), curr_schema.schema_name_size, schmea_file);
        fwrite(&curr_schema.tables_size, sizeof(size_t), 1, schmea_file);

        int j = 0;

        while (j < curr_schema.tables.size()) {
            tables_attrs curr_table;
            curr_table.columns_size = curr_table.columns.size() * sizeof(col_attrs);
            curr_table.table_name_size = curr_table.table_name.size();

            fwrite(&curr_table.columns_size, sizeof(size_t), 1, schmea_file);
            fwrite(&curr_table.heap_page_offset, sizeof(size_t), 1, schmea_file);
            fwrite(&curr_table.page_id, sizeof(heap_page_types::page_id), 1, schmea_file);
            fwrite(&curr_table.table_name_size, sizeof(size_t), 1, schmea_file);
            fwrite(curr_table.table_name.data(), sizeof(char), curr_table.table_name_size, schmea_file);

            int k = 0;
            while (k < curr_table.columns.size()) {
                col_attrs curr_column;
                curr_column.column_name_size = curr_column.column_name.size();

                fwrite(&curr_column.column_name_size, sizeof(size_t), 1, schmea_file);
                fwrite(curr_column.column_name.data(), sizeof(char), curr_column.column_name_size, schmea_file);
                fwrite(&curr_column.type, sizeof(int), 1, schmea_file);

                k++;
            }
            j++;
        }
        i++;
    }
}
