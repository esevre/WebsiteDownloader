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


void write_html(const ES::web::ParsedUrl &url,
                const std::string &base_path="",
                const bool verbose = false)
{
    try {
        // create a filename:
        std::string full_path;
        if (!base_path.empty() && base_path.back() != '/'){
            full_path = base_path + "/" + url.subdirectory();
        } else {
            full_path = base_path + url.subdirectory();
        }
        std::string filename;
        if (url.page().empty()) {
            filename = full_path + "index.html";
        } else {
            filename = full_path + url.page();
        }

        if (verbose){
            std::cout << "downloading page: " << url.whole_url() << std::endl;
        }
        auto result = ES::web::get_html(url.authority(), url.path_to_page(), "80");
        if (verbose){
            std::cout << "writing page: " << filename << std::endl;
        }
        std::filesystem::create_directories(full_path);
        std::ofstream out(filename);
        out << result;
    } catch (std::exception &e) {
        std::cout << "failed to get: " << url.whole_url() << '\n';
        std::cout << "error: " << e.what() << std::endl;
    }
}



int main(int argc, char *argv[]) {
    if (argc < 2) {
        return 1;
    }
    std::string base_site = std::string(argv[1]);
    std::string base_dir = "Site";
    if (argc > 2) {
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
        auto blarg = ES::web::ParsedUrl(tag_url);
        if (blarg.authority().empty()) {
            blarg.authority(base_site);
        }
        url_to_visit.push_back(blarg);
    }

//    std::cout << "*********************************************************\n";
//    std::cout << "**                 TESTING URL OBJECTS                 **\n";
//    std::cout << "*********************************************************\n";
//    for (const auto &url : url_to_visit) {
//        std::cout << "*********************************************************\n";
//        ES::web::test_url(url);
//        std::cout << "*********************************************************\n";
//    }

    for (auto &url : url_to_visit){
        if (url.second_level_domain() == "interridge") {
            write_html(url, "../Site", true);
        }
    }
}

