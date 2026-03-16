#include "headers/planner.hpp"
#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

std::vector<access_methods_types::row_t> planner::select_plan(std::vector<std::unique_ptr<Operator>> &operators,
                                                              buffer_manager::buffer_pool &buff_pool,
                                                              access_methods::Access_methods &access_methods,
                                                              parser_types::SELECT_AST &ast) {

    std::vector<access_methods_types::row_t> matched_rows;
    operators.reserve(3);

    auto seq_scan = std::make_unique<Seq_scan>(access_methods, buff_pool);
    seq_scan->init();
    auto filter = std::make_unique<Filter>(*seq_scan, ast.predicate);
    filter->init();
    auto project = std::make_unique<Projection>(*filter, ast);
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
                                                              access_methods::Access_methods &access_methods, parser_types::INSERT_AST &ast,
                                                              index_write::root_struct &curr_root) {

    operators.reserve(1);
    std::vector<access_methods_types::row_t> inserted_rows;
    auto insert = std::make_unique<Insert>(nullptr, ast, access_methods, buff_pool, curr_root);

    operators.push_back(std::move(insert));
    std::optional<access_methods_types::row_t> inserted_row = operators.back()->next();
    while (inserted_row.has_value()) {
        inserted_rows.push_back(inserted_row.value());
        inserted_row = operators.back()->next();
    }

    return inserted_rows;
}
