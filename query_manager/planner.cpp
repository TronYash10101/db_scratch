#include "headers/planner.hpp"
#include <memory>
#include <optional>
#include <stdexcept>
#include <utility>
#include <variant>
#include <vector>

std::vector<access_methods_types::row_t> planner::select_plan(std::vector<std::unique_ptr<Operator>> &operators,
                                                              buffer_manager::buffer_pool &buff_pool,
                                                              access_methods::Access_methods &access_methods,
                                                              schema::schema_manager &sch_man, parser_types::SELECT_AST &ast,
                                                              std::string schema_name) {

    std::vector<access_methods_types::row_t> matched_rows;
    operators.reserve(3);

    std::optional<std::vector<schema::ENTITY_TYPE>> table_find = sch_man.entity_find(schema::TABLE, ast.table_name, schema_name);
    schema::tables_attrs *table_ptr;
    if (table_find.has_value()) {
        if (!(table_ptr = std::get_if<schema::tables_attrs>(&table_find.value()[0]))) {
            throw std::runtime_error("ERROR AT GETTING TABLE NAME FOR PLANNER");
        }
    }

    std::optional<std::vector<schema::ENTITY_TYPE>> c_t = sch_man.entity_find(schema::TABLE, ast.table_name, schema_name);

    std::vector<size_t> data_size_arr;
    std::vector<access_methods_types::SUPORTED_COLUMN_TYPE> col_types;

    if (c_t.has_value()) {
        if (auto *c_t_ptr = std::get_if<schema::tables_attrs>(&c_t.value()[0])) {
            for (const auto &ele : c_t_ptr->columns) {
                col_types.push_back(ele.column_type);
                switch (ele.column_type) {
                case access_methods_types::STRING:
                    data_size_arr.push_back(access_methods_types::STRING_MAX_SIZE);
                    break;
                case access_methods_types::INTEGER:
                    data_size_arr.push_back(sizeof(int));
                    break;
                case access_methods_types::FLOATING:
                    data_size_arr.push_back(sizeof(float));
                    break;
                }
            }
        }
    }

    auto seq_scan = std::make_unique<Seq_scan>(*table_ptr, access_methods, buff_pool, data_size_arr, col_types);
    seq_scan->init();
    auto filter = std::make_unique<Filter>(*table_ptr, *seq_scan, ast.predicate);
    filter->init();
    auto project = std::make_unique<Projection>(*table_ptr, *filter, ast);
    project->init();

    operators.push_back(std::move(seq_scan));
    operators.push_back(std::move(filter));
    operators.push_back(std::move(project));

    std::optional<access_methods_types::row_t> row = operators.back()->next();
    while (row.has_value()) {
        matched_rows.push_back(row.value());
        row = operators.back()->next();
    }
    std::cout << "scaned all the pages\n";
    return matched_rows;
}

std::vector<access_methods_types::row_t> planner::insert_plan(std::vector<std::unique_ptr<Operator>> &operators,
                                                              buffer_manager::buffer_pool &buff_pool,
                                                              access_methods::Access_methods &access_methods,
                                                              schema::schema_manager &sch_man, parser_types::INSERT_AST &ast,
                                                              index_write::root_struct &curr_root, std::string schema_name) {

    operators.reserve(1);
    std::vector<access_methods_types::row_t> inserted_rows;
    std::optional<std::vector<schema::ENTITY_TYPE>> table_find = sch_man.entity_find(schema::TABLE, ast.table_name, schema_name);
    schema::tables_attrs *table_ptr;
    if (table_find.has_value()) {
        if (!(table_ptr = std::get_if<schema::tables_attrs>(&table_find.value()[0]))) {
            throw std::runtime_error("ERROR AT GETTING TABLE NAME FOR PLANNER");
        }
    }
    auto insert = std::make_unique<Insert>(*table_ptr, nullptr, ast, access_methods, buff_pool, curr_root);

    operators.push_back(std::move(insert));
    std::optional<access_methods_types::row_t> inserted_row = operators.back()->next();
    while (inserted_row.has_value()) {
        inserted_rows.push_back(inserted_row.value());
        inserted_row = operators.back()->next();
    }

    return inserted_rows;
}
