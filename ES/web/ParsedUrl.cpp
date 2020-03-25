//
// Created by esevre on 3/12/2020.
//

#include "ParsedUrl.hpp"

#include <algorithm>
#include <iostream>

namespace ES::web {
    ParsedUrl::ParsedUrl(const std::string &full_url)
            : m_whole_url(full_url)
    {
        // First extract the query and fragment from the URL if present
        //    This is easiest to do first, at a relatively low computational cost
        std::string url;
        std::tie(url, m_query, m_fragment) = split_root_query_fragment(full_url);

        auto url_iterator = url.begin();

        if (scheme_exists()) {
            auto scheme_pos = std::search(url.begin(), url.end(),scheme_delimiter.begin(), scheme_delimiter.end());

            auto scheme_size = std::distance(url.begin(), scheme_pos);
            m_scheme.resize(scheme_size);
            std::copy(url.begin(), url_iterator, m_scheme.begin());
            url_iterator = scheme_pos; // move the URL to the scheme position
            std::advance(url_iterator, scheme_delimiter.size());
        }

        if (authority_exists()) {
            url_iterator = std::search(url_iterator, url.end(), authority_delimiter.begin(), authority_delimiter.end());
            std::advance(url_iterator, authority_delimiter.size());

            //
            // populate the domain vector with all domain levels
            //
            auto domain_end = std::search(url_iterator, url.end(), path_delimiter.begin(), path_delimiter.end());
            auto count_dots = std::count(url_iterator, domain_end, '.');
            // there is one more domain entry than the number of dots
            authority_vector.reserve(count_dots + 1);

            for (int i = 0; i < count_dots + 1; ++i) {
                auto dot_pos = std::find(url_iterator+1, domain_end, '.');
                authority_vector.push_back(it2string(url_iterator, dot_pos));
                url_iterator = dot_pos;
                // only advance if not at end of domain section
                if (i < count_dots) {
                    std::advance(url_iterator, 1);
                }
            }
        }

        // some URLs end after the domains, check if that is the case
        if ((url_iterator == url.end()) || (url_iterator+1 == url.end()))
        {
            return;
        }
        // if the URL ends with '/' then move past it.
        if ( *url_iterator == '/')
        {
            std::advance(url_iterator, 1);
        }


        auto divider_count = std::count(url_iterator, url.end(), '/');
        if (url.back() == '/') { --divider_count; }
        directory_vector.reserve(divider_count + 1);

        for (int i = 0; i < divider_count + 1; ++i) {
            auto div_pos = std::find(url_iterator, url.end(), '/');
            directory_vector.push_back(it2string(url_iterator, div_pos));
            url_iterator = div_pos;
            if (i < divider_count) {
                std::advance(url_iterator, 1);
            }
        }
        page_defined = std::any_of(directory_vector.back().begin(), directory_vector.back().end(), [](auto c){return c=='.';});
        if (page_defined) {
            m_page = directory_vector.back();
            directory_vector.pop_back();
        }
    }

    std::string ParsedUrl::whole_url() const
    {
        return m_whole_url;
    }

    std::string ParsedUrl::scheme() const
    {
        return m_scheme;
    }

    std::string ParsedUrl::subdomain() const
    {
        if (authority_vector.size() > 2) {
            std::string subdomain = authority_vector[0];
            for (int i = 1; i < authority_vector.size() - 2; ++i) {
                subdomain = subdomain + "." + authority_vector[i];
            }
            return subdomain;
        }
        return std::string();
    }

    std::string ParsedUrl::second_level_domain() const
    {
        if (authority_vector.size() > 1) {
            size_t domain_position = authority_vector.size() - 2;
            return authority_vector[domain_position];
        }
        return std::string();
    }

    std::string ParsedUrl::top_domain() const
    {
        if (authority_vector.empty()){
            return std::string();
        }
        return authority_vector.back();
    }

    std::string ParsedUrl::subdirectory() const
    {
        return join_with(directory_vector, "/") + "/";
    }

    std::string ParsedUrl::page() const
    {
        return m_page;
    }

    std::string ParsedUrl::query() const
    {
        return m_query;
    }

    std::string ParsedUrl::fragment() const
    {
        return m_fragment;
    }

