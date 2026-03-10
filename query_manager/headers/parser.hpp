#ifndef PARSER
#define PARSER

#include "types.hpp"

namespace parser {

class token_iterator {
  private:
    std::vector<lexer_types::Token> tokens;
    int curr_idx = 0;

  public:
    token_iterator(const std::string &query);
    lexer_types::Token get_next();
    /* idx : How many positions to peek in tokens relative to curr_idx*/
    lexer_types::Token peek(int idx);
    lexer_types::Token get_prev();
    bool has_next();
};

std::vector<std::string> parse_select_clause(parser::token_iterator &tok_it);
parser_types::AST grammer_check(parser::token_iterator &tok_it);
} // namespace parser

#endif
