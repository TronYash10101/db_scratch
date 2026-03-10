#include "headers/parser.hpp"
#include "headers/lexer.hpp"
#include "headers/types.hpp"
#include <stdexcept>
#include <string>
#include <vector>

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

std::vector<std::string> parser::parse_select_clause(parser::token_iterator &tok_it) {
    std::vector<std::string> returned_columns;
    lexer_types::Token sub_tok = tok_it.get_next();

    while (1) {
        if (sub_tok.token_value == "," && sub_tok.token_type == lexer_types::OPERATOR) {
            lexer_types::Token next = tok_it.get_next();
            if (next.token_type != lexer_types::IDENT && !(next.token_value == "*" && next.token_type == lexer_types::OPERATOR)) {
                throw std::runtime_error("EXPECTED IDENTIFIER AFTER ',' but found " + next.token_value);
            }
            sub_tok = next;
            continue;
        } else if (sub_tok.token_value == "*") {
            return std::vector<std::string>(parser_types::columns.begin(), parser_types::columns.end());
        } /* else if (parser_types::columns.find(sub_tok.token_value) == parser_types::columns.end()) {
            throw std::runtime_error("NO SUCH COLUMN PRESENT");
        } */

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
    return returned_columns;
}

parser_types::Predicate parse_where_clause(parser::token_iterator &tok_it) {
    parser_types::Predicate res;

    lexer_types::Token col = tok_it.get_next();
    lexer_types::Token op = tok_it.get_next();
    lexer_types::Token val = tok_it.get_next();

    res.col = col.token_value;
    res.op = op.token_value;
    res.value = val.token_value;

    return res;
}

std::string parse_from_clause(parser::token_iterator &tok_it) {
    lexer_types::Token sub_token = tok_it.get_next();
    std::cout << "here " << sub_token.token_value;
    if (parser_types::table.find(sub_token.token_value) != parser_types::table.end()) {
        return sub_token.token_value;
    }
    throw std::runtime_error("NO SUCH TABLE EXISTS");
}

parser_types::AST parser::grammer_check(parser::token_iterator &tok_it) {
    parser_types::AST res_ast;

    while (tok_it.has_next()) {
        lexer_types::Token tok = tok_it.get_next();

        if (tok.token_value == "SELECT") {
            std::cout << "es";
            res_ast.cols_name = parse_select_clause(tok_it);
        }

        if (tok.token_value == "FROM") {
            std::cout << "ef";
            res_ast.table_name = parse_from_clause(tok_it);
        }

        if (tok.token_value == "WHERE") {
            std::cout << "ew";
            res_ast.predicate = parse_where_clause(tok_it);
        }
    }
    return res_ast;
}
