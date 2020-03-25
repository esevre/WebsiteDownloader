// If Windows, assume windows 10
#ifdef WIN32
// Define windows 10 ID Code
#define _WIN32_WINNT 0x0A00
#endif

#include <iostream>
#include <string>
#include <fstream>

#include <vector>

#include <filesystem>

#include "ES/web/get_html.hpp"
#include "ES/web/tag_functions.hpp"
#include "ES/web/ParsedUrl.hpp"


void write_file(const std::string &content, const std::string &filename)
{
    std::ofstream outfile(filename);
    outfile << content;
}




int main(int argc, char *argv[]) {
    if (argc < 2) {
        return 1;
    }
    std::string base_site = std::string(argv[1]);
    std::string base_dir = "Site";
    if (argc < 3) {
        base_dir = argv[2];
    }
    std::cout << "base_dir:  " << base_dir << std::endl;

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

    std::filesystem::create_directories("../Site/path/here");
    std::ofstream out("../Site/path/here/thing.txt");
//    out << std::string("This is some stuff\nmore stuff");
    out << result;
    out.close();
}

