#include <iostream>
#include <fstream>
#include <sstream>
#include <file_util.hpp>


std::string FileUtil::readTextFromFile(std::string filename)
{
    std::ifstream inFile;
    inFile.open(filename.c_str(), std::ios::in);
    std::stringstream strStream;
    strStream << inFile.rdbuf();
    std::string text = strStream.str();

    inFile.close();
    return text;
}
