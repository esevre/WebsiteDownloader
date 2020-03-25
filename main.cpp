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
                const std::string &content,
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
        //auto result = ES::web::get_html(url.authority(), url.path_to_page(), "80");
        if (verbose){
            std::cout << "writing page: " << filename << std::endl;
        }
        std::filesystem::create_directories(full_path);
        std::ofstream out(filename);
        out << content;
    } catch (std::exception &e) {
        std::cout << "failed to get: " << url.whole_url() << '\n';
        std::cout << "error: " << e.what() << std::endl;
    }
}

struct Crawler {
public:
    Crawler(const std::string &url_string)
            : base_url (url_string)
            , top_domain (base_url.top_domain())
            , second_domain (base_url.second_level_domain())
    {
        add_url(base_url);
    }

    Crawler(const std::string &url_string, const std::string &file_base)
            : base_url (url_string)
            , top_domain (base_url.top_domain())
            , second_domain (base_url.second_level_domain())
            , base_path(file_base)
    {
        add_url(base_url);
    }

    void add_url(const ES::web::ParsedUrl &url)
    {
        if (is_new_url(url)) {
            urls_to_process.push_back(url);
        }
    }
protected:
    bool is_new_url(const ES::web::ParsedUrl &url) const
    {
        if (!domain_check(url)) { return false; }
        if (url_in_vec(urls_to_process, url)) { return false; }
        if (url_in_vec(urls_processed, url))  { return false; }
        return true;
    }

    bool domain_check(const ES::web::ParsedUrl &url) const
    {
        return url.authority().empty() || url.top_domain() == top_domain && url.second_level_domain() == second_domain;
    }

    bool url_in_vec(const std::vector<ES::web::ParsedUrl> &url_vec, const ES::web::ParsedUrl &url) const
    {
        return std::any_of(
                url_vec.begin(),
                url_vec.end(),
                [&url](auto &u)
                {
                    return u.top_domain() == url.top_domain()
                           &&
                           u.second_level_domain() == url.second_level_domain()
                           &&
                           u.subdirectory() == url.subdirectory()
                           &&
                           u.page() == url.page();
                });
    }
    std::string get_html(const ES::web::ParsedUrl &url) const
    {
        std::cout << "connecting to: " << base_url.authority() << '\n';
        std::cout << "page: " << url.path_to_page() << std::endl;
        try {
            if (url.authority().empty()) {
                return ES::web::get_html(base_url.authority(), url.path_to_page());
            }
            return ES::web::get_html(base_url.authority(), url.path_to_page());
        } catch (std::exception &e) {
            throw e;
        }
    }
    std::vector<ES::web::ParsedUrl> gen_urls(const std::string &contents)
    {
        std::vector<std::string> tag_list = ES::web::get_tags_with(contents, "href");
        std::vector<ES::web::ParsedUrl> urls_in_page;
        urls_in_page.reserve(tag_list.size());

        for (auto &tag : tag_list)
        {
            auto tag_url = ES::web::extract_id_from_tag(tag, "href");
            auto blarg = ES::web::ParsedUrl(tag_url);
            urls_in_page.push_back(blarg);
        }
        return urls_in_page;
    }

public:
    void run()
    {
        while (!urls_to_process.empty()) {
            try {
                auto url = urls_to_process.back();
                urls_to_process.pop_back();
                urls_processed.push_back(url);

                ES::web::test_url(url);

                // read URL content
                std::string page_contents = get_html(url);

                // pull links from page and add to urls to process
                auto urls_from_page = gen_urls(page_contents);
                for (auto & found : urls_from_page) {
                    add_url(found);
                }

                // write page to file
                write_html(url, page_contents, base_path, true);
            } catch (std::exception &e) {
                std::cout << "failed to grab site due to: " << e.what() << std::endl;
            }
        }
    }
private:
    ES::web::ParsedUrl base_url;
    std::string top_domain;
    std::string second_domain;
    std::string base_path;
    std::string current_page;
    std::vector<ES::web::ParsedUrl> urls_to_process;
    std::vector<ES::web::ParsedUrl> urls_processed;
};



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

    Crawler site_crawler(base_site, base_dir);
    site_crawler.run();

//    auto result = ES::web::get_html(base_site, "/", "80");
//
//    std::cout << "************************************************\n";
//
//    std::vector<std::string> tag_list = ES::web::get_tags_with(result, "href");
//    std::vector<ES::web::ParsedUrl> url_to_visit;
//    std::cout << "Found " << tag_list.size() << " tags\n";
//
//    for (auto &tag : tag_list)
//    {
//        auto tag_url = ES::web::extract_id_from_tag(tag, "href");
//        auto blarg = ES::web::ParsedUrl(tag_url);
//        if (blarg.authority().empty()) {
//            blarg.authority(base_site);
//        }
//        url_to_visit.push_back(blarg);
//    }

//    std::cout << "*********************************************************\n";
//    std::cout << "**                 TESTING URL OBJECTS                 **\n";
//    std::cout << "*********************************************************\n";
//    for (const auto &url : url_to_visit) {
//        std::cout << "*********************************************************\n";
//        ES::web::test_url(url);
//        std::cout << "*********************************************************\n";
//    }

//    for (auto &url : url_to_visit){
//        if (url.second_level_domain() == "interridge") {
//            write_html(url, "../Site", true);
//        }
//    }

}

