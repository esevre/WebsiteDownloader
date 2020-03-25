//
// Created by esevre on 2/29/2020.
//

#pragma once

#include <fstream>
#include <string>

namespace ES::file{
    void file_write(std::string filename, std::string contents)
    {
        std::ofstream outfile(filename);
        outfile << contents;
    }

}








