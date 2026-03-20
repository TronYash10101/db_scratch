#ifndef SCHMEA_BUILD
#define SCHMEA_BUILD

#include "../../query_manager/headers/types.hpp"
#include <cstddef>
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace schema {

#pragma pack(push, 1)
struct col_attrs {
    std::string column_name;
    parser_types::COLUMN_TYPE column_type;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct tables_attrs {
    size_t heap_page_offset = 0;
    heap_page_types::page_id page_id = 0;
    std::string table_name;
    std::vector<col_attrs> columns;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct schema_attr {
    std::string schema_name;
    std::vector<tables_attrs> tables;
};
#pragma pack(pop)

class schema_manager {
  private:
    std::filesystem::path schmea_table_path;
    FILE *schmea_file;
    size_t schema_file_size;
    std::vector<schema_attr> schema_catalog;

    std::optional<int> schema_find(const std::string schema_name) const {
        for (int i = 0; i < schema_catalog.size(); i++) {
            if (schema_catalog[i].schema_name == schema_name) {
                return i;
            }
        }

        return std::nullopt;
    };

  public:
    schema_manager(const std::filesystem::path &schmea_table_path) : schmea_table_path(schmea_table_path) {
        if (!std::filesystem::exists(schmea_table_path)) {
            schmea_file = fopen(schmea_table_path.c_str(), "rb+");
        } else {
            schmea_file = fopen(schmea_table_path.c_str(), "wb+");
        }
        fseek(schmea_file, 0, SEEK_END);
        schema_file_size = ftell(schmea_file);
        fseek(schmea_file, 0, SEEK_SET);

        deserialize();
    };

    void deserialize();
    void serialize();
    void create_schema(parser_types::SCHEMA_AST &ast);

    void schema_create_table(const std::string &schema_name, parser_types::CREATE_TABLE_AST &ast);
    void iterate_catalog() const {
        for (const schema_attr &ele : schema_catalog) {
            std::cout << "Schema name: ";
            std::cout << ele.schema_name;
            std::cout << "Table name: ";
            for (const tables_attrs &table : ele.tables) {
                std::cout << table.table_name;
                std::cout << "\n";
                for (const col_attrs &col : table.columns) {
                    std::cout << col.column_name;
                    std::cout << "\n";
                }
            }
            std::cout << "\n";
        }
    }

    ~schema_manager() {
        // write schema here, before closing file
        serialize();
        std::cout << "SCHEMA FILE WRITTEN";
        fclose(schmea_file);
        fflush(schmea_file);
    }
};

} // namespace schema

#endif
