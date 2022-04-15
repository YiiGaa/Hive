#include "kernel/common/tools/tools.hpp"

void common::CTools::InitStruct(void * param, int size){
    memset(param, 0, size);
    return; 
}

int common::CTools::ReadJson(const char *path, Json::Value &json){
    Json::Reader jsonReader;
    std::ifstream jsonFile(path);//open file example.json

    if(!jsonReader.parse(jsonFile, json)){
        printf("\n\n***TOOLS Error:missing %s\n\n\n", path);
        jsonFile.close();
        return -1;
    }
    jsonFile.close();

    return 0; 
}

std::vector<std::string> common::CTools::StringSplit(std::string str, std::string pattern){
    std::string::size_type pos;
    std::vector<std::string> result;
    str+=pattern;
    int size=str.size();
    
    for(int i=0; i<size; i++){
        pos=str.find(pattern,i);
        if(pos<size){
        std::string s=str.substr(i,pos-i);
        result.push_back(s);
        i=pos+pattern.size()-1;
        }
    }
    return result;
}