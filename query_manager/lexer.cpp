#include "headers/lexer.hpp"
#include <stdexcept>
#include <vector>

bool lexer::check_seperator(std::string seperator) {
    // add more later
    if (seperator == "," || seperator == ">" || seperator == "=" || seperator == "<" || seperator == "*" || seperator == ";" ||
        seperator == "(" || seperator == ")") {
        return true;
    } else {
        return false;
    }
}

void lexer::whitespace_split(const std::string &input, std::vector<lexer_types::Token> &result_arr) {
    std::string curr_word;
    for (int i = 0; i < input.size(); i++) {
        std::string char_str(1, input[i]);
        if (check_seperator(char_str) && lexer_types::lexer_table.find(char_str) != lexer_types::lexer_table.end()) {
            if (!curr_word.empty())
                result_arr.push_back({lexer_types::IDENT, curr_word});
            curr_word.clear();

            curr_word.push_back(input[i]);
            i++;
            while (i < input.size() && check_seperator(std::string(1, input[i])) &&
                   (lexer_types::lexer_table.find(std::string(1, input[i])) != lexer_types::lexer_table.end())) {
                curr_word.push_back(input[i]);
                i++;
            }
            auto combined_op = lexer_types::lexer_table.find(curr_word);
            if (combined_op != lexer_types::lexer_table.end()) {
                result_arr.push_back({lexer_types::OPERATOR, curr_word});
                curr_word.clear();
                i--;
            }
            continue;
        } else if (input[i] == ' ' && !curr_word.empty()) {
            auto it = lexer_types::lexer_table.find(curr_word);
            if (it != lexer_types::lexer_table.end()) {
                result_arr.push_back({it->second, curr_word});
            } else {
                result_arr.push_back({lexer_types::IDENT, curr_word});
            }
            curr_word.clear();
            continue;
        }
        if (input[i] != ' ') {
            // Insert only when there are no special characters like " "
            curr_word.push_back(input[i]);
        }
    }

    if (!curr_word.empty()) {
        auto it = lexer_types::lexer_table.find(curr_word);
        if (it != lexer_types::lexer_table.end()) {
            result_arr.push_back({it->second, curr_word});
        } else {
            result_arr.push_back({lexer_types::IDENT, curr_word});
        }
    }
}

std::vector<lexer_types::Token> lexer::lexer(const std::string &query) {
    std::vector<lexer_types::Token> tokens;
    whitespace_split(query, tokens);
    return tokens;
}
