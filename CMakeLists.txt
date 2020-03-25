cmake_minimum_required(VERSION 3.15)
project(WebsiteDownloader)


set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED)


set(Boost_NO_SYSTEM_PATHS true)
set(Boost_USE_STATIC_LIBS OFF CACHE BOOL "use static libraries from Boost")
set(Boost_USE_MULITHREADED ON)

if (WIN32)
    # disable autolinking in boost
    add_definitions( -DBOOST_ALL_NO_LIB )

    # force all boost libraries to dynamic link
    # need this even though autolinking disabled above
    add_definitions( -DBOOST_ALL_DYN_LINK )

    # Windows definitions below not working, moved into code
    #    set(_WIN32_WINNT 0x0601) # Windows 7
    #    set(_WIN32_WINNT 0x0602) # Windows 8
    #    set(_WIN32_WINNT 0x0603) # Windows 8.1
    #    set(_WIN32_WINNT 0x0A00) # Windows 10
endif ()

include_directories(${Boost_INCLUDE_DIRS})
add_executable(WebsiteDownloader
        main.cpp
        ES/web/get_html.hpp
        ES/string/streamable.hpp
        ES/file/file_write.hpp
        ES/string/string_contains.cpp
        ES/string/string_contains.hpp
        ES/string/string_starts_with.cpp
        ES/string/string_starts_with.hpp
        ES/string/parse_string_to.cpp
        ES/string/parse_string_to.hpp
        ES/string/utilities.hpp
        ES/string/to_lower.cpp
        ES/string/to_lower.hpp
        ES/web/ParsedUrl.cpp
        ES/web/ParsedUrl.hpp
        ES/web/tag_functions.cpp
        ES/web/tag_functions.hpp
        )



