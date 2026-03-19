#ifndef SCHMEA_BUILD
#define SCHMEA_BUILD

#include "../../query_manager/headers/types.hpp"
#include <cstddef>
#include <cstdio>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace schema {

#pragma pack(push, 1)
struct col_attrs {
    size_t column_name_size;
    std::string column_name;
    parser_types::COLUMN_TYPE type;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct tables_attrs {
    size_t columns_size;
    size_t heap_page_offset;
    heap_page_types::page_id page_id;
    size_t table_name_size;
    std::string table_name;
    std::vector<col_attrs> columns;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct schema_attr {
    size_t schema_name_size;
    std::string schema_name;
    size_t tables_size;
    std::vector<tables_attrs> tables;
};
#pragma pack(pop)

class schema_manager {
  private:
    std::filesystem::path schmea_table_path;
    FILE *schmea_file;
    size_t schema_file_size;
    std::vector<schema_attr> schema_catalog;

    std::optional<schema_attr> schema_find(const std::string schema_name) const {
        for (const schema_attr &schema : schema_catalog) {
            if (schema.schema_name == schema_name) {
                return schema;
            }
        }
        return std::nullopt;
    };

  public:
    schema_manager(const std::filesystem::path &schmea_table_path) : schmea_table_path(schmea_table_path) {
        if (!std::filesystem::exists(schmea_table_path)) {
            schmea_file = fopen(schmea_table_path.c_str(), "w+b");
        } else {
            schmea_file = fopen(schmea_table_path.c_str(), "r+b");
        }
        fseek(schmea_file, 0, SEEK_END);
        schema_file_size = ftell(schmea_file);
        fseek(schmea_file, 0, SEEK_SET);
    };

    void deserialize();
    void serialize();
    void create_schema(std::vector<schema_attr> schmea_catalog, schema::schema_attr &schema, parser_types::SCHEMA_AST &ast);

    void schema_create_table(std::vector<schema_attr> schmea_catalog, schema_attr &curr_schema, parser_types::CREATE_TABLE_AST &ast);

    ~schema_manager() {
        // write schema here, before closing file
        fclose(schmea_file);
    }
};

} // namespace schema

#endif
