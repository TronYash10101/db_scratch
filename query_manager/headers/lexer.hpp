#ifndef LEXER
#define LEXER

#include <cstring>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace lexer {

enum TOKEN_TYPE { OPERATOR, IDENT, CLAUSE };
struct Token {
    TOKEN_TYPE token_type;
    std::string token_value;
};
const std::unordered_map<std::string, TOKEN_TYPE> lexer_table = {{"SELECT", CLAUSE}, {"FROM", CLAUSE}, {"WHERE", CLAUSE}, {",", OPERATOR},
                                                                 {"*", OPERATOR},    {"=", OPERATOR},  {"<", OPERATOR},   {">", OPERATOR},
                                                                 {">=", OPERATOR},   {"<=", OPERATOR}, {";", OPERATOR}};
std::vector<lexer::Token> _lexer(const std::string &query);

void whitespace_split(const std::string &input, std::vector<Token> &result_arr);

bool check_seperator(std::string seperator);

} // namespace lexer

#endif
