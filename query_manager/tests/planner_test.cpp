#include "../../storage_manager/headers/index_writer.hpp"
#include "../../storage_manager/headers/insert.hpp"
#include "../headers/lexer.hpp"
#include "../headers/parser.hpp"
#include "../headers/planner.hpp"
#include <iostream>
#include <optional>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

int main() {
    std::filesystem::path heap_filepath = "/home/yash-jadhav/db_scratch/heap.bin";
    std::filesystem::path index_filepath = "/home/yash-jadhav/db_scratch/index.bin";
    std::filesystem::path schema_filepath = "/home/yash-jadhav/db_scratch/catalog_manager/schema_file.bin";

    std::ofstream file1(index_filepath, std::ios::binary | std::ios::out | std::ios::trunc);
    std::ofstream file2(heap_filepath, std::ios::binary | std::ios::out | std::ios::trunc);
    std::ofstream file3(schema_filepath, std::ios::binary | std::ios::out | std::ios::trunc);
    file1.close();
    file2.close();
    file3.close();
    buffer_manager::buffer_pool buff_pool(index_filepath, heap_filepath);
    access_methods::Access_methods access_methods;
    schema::schema_manager sch_ma(schema_filepath);

    {
        // 1. Define Queries
        std::string schema_name = "school_db";
        std::string create_schema = "CREATE SCHEMA school_db";
        std::string create_table = "CREATE TABLE students (name STRING, grade INT)";
        std::string insert_query1 = "INSERT INTO students (name, grade) VALUES ('Alice',3)";
        std::string insert_query2 = "INSERT INTO students (name, grade) VALUES ('Bob',2)";
        std::string insert_query3 = "INSERT INTO students (name, grade) VALUES ('Tim',2)";
        std::string select_query = "SELECT name FROM students WHERE grade < 5";

        parser::Parser Parser;
        index_write::root_struct curr_root;
        curr_root.root_pid = buffer_manager_types::INVALID_PAGE_ID;

        // ---------------- SCHEMA ----------------
        parser::token_iterator s_tok_it(create_schema);
        parser_types::ASTResult s_ast = Parser.grammer_check(s_tok_it, sch_ma, schema_name);
        if (auto sast = std::get_if<parser_types::SCHEMA_AST>(&s_ast)) {
            sch_ma.create_schema(*sast);
        }

        // ---------------- TABLE ----------------
        parser::token_iterator t_tok_it(create_table);
        parser_types::ASTResult t_ast = Parser.grammer_check(t_tok_it, sch_ma, schema_name);
        if (auto tast = std::get_if<parser_types::CREATE_TABLE_AST>(&t_ast)) {
            sch_ma.schema_create_table(schema_name, *tast);
        }

        // ---------------- INSERT 1 (Alice) ----------------
        parser::token_iterator i1_tok_it(insert_query1);
        parser_types::ASTResult i1_ast = Parser.grammer_check(i1_tok_it, sch_ma, schema_name);
        if (auto iast = std::get_if<parser_types::INSERT_AST>(&i1_ast)) {
            planner::insert_plan(buff_pool, access_methods, sch_ma, *iast, curr_root, schema_name);
        }

        // ---------------- INSERT 2 (Bob) ----------------
        parser::token_iterator i2_tok_it(insert_query2);
        parser_types::ASTResult i2_ast = Parser.grammer_check(i2_tok_it, sch_ma, schema_name);
        if (auto iast = std::get_if<parser_types::INSERT_AST>(&i2_ast)) {
            planner::insert_plan(buff_pool, access_methods, sch_ma, *iast, curr_root, schema_name);
        }
        // ---------------- INSERT 2 (Tim) ----------------
        parser::token_iterator i3_tok_it(insert_query3);
        parser_types::ASTResult i3_ast = Parser.grammer_check(i3_tok_it, sch_ma, schema_name);
        if (auto iast = std::get_if<parser_types::INSERT_AST>(&i3_ast)) {
            planner::insert_plan(buff_pool, access_methods, sch_ma, *iast, curr_root, schema_name);
        }

        // ---------------- SELECT (Where Grade = 'A') ----------------
        parser::token_iterator sel_tok_it(select_query);
        parser_types::ASTResult sel_ast = Parser.grammer_check(sel_tok_it, sch_ma, schema_name);

        if (auto sast = std::get_if<parser_types::SELECT_AST>(&sel_ast)) {
            std::vector<access_methods_types::row_t> results = planner::select_plan(buff_pool, access_methods, sch_ma, *sast, schema_name);

            std::cout << "\n--- SELECT RESULTS ---" << std::endl;
            if (results.empty()) {
                std::cout << "No rows found." << std::endl;
            } else {

                for (const auto &row_container : results) {
                    for (const auto &col_variant : row_container.row) {
                        // Visitor style printing
                        std::visit([](auto &&arg) { std::cout << "VALUE: " << arg << " "; }, col_variant);
                    }
                    std::cout << std::endl;
                }
            }
        }
    }
}
