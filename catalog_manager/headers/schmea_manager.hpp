#ifndef SCHMEA_BUILD
#define SCHMEA_BUILD

#include "../../query_manager/headers/types.hpp"
#include <cstdio>
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace schema {
using COLUMN_TYPE = std::variant<int, float, std::string>;

struct schema {
    struct col_attrs {
        COLUMN_TYPE type;
        bool primary_key; // not supported
    };

    struct tables {
        size_t offset;
        std::unordered_map<std::string, col_attrs> columns;
    };

    std::string schema_name;
    std::unordered_map<std::string, tables> tables;
};

class schema_manager {
  private:
    std::filesystem::path schmea_table_path;
    std::unordered_map<schema, heap_page_types::page_id> schema_catalog;

  public:
    schema_manager() {
        FILE *schema_file = fopen(schmea_table_path.c_str(), "r");

        fread(void *__restrict ptr, size_t size, size_t n, schema_file);
        schema_catalog = reinterpret_cast<schema *>(schema_file);
    };

    void build_schema(parser_types::SCHEMA_AST &ast) {}
};

} // namespace schema

#endif
