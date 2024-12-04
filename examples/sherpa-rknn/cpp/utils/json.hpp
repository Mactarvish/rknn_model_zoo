//
// Created by mck on 24-5-26.
//

#include <jsoncpp/json/json.h>
#include <fstream>

inline bool ReadJson(const std::string &srcJsonPath, Json::Value &r)
{
    r.clear();
    std::ifstream inFile(srcJsonPath.data(), std::ios::binary);
    if (!inFile.good())
    {
        return false;
    }
    Json::Reader reader;
    return reader.parse(inFile, r);
}

