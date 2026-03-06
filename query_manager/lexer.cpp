#include "headers/lexer.hpp"
#include <vector>

bool lexer::check_seperator(char seperator) {
    // add more later
    if (seperator == ',') {
        return true;
    } else {
        return false;
    }
}

void lexer::whitespace_split(const std::string &input, std::vector<Token> &result_arr) {
    int count = 0;
    std::string curr_word;
    for (char c : input) {
        std::string char_str(1, c);
        if (check_seperator(c) && lexer_table.find(char_str) != lexer_table.end()) {
            auto it = lexer::lexer_table.find(char_str);
            result_arr.push_back({it->second, char_str});
            curr_word.clear();
            continue;
        } else if (c == ' ') {
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
