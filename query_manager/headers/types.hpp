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

struct SCHEMA_AST {
    std::string schmea_name;
};

struct ALTER_TABLE_AST {
    std::string table_name;
    std::string column_name;
    access_methods_types::SUPORTED_COLUMN_TYPE column_type;
};

struct CREATE_TABLE_AST {
  private:
    struct column_attr {
        std::string column_name;
        access_methods_types::SUPORTED_COLUMN_TYPE column_type;
    };

  public:
    std::string table_name;

    std::vector<column_attr> columns;
};

using ASTResult = std::variant<SELECT_AST, INSERT_AST, SCHEMA_AST, CREATE_TABLE_AST, ALTER_TABLE_AST>;

// const std::unordered_map<std::string, COLUMN_TYPE> columns = {{"x", INTEGER}, {"y", INTEGER}};
// const std::unordered_set<std::string> table = {"test"};

} // namespace parser_types

namespace planner {} // namespace planner

#endif
