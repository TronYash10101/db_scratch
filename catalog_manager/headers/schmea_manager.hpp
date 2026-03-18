#ifndef SCHMEA_BUILD
#define SCHMEA_BUILD

#include "../../query_manager/headers/types.hpp"
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
    std::string column_name;
    parser_types::COLUMN_TYPE type;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct tables_attrs {
    size_t heap_page_offset;
    std::string table_name;
    std::vector<col_attrs> columns;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct schema_attr {
    size_t schema_size;
    std::string schema_name;
    heap_page_types::page_id page_id;
    std::vector<tables_attrs> tables;
};
#pragma pack(pop)

class schema_manager {
  private:
    std::filesystem::path schmea_table_path;
    FILE *schmea_file;
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
    schema_manager() { schmea_file = fopen(schmea_table_path.c_str(), "r"); };

    void init();
    void write();
    void create_schema(std::vector<schema_attr> schmea_catalog, schema::schema_attr &schema, parser_types::SCHEMA_AST &ast);

    void schema_create_table(std::vector<schema_attr> schmea_catalog, schema_attr &curr_schema, parser_types::CREATE_TABLE_AST &ast);

    ~schema_manager() {
        // write schema here, before closing file
        fclose(schmea_file);
    }
};

} // namespace schema

#endif
