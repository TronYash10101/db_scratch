#ifndef QUERY_TYPES
#define QUERY_TYPES

#include <cstring>
#include <iostream>
#include <stdexcept>
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

enum COLUMN_TYPE { STRING, INTEGER, FLOATING };

struct Predicate {
    std::string col = "";
    std::string op = "";
    std::string value = "";
};

struct AST {
    std::vector<std::string> cols_name;
    std::string table_name;
    Predicate predicate;
};

const std::unordered_map<std::string, COLUMN_TYPE> columns = {{"name", STRING}, {"age", INTEGER}, {"birth", FLOATING}};
const std::unordered_set<std::string> table = {"test"};

} // namespace parser_types

namespace planner {} // namespace planner

#endif
