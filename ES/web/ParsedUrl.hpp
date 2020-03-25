//
// Created by esevre on 3/12/2020.
//
#pragma once

#include <cctype>
#include <string>
#include <tuple>
#include <vector>

namespace ES::web {
// try this with more specific types to avoid possible naming
// conflicts in complex builds with multiple translation units
    template <class StringIt>
    std::string it2string(const StringIt beg, const StringIt end){
        std::string temp;
        temp.resize(std::distance(beg, end));
        std::copy(beg, end, temp.begin());
        return temp;
    }

/// Parse a URL from the entire domain
///   whole_url (the entire URL)        : http://www.eriksevre.com/projects/interridge.html
///   scheme (what protocol to use)     : http
///   subdomain (what part of the page) : www
///   second-level domain (main domain) : eriksevre
///   top-level (type of entity of URL) : com
///   subdirectory (subpage in site)    : projects
///   page (the specific page name)     : interridge.html
    class ParsedUrl {
    public:
        explicit ParsedUrl(const std::string &url);
        ParsedUrl(const ParsedUrl &) = default;

        // Getters and Setters
        std::string whole_url() const;
        std::string scheme() const;
        std::string subdomain() const;
        std::string second_level_domain() const;
        std::string top_domain() const;
        std::string subdirectory() const;
        std::string page() const;
        std::string query() const;
        std::string fragment() const;

        // Get useful combinations : like authority() --> www.eriksevre.com without http or page
        std::string authority() const;
        void authority(const std::string &updated_authority);
        std::string path_to_page() const;

    private:
        std::string join_with(const std::vector<std::string> &vec, const std::string &seperator) const;
        std::vector<std::string> split_with(const std::string &string, const char &seperator) const;
        std::tuple<std::string, std::string, std::string> split_root_query_fragment(const std::string &full_url) const;

        bool scheme_exists() const;
        bool authority_exists() const;

    private:
        const std::string scheme_delimiter = ":";
        const std::string authority_delimiter = "//";
        const std::string path_delimiter = "/";
//    const std::string user_info_delimiter = "@";
//    const std::string port_delimiter = ":";
//    const std::string query_delimiter = "?";
//    const std::string fragment_delimiter = "#";
//    const std::string authority_separator = ".";
//    const std::string path_separator = "/";

        std::string m_whole_url;
        std::string m_scheme;
        std::string m_page;
        std::string m_query;
        std::string m_fragment;
        std::vector<std::string> authority_vector;
        std::vector<std::string> directory_vector;
        bool page_defined = false;

    };

    void test_url(const ParsedUrl &url);

}







