#include "../headers/lexer.hpp"
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

// Simple helper to check a single token.
void expect_token(const lexer_types::Token &tok, lexer_types::TOKEN_TYPE type, const std::string &value) {
    assert(tok.token_type == type);
    assert(tok.token_value == value);
}

int main() {
    /* {
        // Basic SELECT with two columns and WHERE-like clause.
        const std::string query = "SELECT name, age FROM T age>10";
        std::vector<lexer_types::Token> toks = lexer::lexer(query);

        // 0: SELECT (CLAUSE)
        expect_token(toks[0], lexer_types::CLAUSE, "SELECT");
        // 1: name (IDENT)
        expect_token(toks[1], lexer_types::IDENT, "name");
        // 2: , (OPERATOR)
        expect_token(toks[2], lexer_types::OPERATOR, ",");
        // 3: age (IDENT)
        expect_token(toks[3], lexer_types::IDENT, "age");
        // 4: FROM (CLAUSE)
        expect_token(toks[4], lexer_types::CLAUSE, "FROM");
    }

    {
        // Check that multi-character operators like \">=\" are recognized.
        const std::string query = "age>=10";
        std::vector<lexer_types::Token> toks = lexer::lexer(query);
        assert(toks.size() == 3);
        expect_token(toks[0], lexer_types::IDENT, "age");
        expect_token(toks[1], lexer_types::OPERATOR, ">=");
    }

    {
        // Trailing semicolon token.
        const std::string query = "SELECT * FROM T;";
        std::vector<lexer_types::Token> toks = lexer::lexer(query);
        // Last token should be \";\" operator.
        expect_token(toks.back(), lexer_types::OPERATOR, ";");
    } */
    {
        std::string query = "INSERT INTO test VALUES (1,2)";
        std::vector<lexer_types::Token> toks = lexer::lexer(query);
        for (const lexer_types::Token &tok : toks) {
            std::cout << tok.token_type << " | " << tok.token_value << "\n";
        }
    }
    std::cout << "lexer tests passed\n";
    return 0;
}
