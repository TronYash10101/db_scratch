#include "headers/parser.hpp"
#include "headers/lexer.hpp"
#include "headers/types.hpp"
#include <stdexcept>
#include <string>
#include <vector>

/* Token Iterator Definitions */

parser::token_iterator::token_iterator(const std::string &query) : curr_idx(0) { tokens = lexer::lexer(query); };

lexer_types::Token parser::token_iterator::get_next() {
    /* Increaments internal index maintained for tokens, and return a token */
    if (curr_idx < tokens.size()) {
        return tokens[curr_idx++];
    } else {
        throw std::runtime_error("TOKENS OUT OF BOUNDS (POSITIVE)");
    }
}
lexer_types::Token parser::token_iterator::get_prev() {

    /* Does not Decreament internal index maintained for tokens, and return a token */
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

lexer_types::Token parser::token_iterator::peek(int idx) {
    if (idx < tokens.size() && idx >= 0) {
        return tokens[curr_idx + idx];
    }
    throw std::runtime_error("COULD NOT PEEK, INVALID INDEX");
}

/* Parser Definitions */

std::string parser::Parser::parse_from_clause(parser::token_iterator &tok_it) {
    lexer_types::Token sub_token = tok_it.get_next();
    if (parser_types::table.find(sub_token.token_value) != parser_types::table.end()) {
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
parser_types::SELECT_AST parser::Parser::parse_select_clause(parser::token_iterator &tok_it) {
    std::vector<std::string> returned_columns;
    parser_types::SELECT_AST ast;

    while (tok_it.has_next()) {
        lexer_types::Token sub_tok = tok_it.get_next();
        if (sub_tok.token_value == "FROM") {
            ast.table_name = parse_from_clause(tok_it);
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
                    for (auto const &[key, value] : parser_types::columns) {
                        returned_columns.push_back(key);
                    }
                    ast.cols_name = returned_columns;
                    break;
                } else if (parser_types::columns.find(sub_tok.token_value) == parser_types::columns.end()) {
                    throw std::runtime_error("NO SUCH COLUMN PRESENT");
                }

                if (sub_tok.token_type == lexer_types::IDENT) {
                    returned_columns.push_back(sub_tok.token_value);
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

void parser::Parser::parse_into_clause(parser::token_iterator &tok_it, parser_types::INSERT_AST &ast) {
    lexer_types::Token sub_tok = tok_it.get_next();

    // Support columns

    if (sub_tok.token_type == lexer_types::IDENT) {
        auto table_match = parser_types::table.find(sub_tok.token_value);
        if (table_match != parser_types::table.end()) {
            ast.table_name = sub_tok.token_value;
        } else {
            throw std::runtime_error("NO SUCH TABLE FOUND");
        }
    } else {
        throw std::runtime_error("EXPECTED A TABLE NAME");
    }

    sub_tok = tok_it.get_next();

    if (sub_tok.token_value == "(" && sub_tok.token_type == lexer_types::OPERATOR) {
        sub_tok = tok_it.get_next();
        while (sub_tok.token_value != ")") {
            if (sub_tok.token_type == lexer_types::IDENT) {
                ast.cols_name.push_back(sub_tok.token_value);
            }
            sub_tok = tok_it.get_next();
        }
    }
}

void parser::Parser::parse_value_clause(parser::token_iterator &tok_it, parser_types::INSERT_AST &ast) {
    lexer_types::Token sub_tok = tok_it.get_next();
    if (sub_tok.token_value == "(") {
        sub_tok = tok_it.get_next();
        while (sub_tok.token_value != ")") {
            if (sub_tok.token_type == lexer_types::IDENT) {
                ast.values.push_back(sub_tok.token_type);
            }
            sub_tok = tok_it.get_next();
        }
    } else {
        throw std::runtime_error("EXPECTED ( FOR VALUE");
    }
}

parser_types::INSERT_AST parser::Parser::parse_insert_clause(parser::token_iterator &tok_it) {
    parser_types::INSERT_AST ast;
    while (tok_it.has_next()) {
        lexer_types::Token sub_tok = tok_it.get_next();

        if (sub_tok.token_value == "INTO" && sub_tok.token_type == lexer_types::CLAUSE) {
            parse_into_clause(tok_it, ast);
        } else {
            throw std::runtime_error("EXPECTED INTO KEYWORD AFTER INSERT");
        }

        if (sub_tok.token_value == "VALUES" && sub_tok.token_type == lexer_types::CLAUSE) {
            parse_value_clause(tok_it, ast);
        } else {
            throw std::runtime_error("EXPECTED VALUES KEYWORD AFTER INTO");
        }
    }
    return ast;
}

parser_types::ASTResult parser::Parser::grammer_check(parser::token_iterator &tok_it) {

    lexer_types::Token first_tok = tok_it.get_next();

    if (first_tok.token_value == "SELECT") {
        return parse_select_clause(tok_it);
    }
    if (first_tok.token_value == "INSERT") {
        return parse_insert_clause(tok_it);
    }

    throw std::runtime_error("COULD NOT CHECK GRAMMER");
}
