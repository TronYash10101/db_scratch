#ifndef PLANNER
#define PLANNER

#include "../../storage_manager/headers/access_methods.hpp"
#include "../../storage_manager/headers/types.hpp"
#include "types.hpp"
#include <iostream>
#include <optional>
#include <vector>

class Operator {
  protected:
  public:
    virtual std::optional<access_methods_types::row_t> next() = 0;
    virtual void init() = 0;
    virtual void close() = 0;
    virtual void main();
};

class Seq_scan : public Operator {
  protected:
    access_methods::Access_methods &am;
    buffer_manager::buffer_pool &buff_pool;
    access_methods::Access_methods::heap_scan heap_scan;
    Operator *next_op = NULL; // specifies this is the last op

  public:
    Seq_scan(access_methods::Access_methods &access_methods, buffer_manager::buffer_pool &buff_pool)
        : am(access_methods), buff_pool(buff_pool), heap_scan(buff_pool) {}

    void init() override {}

    std::optional<access_methods_types::row_t> next() override {
        // calls heap scan for next row, and return

        std::optional<access_methods_types::row_t> res = heap_scan.scan();
        if (res.has_value()) {
            return res;
        }
        return std::nullopt;
    }

    void close() override {}
};

class Filter : public Operator {
  protected:
    parser_types::Predicate &predicate;
    access_methods_types::SARG parse_predicate() {
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
    Operator &next_op;

  public:
    Filter(Operator &op, parser_types::Predicate &predicate) : next_op(op), predicate(predicate){};
    void init() override {};

    std::optional<access_methods_types::row_t> next() override {
        /* Checks SARGs */
        std::optional<access_methods_types::row_t> res_row = this->next_op.next();
        if (res_row.has_value()) {
            access_methods_types::SARG to_match = parse_predicate();
            to_match.match(res_row.value());
            return res_row;
        }
        return std::nullopt;
    };

    void close() override {}
};

class Projection : public Operator {
  protected:
    Operator &next_op;
    parser_types::AST &ast;

  public:
    Projection(Operator &op, parser_types::AST &ast) : next_op(op), ast(ast) {}

    void init() override {};
    std::optional<access_methods_types::row_t> next() override {
        // *input.end()->table_name internally used here, fetch this table check valid columns iterate
        std::optional<access_methods_types::row_t> res_row = this->next_op.next();
        if (res_row.has_value()) {
            /* for (const std::string &ele : ast.cols_name) {
                // Change to handle specific columns based on hash fnc
            } */
            return res_row;
        }
        return std::nullopt;
    }
    void close() override {}
};

#endif
