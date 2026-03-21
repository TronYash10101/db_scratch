#include "headers/parser.hpp"
#include "headers/lexer.hpp"
#include "headers/types.hpp"
#include <cstdlib>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>
#include <vector>

/* Token Iterator Definitions */

parser::token_iterator::token_iterator(const std::string &query) : curr_idx(0) { tokens = lexer::lexer(query); };

lexer_types::Token parser::token_iterator::get_next() {
    if (curr_idx < tokens.size()) {
        return tokens[curr_idx++];
    } else {
        throw std::runtime_error("TOKENS OUT OF BOUNDS (POSITIVE)");
    }
}
lexer_types::Token parser::token_iterator::get_prev() {

    if (curr_idx > 0) {
        return tokens[curr_idx - 1];
    } else {
        throw std::runtime_error("TOKENS OUT OF BOUNDS (NEGATIVE)");
    }
}

bool parser::token_iterator::has_next() {
    if (curr_idx < tokens.size()) {
        return true;
    }
    return false;
}

lexer_types::Token parser::token_iterator::peek(int idx) const {

    int pos = curr_idx + idx - 1;

    if (pos >= 0 && pos < tokens.size()) {
        return tokens[pos];
    }

    throw std::runtime_error("COULD NOT PEEK, INVALID INDEX");
}

/* Parser Definitions */

std::string parser::Parser::parse_from_clause(parser::token_iterator &tok_it, schema::schema_manager &schema_manager,
                                              std::string &schema_name) {
    lexer_types::Token sub_token = tok_it.get_next();
    std::optional<std::vector<schema::ENTITY_TYPE>> table_find =
            schema_manager.entity_find(schema::TABLE, sub_token.token_value, &schema_name);

    if (table_find.has_value()) {
        return sub_token.token_value;
    }
    throw std::runtime_error("NO SUCH TABLE EXISTS");
}

parser_types::Predicate parser::Parser::parse_where_clause(parser::token_iterator &tok_it) {
    parser_types::Predicate res;

    lexer_types::Token col = tok_it.get_next();
    lexer_types::Token op = tok_it.get_next();
    lexer_types::Token val = tok_it.get_next();

    res.col = col.token_value;
    res.op = op.token_value;
    res.value = val.token_value;

    return res;
}
parser_types::SELECT_AST parser::Parser::parse_select_clause(parser::token_iterator &tok_it, schema::schema_manager &schema_manager,
                                                             std::string &schema_name) {
    std::vector<std::string> returned_columns;
    parser_types::SELECT_AST ast;

    while (tok_it.has_next()) {
        lexer_types::Token sub_tok = tok_it.get_next();
        if (sub_tok.token_value == "FROM") {
            ast.table_name = parse_from_clause(tok_it, schema_manager, schema_name);
            continue;
        } else if (sub_tok.token_value == "WHERE") {
            ast.predicate = parse_where_clause(tok_it);
            continue;
        } else {
            while (1) {

                if (sub_tok.token_value == "," && sub_tok.token_type == lexer_types::OPERATOR) {
                    lexer_types::Token next = tok_it.get_next();
                    if (next.token_type != lexer_types::IDENT && !(next.token_value == "*" && next.token_type == lexer_types::OPERATOR)) {
                        throw std::runtime_error("EXPECTED IDENTIFIER AFTER ',' but found " + next.token_value);
                    }
                    sub_tok = next;
                    continue;
                } else if (sub_tok.token_value == "*") {
                    // table name not available
                    std::optional<std::vector<schema::ENTITY_TYPE>> columns =
                            schema_manager.entity_find(schema::COLUMN, sub_tok.token_value, &ast.table_name, &schema_name);
                    if (columns.has_value()) {
                        for (const schema::ENTITY_TYPE &ele : columns.value()) {
                            if (auto *ptr = std::get_if<schema::col_attrs>(&ele)) {
                                returned_columns.push_back(ptr->column_name);
                            }
                        }
                    }
                    ast.cols_name = returned_columns;
                    break;
                }

                if (sub_tok.token_type == lexer_types::IDENT) {
                    std::optional<std::vector<schema::ENTITY_TYPE>> column =
                            schema_manager.entity_find(schema::COLUMN, sub_tok.token_value, &ast.table_name, &schema_name);
                    if (column.has_value()) {
                        if (auto *found_col = std::get_if<schema::col_attrs>(&column.value()[0])) {
                            returned_columns.push_back(found_col->column_name);
                        }
                    } else {
                        throw std::runtime_error("NO SUCH COLUMN FOUND");
                    }

                } else {
                    throw std::runtime_error("EXPECTED IDENTIFIER AFTER" + tok_it.get_prev().token_value);
                }

                if (tok_it.peek(1).token_type != lexer_types::IDENT || tok_it.peek(1).token_type != lexer_types::OPERATOR) {
                    break;
                }
                sub_tok = tok_it.get_next();
            }
            ast.cols_name = returned_columns;
        }
    }
    return ast;
}

