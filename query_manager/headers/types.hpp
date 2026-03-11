#ifndef QUERY_TYPES
#define QUERY_TYPES

#include "../../storage_manager/headers/access_methods.hpp"
#include "../../storage_manager/headers/types.hpp"
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

namespace planner {

enum NODE_TYPE { FILTER, SEQ_SCAN, INDEX_SCAN, PROJECTION };

class Operator {
  protected:
    NODE_TYPE next_node;

  public:
    virtual heap_page_types::RID next() = 0;
    virtual void init() = 0;
    virtual void close() = 0;
    virtual void main();
};

class Seq_scan : public Operator {
  protected:
    std::vector<heap_page_types::RID> seq_outputs;
    std::vector<access_methods_types::SARG> seq_inputs;
    access_methods::Access_methods &am;
    buffer_manager::buffer_pool &buff_pool;

  public:
    Seq_scan(access_methods::Access_methods &access_methods, buffer_manager::buffer_pool &buff_pool)
        : am(access_methods), buff_pool(buff_pool) {}

    void init() override {}

    heap_page_types::RID next() override {
        // calls heap scan for next row

        std::optional res = am.heap_scan(buff_pool, *seq_inputs.end());
        if (res.has_value()) {
            seq_outputs.push_back(res.value());
        }
        throw std::runtime_error("NO ROW");
    }

    void close() override {
        seq_outputs.clear();
        seq_inputs.clear();
    }
};

class Filter : public Seq_scan {
  protected:
    std::vector<parser_types::Predicate> filter_inputs;
    access_methods_types::SARG parse_predicate(parser_types::Predicate &predicate) {
        access_methods_types::SARG ret_sarg;
        try {
            if (predicate.op == "==") {
                ret_sarg.op = access_methods_types::EQ;
            } else if (predicate.op == ">=") {
                ret_sarg.op = access_methods_types::GTE;
            } else if (predicate.op == "<=") {
                ret_sarg.op = access_methods_types::LSE;
            } else if (predicate.op == ">") {
                ret_sarg.op = access_methods_types::GT;
            } else if (predicate.op == "<") {
                ret_sarg.op = access_methods_types::LS;
            }

            if (predicate.col == "x") {
                ret_sarg.col = access_methods_types::X;
            } else {
                ret_sarg.col = access_methods_types::Y;
            };
            ret_sarg.constant = std::stoi(predicate.value);
            return ret_sarg;
        } catch (...) {
            throw;
        }
    };

  public:
    Filter();
    void init() override { this->next_node = SEQ_SCAN; };

    heap_page_types::RID next() override {
        parser_types::Predicate predicate = *filter_inputs.end();
        access_methods_types::SARG sarg = parse_predicate(predicate);
        seq_inputs.push_back(sarg);

        heap_page_types::RID res_rid = Seq_scan::next();

        return res_rid;
    };

    void close() override { filter_inputs.clear(); }
};

class Projection : public Filter {
  protected:
  public:
    heap_page_types::RID next() override {}
};

} // namespace planner

#endif
