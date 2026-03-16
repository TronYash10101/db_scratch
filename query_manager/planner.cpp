#include "headers/planner.hpp"
#include <memory>
#include <vector>

std::vector<access_methods_types::row_t> planner::select_plan(std::vector<std::unique_ptr<Operator>> &operators,
                                                              buffer_manager::buffer_pool &buff_pool,
                                                              access_methods::Access_methods &access_manager,
                                                              parser_types::SELECT_AST &ast) {

    std::vector<access_methods_types::row_t> matched_rows;
    operators.reserve(3);

    auto seq_scan = std::make_unique<Seq_scan>(access_manager, buff_pool);
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

void planner::insert() {}
