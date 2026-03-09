#include "../headers/lexer.hpp"
#include <iostream>
#include <string>
#include <vector>

std::string enum_print(lexer_types::TOKEN_TYPE tok) {
    switch (tok) {
    case lexer_types::OPERATOR:
        return "OPERATOR";
        break;
    case lexer_types::IDENT:
        return "IDENT";
        break;
    case lexer_types::CLAUSE:
        return "CLAUSE";
        break;

    default:
        return "NULL";
    }
}

int main() {
    const std::string x = "SELECT a, b FROM T x>10";

    std::vector<lexer_types::Token> res = lexer::lexer(x);

    for (const lexer_types::Token &ele : res) {
        std::cout << "TOKEN TYPE: " << enum_print(ele.token_type) << " | TOKEN VALUE: " << ele.token_value << "\n";
    }
}
