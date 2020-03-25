//
// Created by esevre on 3/10/2020.
//

#include "parse_string_to.hpp"

namespace ES::string{
    std::string parse_string_to(
            const std::string &base,
            const std::string &substring)
    {
        // notice if not found, return entire string since substring_pos = base.end()
        auto substring_pos = std::search(base.begin(), base.end(), substring.begin(), substring.end());

        std::string selection;
        auto pos = base.begin();
        size_t selection_size = std::distance(pos, substring_pos);

        for (int i = 0; i < selection_size; ++i, ++pos) {
            selection.push_back(*pos);
        }
        
        return selection;
    }
}


