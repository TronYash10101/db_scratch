#ifndef QUERY_TYPES
#define QUERY_TYPES

#include "../../storage_manager/headers/types.hpp"
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

namespace lexer_types {
enum TOKEN_TYPE { OPERATOR, IDENT, CLAUSE };
struct Token {
    TOKEN_TYPE token_type;
    std::string token_value;
};

const std::unordered_map<std::string, TOKEN_TYPE> lexer_table = {
        {"SELECT", CLAUSE}, {"INSERT", CLAUSE}, {"INTO", CLAUSE}, {"VALUES", CLAUSE}, {"FROM", CLAUSE}, {"WHERE", CLAUSE},
        {",", OPERATOR},    {"*", OPERATOR},    {"=", OPERATOR},  {"<", OPERATOR},    {">", OPERATOR},  {">=", OPERATOR},
        {"<=", OPERATOR},   {";", OPERATOR},    {")", OPERATOR},  {"(", OPERATOR}};
} // namespace lexer_types

namespace parser_types {

enum COLUMN_TYPE { STRING, INTEGER, FLOATING };

struct Predicate {
    std::string col = "";
    std::string op = "";
    std::string value = "";
};

struct SELECT_AST {
    std::vector<std::string> cols_name;
    std::string table_name;
    Predicate predicate;
};
struct INSERT_AST {
    std::vector<std::string> cols_name;
    std::string table_name;
    std::vector<access_methods_types::row_t> values;
};

using ASTResult = std::variant<parser_types::SELECT_AST, parser_types::INSERT_AST>;

const std::unordered_map<std::string, COLUMN_TYPE> columns = {{"x", INTEGER}, {"y", INTEGER}};
const std::unordered_set<std::string> table = {"test"};

} // namespace parser_types

namespace planner {} // namespace planner

#endif
