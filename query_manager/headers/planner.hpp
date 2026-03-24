#ifndef PLANNER
#define PLANNER

#include "../../catalog_manager/headers/schmea_manager.hpp"
#include "../../storage_manager/headers/access_methods.hpp"
#include "../../storage_manager/headers/insert.hpp"
#include "../../storage_manager/headers/types.hpp"
#include "types.hpp"
#include <atomic>
#include <cstring>
#include <optional>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace planner {

class Operator {
  protected:
    schema::tables_attrs table_name;

  public:
    Operator(schema::tables_attrs &tn) : table_name(std::move(tn)) {}
    virtual std::optional<access_methods_types::row_t> next() = 0;
    virtual void init() = 0;
    virtual void close() = 0;
};

class Seq_scan : public Operator {
  protected:
    access_methods::Access_methods &am;
    buffer_manager::buffer_pool &buff_pool;
    access_methods::Access_methods::heap_scan heap_scan;
    std::vector<size_t> &data_size_arr;
    std::vector<access_methods_types::SUPORTED_COLUMN_TYPE> &col_types;
    // specifies this is the last op

  public:
    Seq_scan(schema::tables_attrs &tn, access_methods::Access_methods &access_methods, buffer_manager::buffer_pool &buff_pool,
             std::vector<size_t> &data_size_arr, std::vector<access_methods_types::SUPORTED_COLUMN_TYPE> &col_types)
        : am(access_methods), buff_pool(buff_pool), heap_scan(buff_pool), data_size_arr(data_size_arr), col_types(col_types), Operator(tn) {
    }

    void init() override {}

    std::optional<access_methods_types::row_t> next() override {
        // calls heap scan for next row, and return

        std::optional<access_methods_types::row_t> res = heap_scan.scan(data_size_arr, col_types);
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
            ret_sarg.col = predicate.col;
            ret_sarg.constant = std::stoi(predicate.value);
            return ret_sarg;
        } catch (...) {
            throw;
        }
    };
    access_methods_types::SARG to_match;
    std::optional<bool> match_sarg(std::optional<access_methods_types::row_t> res_row, size_t row_no) {
        switch (to_match.op) {
        case access_methods_types::EQ:
            return (to_match.constant == res_row.value().row[row_no]);
        case access_methods_types::GT:
            return (to_match.constant < res_row.value().row[row_no]);
        case access_methods_types::GTE:
            return (to_match.constant <= res_row.value().row[row_no]);
        case access_methods_types::LS:
            return (to_match.constant > res_row.value().row[row_no]);
        case access_methods_types::LSE:
            return (to_match.constant >= res_row.value().row[row_no]);
        }
        return std::nullopt;
    }
    Operator &next_op;

  public:
    Filter(schema::tables_attrs &tn, Operator &op, parser_types::Predicate &predicate) : next_op(op), predicate(predicate), Operator(tn){};
    void init() override { to_match = parse_predicate(); };

    std::optional<access_methods_types::row_t> next() override {
        /* Checks SARGs */
        std::optional<access_methods_types::row_t> res_row = this->next_op.next();
        size_t search_column;

        for (int i = 0; i < table_name.columns.size(); i++) {
            if (table_name.columns[i].column_name == to_match.col) {
                search_column = i;
            }
        }

        while (res_row.has_value()) {
            if (match_sarg(res_row.value(), search_column)) {
                return res_row;
            }
            res_row = this->next_op.next();
        }
        return std::nullopt;
    };

    void close() override {}
};

class Projection : public Operator {
  protected:
    Operator &next_op;
    parser_types::SELECT_AST &ast;

  public:
    Projection(schema::tables_attrs tn, Operator &op, parser_types::SELECT_AST &ast) : next_op(op), ast(ast), Operator(tn) {}

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

class Insert : public Operator {
  private:
    Operator *next_op;
    parser_types::INSERT_AST &ast;
    access_methods::Access_methods &am;
    buffer_manager::buffer_pool &buff_pool;
    index_write::root_struct &curr_root;
    std::atomic<int> curr_row = 0;

  public:
    Insert(schema::tables_attrs &tn, Operator *next_op, parser_types::INSERT_AST &ast, access_methods::Access_methods &am,
           buffer_manager::buffer_pool &buff_pool, index_write::root_struct &curr_root)
        : next_op(next_op), ast(ast), am(am), buff_pool(buff_pool), curr_root(curr_root), Operator(tn){};

    void init() override {}

    std::optional<access_methods_types::row_t> next() override {
        access_methods_types::row_t inserted_row;
        if (curr_row < ast.values.size()) {
            if (auto v = insert::create_entry(buff_pool, am, ast.values[curr_row], &curr_root)) {
                curr_root.root_pid = v.value();
            }
            curr_row++;
            return inserted_row;
        }
        return std::nullopt;
    }
    void close() override {}
};

std::vector<access_methods_types::row_t> select_plan(std::vector<std::unique_ptr<Operator>> &operators,
                                                     buffer_manager::buffer_pool &buff_pool, access_methods::Access_methods &access_methods,
                                                     schema::schema_manager &sch_man, parser_types::SELECT_AST &ast,
                                                     std::string *schema_name);

std::vector<access_methods_types::row_t> insert_plan(std::vector<std::unique_ptr<Operator>> &operators,
                                                     buffer_manager::buffer_pool &buff_pool, access_methods::Access_methods &access_methods,
                                                     schema::schema_manager &sch_man, parser_types::INSERT_AST &ast,
                                                     index_write::root_struct &curr_root, std::string *schema_name);

}; // namespace planner

#endif
