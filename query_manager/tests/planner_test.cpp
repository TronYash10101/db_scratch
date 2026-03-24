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
    std::filesystem::path schema_filepath = "/home/yash-jadhav/db_scratch/catalog_manager/schema_file.bin";

    std::ofstream file1(index_filepath, std::ios::binary | std::ios::out | std::ios::trunc);
    std::ofstream file2(heap_filepath, std::ios::binary | std::ios::out | std::ios::trunc);
    file1.close();
    file2.close();
    buffer_manager::buffer_pool buff_pool(heap_filepath, index_filepath);
    access_methods::Access_methods access_methods;

    /* access_methods_types::row_t temp_row = {1, 1};
    access_methods_types::row_t temp_row2 = {5, 2};
    access_methods_types::row_t temp_row3 = {6, 1};
    access_methods_types::row_t temp_row4 = {2, 2}; */

    {
        std::string schema_name = "abc";

        std::string create_schema = "CREATE SCHEMA abc";
        std::string create_table = "CREATE TABLE test (age INT)";
        std::string insert_query = "INSERT INTO test VALUES (2),(1)";

        parser::token_iterator insert_tok_it(insert_query);
        parser::token_iterator s_tok_it(create_schema);
        parser::token_iterator t_tok_it(create_table);

        std::vector<std::unique_ptr<planner::Operator>> insert_operators;

        schema::schema_manager sch_ma(schema_filepath);
        parser::Parser Parser;

        // ---------------- SCHEMA ----------------
        parser_types::ASTResult schema_res_ast = Parser.grammer_check(s_tok_it, sch_ma, schema_name);

        if (auto sast = std::get_if<parser_types::SCHEMA_AST>(&schema_res_ast)) {
            sch_ma.create_schema(*sast);
        }

        // ---------------- TABLE ----------------
        parser_types::ASTResult table_res_ast = Parser.grammer_check(t_tok_it, sch_ma, schema_name);

        if (auto tast = std::get_if<parser_types::CREATE_TABLE_AST>(&table_res_ast)) {
            sch_ma.schema_create_table(schema_name, *tast);
        }

        // ---------------- INDEX ROOT INIT ----------------
        index_write::root_struct curr_root;
        curr_root.root_pid = buffer_manager_types::INVALID_PAGE_ID;

        // NOTE: ensure this actually allocates + initializes page
        buffer_manager_types::Page *root_page = index_write::fetch_page(buff_pool);

        // ---------------- INSERT ----------------
        parser_types::ASTResult insert_res_ast = Parser.grammer_check(insert_tok_it, sch_ma, schema_name);

        if (auto insert_ast = std::get_if<parser_types::INSERT_AST>(&insert_res_ast)) {

            std::vector<access_methods_types::row_t> res =
                    planner::insert_plan(insert_operators, buff_pool, access_methods, sch_ma, *insert_ast, curr_root, &schema_name);
        }
    }

    /* {
        std::string select_query = "SELECT x FROM test WHERE x >= 1";
        parser::token_iterator select_tok_it(select_query);
        std::vector<std::unique_ptr<planner::Operator>> select_operators;
        parser::Parser Parser;
        parser_types::ASTResult select_res_ast = Parser.grammer_check(select_tok_it);
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
    } */
}