void parser::Parser::parse_into_clause(parser::token_iterator &tok_it, parser_types::INSERT_AST &ast,
                                       schema::schema_manager &schema_manager, std::string &schema_name) {
    lexer_types::Token sub_tok = tok_it.get_next();
    // Support columns

    if (sub_tok.token_type == lexer_types::IDENT) {
        std::optional<std::vector<schema::ENTITY_TYPE>> table_match =
                schema_manager.entity_find(schema::TABLE, sub_tok.token_value, &schema_name);

        if (table_match.has_value()) {
            ast.table_name = sub_tok.token_value;
        } else {
            throw std::runtime_error("NO SUCH TABLE FOUND");
        }
    } else {
        throw std::runtime_error("EXPECTED A TABLE NAME");
    }

    lexer_types::Token next_tok = tok_it.peek(1);

    if (next_tok.token_value == "(" && next_tok.token_type == lexer_types::OPERATOR) {
        sub_tok = tok_it.get_next();
        while (sub_tok.token_value != ")") {
            if (sub_tok.token_type == lexer_types::IDENT) {
                std::optional<std::vector<schema::ENTITY_TYPE>> find =
                        schema_manager.entity_find(schema::COLUMN, sub_tok.token_value, &ast.table_name, &schema_name);

                if (find.has_value()) {
                    ast.cols_name.push_back(sub_tok.token_value);
                } else {
                    throw std::runtime_error("NO SUCH COULMNS");
                }
            }
            sub_tok = tok_it.get_next();
        }
    }
}

void parser::Parser::parse_value_clause(parser::token_iterator &tok_it, parser_types::INSERT_AST &ast,
                                        schema::schema_manager &schema_manager, std::string &schema_name) {
    lexer_types::Token sub_tok = tok_it.get_next();

    if (sub_tok.token_value == "(") {
        parser_types::Row this_row;

        int i = 0;
        while (sub_tok.token_value != ")") {
            if (sub_tok.token_type == lexer_types::IDENT) {
                sub_tok = tok_it.get_next();
                std::optional<std::vector<schema::ENTITY_TYPE>> f =
                        schema_manager.entity_find(schema::COLUMN, ast.cols_name[i], &ast.table_name, &schema_name);

                if (f.has_value()) {
                    if (auto *pt = std::get_if<schema::col_attrs>(&f.value()[0])) {
                        if (pt->match(pt->column_type, sub_tok.token_value)) {
                            this_row.row.push_back(sub_tok.token_value);
                        } else {
                            throw std::runtime_error("VALUES TYPE DOES NOT MATCH COLUMN ORDER GIVEN");
                        }
                    }
                }
                i++;
            }
        }
        ast.values.push_back(this_row);
        // follows default schema order when not provided and now, but should also work for some order
    } else if (tok_it.get_prev().token_value == "(") {
        throw std::runtime_error("EXPECTED ( AFTER VALUES");
    }

    if (tok_it.has_next())
        parse_value_clause(tok_it, ast, schema_manager, schema_name);
}

parser_types::INSERT_AST parser::Parser::parse_insert_clause(parser::token_iterator &tok_it, schema::schema_manager &schema_manager,
                                                             std::string &schema_name) {
    parser_types::INSERT_AST ast;
    while (tok_it.has_next()) {
        lexer_types::Token sub_tok = tok_it.get_next();

        if (sub_tok.token_value == "INTO" && sub_tok.token_type == lexer_types::CLAUSE) {
            parse_into_clause(tok_it, ast, schema_manager, schema_name);
            continue;
        } else if (tok_it.get_prev().token_value == "INSERT") {
            throw std::runtime_error("EXPECTED INTO KEYWORD AFTER INSERT");
        }

        if (sub_tok.token_value == "VALUES" && sub_tok.token_type == lexer_types::CLAUSE) {
            parse_value_clause(tok_it, ast, schema_manager, schema_name);
            continue;
        } else if (tok_it.get_prev().token_value == ast.table_name) {
            throw std::runtime_error("EXPECTED VALUES KEYWORD AFTER INTO");
        }
    }
    return ast;
}

parser_types::ASTResult parser::Parser::grammer_check(parser::token_iterator &tok_it, schema::schema_manager &schema_manager,
                                                      std::string &schema_name) {

    lexer_types::Token first_tok = tok_it.get_next();

    if (first_tok.token_value == "SELECT") {
        return parse_select_clause(tok_it, schema_manager, schema_name);
    }
    if (first_tok.token_value == "INSERT") {
        return parse_insert_clause(tok_it, schema_manager, schema_name);
    }

    throw std::runtime_error("COULD NOT CHECK GRAMMER");
}
