#include "headers/lexer.hpp"
#include <vector>

void lexer::whitespace_split(const std::string &input, std::vector<Token> &result_arr) {
    int count = 0;
    std::string curr_word;
    for (char c : input) {
        if (c == ' ' || c == ',') {
            auto it = lexer::lexer_table.find(curr_word);
            if (it != lexer::lexer_table.end()) {
                result_arr.push_back({it->second, curr_word});
            } else {
                result_arr.push_back({IDENT, curr_word});
            }

            curr_word.clear();
            continue;
        }
        curr_word.push_back(c);
    }

    if (!curr_word.empty()) {
        auto it = lexer::lexer_table.find(curr_word);
        if (it != lexer::lexer_table.end()) {
            result_arr.push_back({it->second, curr_word});
        } else {
            result_arr.push_back({IDENT, curr_word});
        }
    }
}

std::vector<lexer::Token> lexer::_lexer(const std::string &query) {
    std::vector<lexer::Token> tokens;
    whitespace_split(query, tokens);
    return tokens;
}
