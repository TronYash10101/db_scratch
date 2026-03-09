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
    lexer_types::Token peek();
    bool has_next();
};
std::vector<std::string> check(parser::token_iterator tok_it);
} // namespace parser

#endif
