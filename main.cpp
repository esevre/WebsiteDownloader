// If Windows, assume windows 10
#ifdef WIN32
// Define windows 10 ID Code
#define _WIN32_WINNT 0x0A00
#endif

#include <algorithm>
#include <iostream>
#include <regex>
#include <string>

#include <vector>


#include "ES/web/get_html.hpp"
#include "ES/web/tag_functions.hpp"
#include "ES/web/ParsedUrl.hpp"





int main(int argc, char *argv[]) {
    if (argc < 2) {
        return 1;
    }
    std::string base_site = std::string(argv[1]);
    auto result = ES::web::get_html(base_site, "/", "80");

    std::cout << "************************************************\n";

    std::vector<std::string> tag_list = ES::web::get_tags_with(result, "href");

    std::vector<ES::web::ParsedUrl> url_to_visit;

    std::cout << "Found " << tag_list.size() << " tags\n";

    for (auto &tag : tag_list)
    {
        auto tag_url = ES::web::extract_id_from_tag(tag, "href");
//        std::cout << "extract tag: " << tag << '\n';
//        std::cout << "grab test: " << tag_url << '\n';
        auto blarg = ES::web::ParsedUrl(tag_url);
        if (blarg.authority().empty()) {
            blarg.authority(base_site);
        }
        url_to_visit.push_back(blarg);
    }

    std::cout << "*********************************************************\n";
    std::cout << "**                 TESTING URL OBJECTS                 **\n";
    std::cout << "*********************************************************\n";
    for (const auto &url : url_to_visit) {
        std::cout << "*********************************************************\n";
        ES::web::test_url(url);
        std::cout << "*********************************************************\n";
    }

}

