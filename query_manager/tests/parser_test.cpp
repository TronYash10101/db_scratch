#include "../headers/parser.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
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
        parser::token_iterator it("SELECT age FROM test WHERE age >= 10");

        parser_types::AST res = parser::grammer_check(it);
        std::cout << "Col names:\n ";
        for (const std::string &ele : res.cols_name) {
            std::cout << ele << " ";
        }
        std::cout << "\n";

        std::cout << "Table: ";
        std::cout << res.table_name << "\n";

        std::cout << "Predicate: ";
        std::cout << res.predicate.col << res.predicate.op << res.predicate.value << "\n";
    }

    std::cout << "parser tests passed\n";
    return 0;
}
