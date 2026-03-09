#include "headers/parser.hpp"
#include "headers/lexer.hpp"
#include <stdexcept>
#include <string>
#include <vector>

parser::token_iterator::token_iterator(const std::string &query) : curr_idx(0) { tokens = lexer::lexer(query); };

lexer_types::Token parser::token_iterator::get_next() {

    if (curr_idx < tokens.size()) {
        return tokens[curr_idx++];
    } else {
        throw std::runtime_error("TOKENS OUT OF BOUNDS");
    }
}

bool parser::token_iterator::has_next() {
    if (curr_idx != tokens.size() - 1) {
        return true;
    }
    return false;
}

std::vector<std::string> parser::check(parser::token_iterator tok_it) {
    while (tok_it.has_next()) {
        lexer_types::Token tok = tok_it.get_next();

        if (tok.token_value == "SELECT" && tok.token_type == lexer_types::CLAUSE) {
            std::vector<std::string> returned_columns;
            lexer_types::Token sub_tok = tok_it.get_next();

            while (sub_tok.token_type == lexer_types::IDENT || sub_tok.token_type == lexer_types::OPERATOR) {
                if (sub_tok.token_value == "," && sub_tok.token_type == lexer_types::OPERATOR) {
                    sub_tok = tok_it.get_next();
                    continue;
                }

                if (sub_tok.token_value == "*") {
                    return std::vector<std::string>(parser_types::columns.begin(), parser_types::columns.end());
                } else if (parser_types::columns.find(sub_tok.token_value) == parser_types::columns.end()) {
                    throw std::runtime_error("NO SUCH COLUMN PRESENT");
                }

                if (sub_tok.token_type == lexer_types::IDENT)
                    returned_columns.push_back(sub_tok.token_value);

                sub_tok = tok_it.get_next();
            }
            return returned_columns;
        }
    }
    return std::vector<std::string>();
}
