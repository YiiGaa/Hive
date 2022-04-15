#ifndef   _TOOLS_HPP_
#define   _TOOLS_HPP_ 

#define TOOLS_INITSTRUCT(param) common::CTools::InitStruct((void*)&param, sizeof(param))
#define TOOLS_READJSON(path, json) common::CTools::ReadJson((const char*)path, json)
#define TOOLS_STRINGSPLIT(str, pattern) common::CTools::StringSplit(str, pattern)

#include <string.h>
#include <vector>
#include <fstream>
#include <jsoncpp/json/json.h>

namespace common{
    class CTools {
        public:
            static void InitStruct(void *, int);
            static int ReadJson(const char *, Json::Value &);
            static std::vector<std::string> StringSplit(std::string str, std::string pattern);
    };
}

#endif