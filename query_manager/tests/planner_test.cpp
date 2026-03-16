#include "../../storage_manager/headers/index_writer.hpp"
#include "../../storage_manager/headers/insert.hpp"
#include "../headers/lexer.hpp"
#include "../headers/parser.hpp"
#include "../headers/planner.hpp"
#include <iostream>
#include <optional>
#include <string>
#include <variant>
#include <vector>

int main() {
    std::filesystem::path heap_filepath = "/home/yash-jadhav/db_scratch/heap.bin";
    std::filesystem::path index_filepath = "/home/yash-jadhav/db_scratch/index.bin";

    std::ofstream file1(index_filepath, std::ios::binary | std::ios::out | std::ios::trunc);
    std::ofstream file2(heap_filepath, std::ios::binary | std::ios::out | std::ios::trunc);
    file1.close();
    file2.close();

    std::string query = "SELECT x FROM test WHERE x >= 1";

    buffer_manager::buffer_pool buff_pool(heap_filepath, index_filepath);
    access_methods::Access_methods access_methods;
    parser::token_iterator tok_it(query);
    std::vector<std::unique_ptr<planner::Operator>> operators;
    parser::Parser Parser;

    access_methods_types::row_t temp_row = {1, 1};
    access_methods_types::row_t temp_row2 = {5, 2};
    access_methods_types::row_t temp_row3 = {6, 1};
    access_methods_types::row_t temp_row4 = {2, 2};

    index_write::root_struct curr_root;
    curr_root.root_pid = buffer_manager_types::INVALID_PAGE_ID;

    buffer_manager_types::Page *root_page = index_write::fetch_page(buff_pool);

    insert::create_entry(buff_pool, access_methods, temp_row, curr_root);
    insert::create_entry(buff_pool, access_methods, temp_row2, curr_root);
    insert::create_entry(buff_pool, access_methods, temp_row3, curr_root);
    insert::create_entry(buff_pool, access_methods, temp_row4, curr_root);

    std::vector<lexer_types::Token> tokens = lexer::lexer(query);
    parser_types::ASTResult res_ast = Parser.grammer_check(tok_it);
    if (auto select_ast = std::get_if<parser_types::SELECT_AST>(&res_ast)) {
        std::vector<access_methods_types::row_t> r = planner::select_plan(operators, buff_pool, access_methods, *select_ast);
        if (r.size() == 0) {
            std::cout << "no row found";
        } else {
            for (const access_methods_types::row_t &ele : r) {
                std::cout << "x: " << ele.x << " y: " << ele.y << "\n";
            }
        }
    }
}
