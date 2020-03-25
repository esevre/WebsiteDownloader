//
// Created by esevre on 3/10/2020.
//

#include "to_lower.hpp"

std::string ES::string::to_lower(const std::string &string)
{
    std::string lowercase_string = string;
    for (auto &c : lowercase_string) {
        c = std::tolower(c);
    }
    return lowercase_string;
}
