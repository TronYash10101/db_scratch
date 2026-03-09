#ifndef LEXER
#define LEXER

#include "types.hpp"

namespace lexer {

std::vector<lexer_types::Token> lexer(const std::string &query);

void whitespace_split(const std::string &input, std::vector<lexer_types::Token> &result_arr);

bool check_seperator(std::string seperator);

} // namespace lexer

#endif
