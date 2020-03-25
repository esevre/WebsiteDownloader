//
// Created by esevre on 3/9/2020.
//

#include "string_starts_with.hpp"

namespace ES::string {
    bool string_starts_with(const std::string &full_string, const std::string &start_string)
    {
        if (full_string.size() < start_string.size()) { return false; }
        size_t i = 0;
        for (const auto &start_char : start_string) {
            if (start_char != full_string[i++]){
                return false;
            }
        }
        return true;
    }
}