    std::string ParsedUrl::authority() const
    {
        return join_with(authority_vector, ".");
    }

    void ParsedUrl::authority(const std::string &updated_authority)
    {
        authority_vector = split_with(updated_authority, '.');
    }

    std::string ParsedUrl::path_to_page() const
    {
        // if the subdirectory is empty, prefix the page with forward slash
        return join_with(directory_vector, "/") + "/" + m_page;
    }

    std::string ParsedUrl::join_with(const std::vector<std::string> &vec, const std::string &seperator) const
    {
        if (vec.empty()) { return std::string(); }
        if (vec.size() == 1) { return vec[0]; }

        std::string merged_string;

        merged_string = vec[0];
        for (int i = 1; i < vec.size() - 1; ++i) {
            merged_string += seperator + vec[i];
        }
        merged_string += seperator + vec.back();

        return merged_string;
    }

    std::vector<std::string> ParsedUrl::split_with(const std::string &string, const char &seperator) const
    {
        auto url_iterator = string.begin();
        auto divider_count = std::count(url_iterator, string.end(), seperator);
        std::vector<std::string> split_vector;
        if (string.back() == seperator) { --divider_count; }
        split_vector.reserve(divider_count + 1);

        for (int i = 0; i < divider_count + 1; ++i) {
            auto div_pos = std::find(url_iterator, string.end(), seperator);
            split_vector.push_back(it2string(url_iterator, div_pos));
            url_iterator = div_pos;
            if (i < divider_count) {
                std::advance(url_iterator, 1);
            }
        }
        return split_vector;
    }

    std::tuple<std::string, std::string, std::string> ParsedUrl::split_root_query_fragment(const std::string &full_url) const
    {
        std::string root_url = full_url;
        std::string query;
        std::string fragment;

        auto begin_pos = full_url.begin();
        auto query_pos = std::find(full_url.begin(), full_url.end(), '?');
        auto fragment_pos = std::find(full_url.begin(), full_url.end(), '#');
        auto end_pos = full_url.end();

        if (query_pos == end_pos) {
            // only check for fragment, also works for no fragment
            if (fragment_pos != end_pos) {
                root_url = it2string(begin_pos, fragment_pos);
                fragment = it2string(fragment_pos+1, end_pos);
            }
        } else {
            if (fragment_pos != end_pos) {
                root_url = it2string(begin_pos, query_pos);
                query = it2string(query_pos+1, fragment_pos);
                fragment = it2string(fragment_pos+1, end_pos);
            } else {
                root_url = it2string(begin_pos, query_pos);
                query = it2string(query_pos+1, fragment_pos);
            }
        }

        return std::make_tuple(root_url, query, fragment);
    }

    bool ParsedUrl::scheme_exists() const
    {
        auto begin_pos = m_whole_url.begin();
        auto scheme_pos = std::search(m_whole_url.begin(), m_whole_url.end(), scheme_delimiter.begin(), scheme_delimiter.end());
        if (scheme_pos == m_whole_url.end()) { return false; }

        return std::all_of(begin_pos, scheme_pos, std::isalpha);
    }

    bool ParsedUrl::authority_exists() const
    {
        auto begin_pos = m_whole_url.begin();
        auto authority_pos = std::search(m_whole_url.begin(), m_whole_url.end(), authority_delimiter.begin(), authority_delimiter.end());

        return authority_pos != m_whole_url.end();
    }


    void test_url(const ParsedUrl &url)
    {
        std::cout << "URL:  " << url.whole_url() << '\n';
        std::cout << "scheme            : \'" << url.scheme() << "\'\n";
        std::cout << "subdomain         : \'" << url.subdomain() << "\'\n";
        std::cout << "2nd level domain  : \'" << url.second_level_domain() << "\'\n";
        std::cout << "top domain        : \'" << url.top_domain() << "\'\n";
        std::cout << "subdirectory      : \'" << url.subdirectory() << "\'\n";
        std::cout << "page              : \'" << url.page() << "\'\n";
        std::cout << "query             : \'" << url.query() << "\'\n";
        std::cout << "fragment          : \'" << url.fragment() << "\'\n";
        std::cout << "authority         : \'" << url.authority() << "\'\n";
        std::cout << "path to page      : \'" << url.path_to_page() << "\'\n" << std::flush;
    }
}






