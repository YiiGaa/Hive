#include "kernel/think/httppost/httppost.hpp"
#include "kernel/common/errorcode/errorcode.hpp"
#include "kernel/common/logger/logger.hpp"
#include "kernel/common/tools/tools.hpp"
#include "kernel/think/interface.hpp"
#include <stdio.h>
#include <map>

/* The same function as "kernel/think/think.hpp" */
#define START_MODULE(module, action, moduleParam, passParam, resultParam)  (thinkMap[module])->Start((int)action, (void*)&moduleParam, passParam, resultParam)
std::map<std::string, think::CInterface*> thinkMap = {
    {"httppost", new think::Chttppost()},    //Need to add "kernel/think/think.cpp" manually
};
/* The same function as "kernel/think/think.hpp" */

using namespace think;
int main(int argc, char *argv[]){
    Json::Value passParam;
    Json::Value returnParam;

    if(argc < 2){
        printf("\n\nplease enter: XXXXXX XXXXXX XXXXXX\n\n\n");
        return 0;
    }

    //STEP filling passParam
    Json::Reader passParamReader;
    Json::Value tempParam;
    std::ifstream passParamJson("../sample_list.json");//open file example.json

    if(!passParamReader.parse(passParamJson, tempParam)){
        printf("\n\n***Error:missing %s\n\n", "../sample_list.json");
        passParamJson.close();
        return -2;
    }
    passParamJson.close();

    if(!tempParam.isMember(argv[1])){
        printf("\n\n***Error:missing %s in sample_list.json\n\n", argv[1]);
        return -3;
    }
    Json::Value targetParam = tempParam[argv[1]];

    //STEP httppost
    think::Chttppost::ModuleParam_t moduleParam_1;
    TOOLS_INITSTRUCT(moduleParam_1);

    std::string requestMethod;
    if(targetParam.isMember("requestMethod")){
        requestMethod = targetParam["requestMethod"].asString();
        moduleParam_1.requestMethod = (char*)requestMethod.c_str();
    }

    std::string url;
    if(targetParam.isMember("url")){
        url = targetParam["url"].asString();
        moduleParam_1.url = (char*)url.c_str();
    }

    std::string messageBody;
    if(targetParam.isMember("messageBody")){
        messageBody = targetParam["messageBody"].asString();
        moduleParam_1.messageBody = (char*)messageBody.c_str();
    }

    START_MODULE("httppost", think::Chttppost::NONE , moduleParam_1, passParam, returnParam);
    
    return 0;
}
