//
// Created by esevre on 3/25/2020.
//

#include "tag_functions.hpp"

namespace ES::web
{
    std::vector<std::string> get_tags_with(const std::string &full_content, const std::string &tag_id)
    {
        //  <[^/].*?href.*?>
        std::string beg_open_tag_regex = "<[^/]";
        std::string end_open_tag_regex = ">";
        std::string href_exists_regex = "[^<>]*?" + tag_id + ".*?";

        std::regex html_tag_regex(beg_open_tag_regex + href_exists_regex + end_open_tag_regex,
                                  std::regex_constants::ECMAScript | std::regex_constants::icase);
        auto tags_begin =
                std::sregex_iterator(full_content.begin(), full_content.end(), html_tag_regex);
        auto tags_end = std::sregex_iterator();

        std::vector<std::string> tag_vector(std::distance(tags_begin, tags_end));

        std::transform(tags_begin, tags_end,
                       tag_vector.begin(),
                       [](const auto &tag)
                       {
                           return tag.str();
                       });

        return tag_vector;
    }

    std::string extract_id_from_tag(const std::string &tag_string, const std::string &tag_id)
    {
        //  href=""
        //  href=".*?"
        std::string regex_tag_squote = "href=\'.*?\'";
        std::string regex_tag_dquote = "href=\".*?\"";

        std::regex html_tag_regex_squote(regex_tag_squote);
        std::regex html_tag_regex_dquote(regex_tag_dquote);

        auto tags_begin =
                std::sregex_iterator(tag_string.begin(), tag_string.end(), html_tag_regex_squote);
        auto tags_end = std::sregex_iterator();

        if (std::distance(tags_begin, tags_end) < 1)
        {
            tags_begin =
                    std::sregex_iterator(tag_string.begin(), tag_string.end(), html_tag_regex_dquote);
        }

        //    std::string content;
        // need to know length of tag, such as href="
        //    here the length is 4 + 2 (add two for =" characters
        // the length of the content will be the length of the tagline, minus the start minus the end
        const auto tag_offset = tag_id.size() + 2;
        const auto tag_content_length = tags_begin->str().size() - tag_offset - 1;
        return tags_begin->str().substr(tag_offset, tag_content_length);
    }
}