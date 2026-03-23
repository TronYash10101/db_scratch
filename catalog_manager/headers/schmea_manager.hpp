#ifndef SCHMEA_BUILD
#define SCHMEA_BUILD

#include "../../query_manager/headers/types.hpp"
#include <cstddef>
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

namespace schema {

#pragma pack(push, 1)
struct col_attrs {
    std::string column_name;
    access_methods_types::SUPORTED_COLUMN_TYPE column_type;

    template <typename T> bool col_type_match(access_methods_types::SUPORTED_COLUMN_TYPE col_type, T value) const {
        if (col_type == access_methods_types::STRING) {
            return std::is_same_v<T, std::string>;
        } else if (col_type == access_methods_types::INTEGER) {
            return std::is_same_v<T, int>;
        } else if (col_type == access_methods_types::FLOATING) {
            return std::is_same_v<T, float>;
        }
        return false;
    }
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

enum FIND_TYPE { SCHEMA, TABLE, COLUMN };

using ENTITY_TYPE = std::variant<schema_attr, tables_attrs, col_attrs>;

class schema_manager {
  private:
    std::filesystem::path schmea_table_path;
    FILE *schmea_file;
    size_t schema_file_size;
    void deserialize();
    void serialize();
    std::vector<schema_attr> schema_catalog;

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

    void create_schema(parser_types::SCHEMA_AST &ast);
    void schema_create_table(const std::string &schema_name, parser_types::CREATE_TABLE_AST &ast);

    std::optional<std::vector<ENTITY_TYPE>> entity_find(FIND_TYPE find, const std::string &child_context,
                                                        std::string *parent_context = nullptr,
                                                        std::string *grand_parent_context = nullptr) {
        std::vector<ENTITY_TYPE> res;
        switch (find) {
        case SCHEMA:
            for (size_t i = 0; i < schema_catalog.size(); i++) {
                if (child_context == schema_catalog[i].schema_name) {
                    res.push_back(schema_catalog[i]);
                    return res;
                }
            }
            return std::nullopt;
            break;

        case TABLE:
            if (parent_context != nullptr) {

                std::optional<std::vector<ENTITY_TYPE>> schema_find = entity_find(SCHEMA, *parent_context);
                if (schema_find.has_value() && schema_find->size() == 1) {
                    if (auto *curr_schema = std::get_if<schema_attr>(&schema_find.value()[0])) {
                        for (int k = 0; k < curr_schema->tables.size(); k++) {
                            if (child_context == curr_schema->tables[k].table_name) {
                                res.push_back(curr_schema->tables[k]);
                                return res;
                            }
                        }
                    }
                }
            }
            return std::nullopt;
            break;

        case COLUMN:
            if (grand_parent_context != nullptr && parent_context != nullptr) {
                std::optional<std::vector<ENTITY_TYPE>> schema_find = entity_find(SCHEMA, *grand_parent_context);

                std::optional<std::vector<ENTITY_TYPE>> table_find = entity_find(TABLE, child_context, parent_context);

                if (schema_find.has_value() && table_find.has_value()) {
                    if (tables_attrs *curr_table = std::get_if<tables_attrs>(&table_find.value()[0])) {
                        if (child_context == "*") {
                            for (size_t z = 0; z < curr_table->columns.size(); z++) {
                                res.push_back(curr_table->columns[z]);
                            }
                            return res;
                        } else {
                            for (size_t z = 0; z < curr_table->columns.size(); z++) {
                                if (child_context == curr_table->columns[z].column_name) {
                                    return res;
                                }
                            }
                        }
                    }
                }
            }
            return std::nullopt;

        default:
            throw std::runtime_error("ERROR FINDING ENTITY");
        }
    };
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
