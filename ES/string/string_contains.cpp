//
// Created by esevre on 3/6/2020.
//

#include "string_contains.hpp"

#include <regex>

bool ES::string::string_contains(const std::string &source, const std::string &substring) {
    std::regex search_regex(substring);
    return std::regex_search(source, search_regex);
}


