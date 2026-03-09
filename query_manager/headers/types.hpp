#ifndef TYPES
#define TYPES

#include <cstring>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace lexer_types {
enum TOKEN_TYPE { OPERATOR, IDENT, CLAUSE };
struct Token {
    TOKEN_TYPE token_type;
    std::string token_value;
};
const std::unordered_map<std::string, TOKEN_TYPE> lexer_table = {{"SELECT", CLAUSE}, {"FROM", CLAUSE}, {"WHERE", CLAUSE}, {",", OPERATOR},
                                                                 {"*", OPERATOR},    {"=", OPERATOR},  {"<", OPERATOR},   {">", OPERATOR},
                                                                 {">=", OPERATOR},   {"<=", OPERATOR}, {";", OPERATOR}};
} // namespace lexer_types

namespace parser_types {
inline std::unordered_set<std::string> columns = {"name", "age", "birth"};

} // namespace parser_types

#endif
