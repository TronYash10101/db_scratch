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
    /* Increaments internal index maintained for tokens, and return a token */
    lexer_types::Token get_next();
    /* idx : How many positions to peek in tokens relative to curr_idx*/
    lexer_types::Token peek(int idx);
    /* Does not Decreament internal index maintained for tokens, and return a token */
    lexer_types::Token get_prev();
    bool has_next();
};

class Parser {
  private:
    std::string parse_from_clause(parser::token_iterator &tok_it);
    parser_types::Predicate parse_where_clause(parser::token_iterator &tok_it);
    void parse_into_clause(parser::token_iterator &tok_it, parser_types::INSERT_AST &ast);
    void parse_value_clause(parser::token_iterator &tok_it, parser_types::INSERT_AST &ast);
    parser_types::INSERT_AST parse_insert_clause(parser::token_iterator &tok_it);
    parser_types::SELECT_AST parse_select_clause(parser::token_iterator &tok_it);

  public:
    parser_types::ASTResult grammer_check(parser::token_iterator &tok_it);
};

} // namespace parser

#endif
