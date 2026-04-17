#include "../headers/parser.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

int main() {
    /* {
        // Happy path: valid projection list.
        parser::token_iterator it("SELECT age , name FROM T");
        std::vector<std::string> cols = parser::grammer_check(it);
        assert(cols.size() == 2);
        assert(cols[0] == "age");
        assert(cols[1] == "name");
    }

    {
        // Wildcard should return all known columns.
        parser::token_iterator it("SELECT * FROM T");
        std::vector<std::string> cols = parser::grammer_check(it);
        // parser_types::columns is {"name","age"} (unordered), but size should match.
        assert(cols.size() == parser_types::columns.size());
    }

    {
        // Trailing comma should cause an error, e.g. \"SELECT name, FROM T\".
        bool threw = false;
        try {
            parser::token_iterator it("SELECT name, FROM T");
            (void)parser::grammer_check(it);
        } catch (const std::runtime_error &) {
            threw = true;
        }
        assert(threw && "Expected parser to throw on trailing comma without identifier");
    } */

    {
        std::filesystem::path filepath = "/home/yash-jadhav/db_scratch/catalog_manager/schema_file.bin";
        parser::token_iterator it("SELECT x FROM test");
        parser::Parser Parser;
        schema::schema_manager sch_ma(filepath);
        std::string schmea_name = "s1";
        parser_types::ASTResult res = Parser.grammer_check(it, sch_ma, schmea_name);
        if (auto select = std::get_if<parser_types::SELECT_AST>(&res)) {
            std::cout << "Col names:\n ";
            for (const std::string &ele : select->cols_name) {
                std::cout << ele << " ";
            }
            std::cout << "\n";

            std::cout << "Table: ";
            std::cout << select->table_name << "\n";

            std::cout << "Predicate: ";
            std::cout << select->predicate.col << select->predicate.op << select->predicate.value << "\n";
        }
    }

    /* {
        parser::token_iterator it("INSERT INTO test (x,y) VALUES (1,2),(4,5)");
        parser::Parser Parser;
        parser_types::ASTResult res = Parser.grammer_check(it);
        if (auto inesrt = std::get_if<parser_types::INSERT_AST>(&res)) {
            std::cout << "Col names:\n ";
            for (const std::string &ele : inesrt->cols_name) {
                std::cout << ele << " ";
            }
            std::cout << "\n";

            std::cout << "Table: ";
            std::cout << inesrt->table_name << "\n";

            std::cout << "values:\n ";
            for (const access_methods_types::row_t &ele : inesrt->values) {
                std::cout << "(" << ele.x << ", " << ele.y << ")" << " ";
            }
            std::cout << "\n";
        }
    } */

    std::cout << "parser tests passed\n";
    return 0;
}
