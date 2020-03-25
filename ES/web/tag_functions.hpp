//
// Created by esevre on 3/25/2020.
//

#pragma once

#include <regex>
#include <string>
#include <vector>

namespace ES::web {
    std::vector<std::string> get_tags_with(const std::string &full_content, const std::string &tag_id);
    std::string extract_id_from_tag(const std::string &tag_string, const std::string &tag_id);
}