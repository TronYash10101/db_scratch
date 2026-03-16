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
    buffer_manager::buffer_pool buff_pool(heap_filepath, index_filepath);
    access_methods::Access_methods access_methods;

    access_methods_types::row_t temp_row = {1, 1};
    access_methods_types::row_t temp_row2 = {5, 2};
    access_methods_types::row_t temp_row3 = {6, 1};
    access_methods_types::row_t temp_row4 = {2, 2};

    /* {
        std::string query = "SELECT x FROM test WHERE x >= 1";
        parser::token_iterator tok_it(query);
        std::vector<std::unique_ptr<planner::Operator>> operators;
        parser::Parser Parser;

        index_write::root_struct curr_root;
        curr_root.root_pid = buffer_manager_types::INVALID_PAGE_ID;
    } */

    {
        std::string insert_query = "INSERT INTO test VALUES (1,2)";
        std::string select_query = "SELECT x FROM test WHERE x >= 1";

        parser::token_iterator select_tok_it(select_query);
        parser::token_iterator insert_tok_it(insert_query);
        std::vector<std::unique_ptr<planner::Operator>> select_operators;
        std::vector<std::unique_ptr<planner::Operator>> insert_operators;
        parser::Parser Parser;

        index_write::root_struct curr_root;
        curr_root.root_pid = buffer_manager_types::INVALID_PAGE_ID;

        buffer_manager_types::Page *root_page = index_write::fetch_page(buff_pool);

        parser_types::ASTResult select_res_ast = Parser.grammer_check(select_tok_it);
        parser_types::ASTResult insert_res_ast = Parser.grammer_check(insert_tok_it);

        if (auto insert_ast = std::get_if<parser_types::INSERT_AST>(&insert_res_ast)) {
            std::vector<access_methods_types::row_t> k =
                    planner::insert_plan(insert_operators, buff_pool, access_methods, *insert_ast, curr_root);

            for (const access_methods_types::row_t &row : k) {
                std::cout << "Inserted row x: " << row.x << "   ";
            }
        }

        if (auto select_ast = std::get_if<parser_types::SELECT_AST>(&select_res_ast)) {
            std::vector<access_methods_types::row_t> r = planner::select_plan(select_operators, buff_pool, access_methods, *select_ast);
            if (r.size() == 0) {
                std::cout << "no row found";
            } else {
                for (const access_methods_types::row_t &ele : r) {
                    std::cout << "x: " << ele.x << " y: " << ele.y << "\n";
                }
            }
        }
    }
    {}
}
