#include "headers/schmea_manager.hpp"
#include <cstdio>
#include <cwchar>
#include <optional>
#include <stdexcept>
#include <variant>
#include <vector>

void schema::schema_manager::create_schema(parser_types::SCHEMA_AST &ast) {

    schema_catalog.push_back({ast.schmea_name});
    // schema.page_id
    // can also handle other things like permission
}

void schema::schema_manager::schema_create_table(const std::string &schema_name, parser_types::CREATE_TABLE_AST &ast) {

    schema::tables_attrs table_attr;
    std::vector<schema::col_attrs> columns;

    for (size_t i = 0; i < ast.columns.size(); i++) {
        columns.push_back({ast.columns[i].column_name, ast.columns[i].column_type});
    }

    table_attr.table_name = ast.table_name;
    table_attr.columns = columns;

    // std::optional<std::vector<ENTITY_TYPE>> matched_idx = entity_find(SCHEMA, schema_name);
    /* if (matched_idx.has_value()) {
        if (auto *ptr = std::get_if<schema_attr>(&matched_idx.value()[0])) {
            ptr->tables.push_back(table_attr);
        } else {
            throw std::runtime_error("ERROR INSERTING NEW TABLE");
        }
    } else {
        throw std::runtime_error("NO SCHEMA FOUND");
    } */
    for (schema_attr &ele : schema_catalog) {
        if (ele.schema_name == schema_name) {
            ele.tables.push_back(table_attr);
        }
    }
}

void schema_alter_table(schema::tables_attrs &curr_table, parser_types::ALTER_TABLE_AST &ast) {}

void schema::schema_manager::deserialize() {

    while (ftell(schmea_file) < schema_file_size) {

        schema_attr curr_schema;

        size_t schema_name_size;
        fread(&schema_name_size, sizeof(size_t), 1, schmea_file);

        curr_schema.schema_name.resize(schema_name_size);
        fread(curr_schema.schema_name.data(), schema_name_size, 1, schmea_file);

        size_t table_size;
        fread(&table_size, sizeof(size_t), 1, schmea_file);
        curr_schema.tables.resize(table_size);

        size_t j = 0;
        while (j < table_size) {
            fread(&curr_schema.tables[j].heap_page_offset, sizeof(size_t), 1, schmea_file);
            fread(&curr_schema.tables[j].page_id, sizeof(heap_page_types::page_id), 1, schmea_file);

            size_t table_name_size;
            fread(&table_name_size, sizeof(size_t), 1, schmea_file);

            curr_schema.tables[j].table_name.resize(table_name_size);
            fread(curr_schema.tables[j].table_name.data(), table_name_size, 1, schmea_file);

            size_t columns_size;
            fread(&columns_size, sizeof(size_t), 1, schmea_file);
            curr_schema.tables[j].columns.resize(columns_size);

            size_t k = 0;
            while (k < columns_size) {
                size_t column_name_size;
                fread(&column_name_size, sizeof(size_t), 1, schmea_file);

                curr_schema.tables[j].columns[k].column_name.resize(column_name_size);
                fread(curr_schema.tables[j].columns[k].column_name.data(), column_name_size, 1, schmea_file);

                fread(&curr_schema.tables[j].columns[k].column_type, sizeof(int), 1, schmea_file);

                k++;
            }
            j++;
        }
        schema_catalog.push_back(curr_schema);
    }
}

void schema::schema_manager::serialize() {
    for (schema_attr &curr_schema : schema_catalog) {

        size_t tables_size = curr_schema.tables.size();
        size_t schema_name_size = curr_schema.schema_name.size();

        fwrite(&schema_name_size, sizeof(size_t), 1, schmea_file);
        fwrite(curr_schema.schema_name.data(), sizeof(char), schema_name_size, schmea_file);
        fwrite(&tables_size, sizeof(size_t), 1, schmea_file);

        for (tables_attrs &curr_table : curr_schema.tables) {
            // auto curr_table = schema_catalog[i].tables[j];
            size_t columns_size = curr_table.columns.size();
            size_t table_name_size = curr_table.table_name.size();

            fwrite(&curr_table.heap_page_offset, sizeof(size_t), 1, schmea_file);
            fwrite(&curr_table.page_id, sizeof(heap_page_types::page_id), 1, schmea_file);
            fwrite(&table_name_size, sizeof(size_t), 1, schmea_file);
            fwrite(curr_table.table_name.data(), sizeof(char), table_name_size, schmea_file);
            fwrite(&columns_size, sizeof(size_t), 1, schmea_file);

            for (col_attrs &curr_column : curr_table.columns) {
                size_t column_name_size = curr_column.column_name.size();

                fwrite(&column_name_size, sizeof(size_t), 1, schmea_file);
                fwrite(curr_column.column_name.data(), sizeof(char), column_name_size, schmea_file);
                fwrite(&curr_column.column_type, sizeof(int), 1, schmea_file);
            }
        }
    }
}
