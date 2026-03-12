#include "headers/planner.hpp"
#include <memory>
#include <vector>

std::optional<access_methods_types::row_t> select_plan(buffer_manager::buffer_pool &buff_pool,
                                                       access_methods::Access_methods &access_manager, parser_types::AST &ast) {

    std::vector<std::unique_ptr<Operator>> operators;

    auto seq_scan = std::make_unique<Seq_scan>(access_manager, buff_pool);
    auto filter = std::make_unique<Filter>(*seq_scan, ast.predicate);
    auto project = std::make_unique<Projection>(*filter, ast);

    operators.push_back(std::move(seq_scan));
    operators.push_back(std::move(filter));
    operators.push_back(std::move(project));

    std::optional<access_methods_types::row_t> row = project->next();
    if (row.has_value()) {
        return row.value();
    }
    return std::nullopt;
}
