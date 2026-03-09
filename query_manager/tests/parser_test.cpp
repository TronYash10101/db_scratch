#include "../headers/parser.hpp"
#include <iostream>
#include <vector>

int main() {
    parser::token_iterator tik("SELECT name, FROM T");

    std::vector<std::string> res = parser::check(tik);

    for (const std::string &ele : res) {
        std::cout << ele << " ";
    }
}
