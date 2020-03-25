//
// Created by esevre on 2/25/2020.
//

#pragma once

#include <string>
#include <sstream>

namespace ES::string
{
    template <class Streamable>
    std::string streamable_to_string(const Streamable &streamable)
    {
        std::stringstream ss;
        ss << streamable;
        return ss.str();
    }
}



