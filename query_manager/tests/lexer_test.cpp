#include "../headers/lexer.hpp"
#include <iostream>
#include <string>
#include <vector>

std::string enum_print(lexer::TOKEN_TYPE tok) {
    switch (tok) {
    case lexer::OPERATOR:
        return "OPERATOR";
        break;
    case lexer::IDENT:
        return "IDENT";
        break;
    case lexer::CLAUSE:
        return "CLAUSE";
        break;

    default:
        return "NULL";
    }
}

int main() {
    const std::string x = "SELECT a,b FROM T x >= 10 ";

    std::vector<lexer::Token> res = lexer::_lexer(x);

    for (const lexer::Token ele : res) {
        std::cout << "TOKEN TYPE: " << enum_print(ele.token_type) << " | TOKEN VALUE: " << ele.token_value << "\n";
    }
}
