#include "kernel/think/redis/redis.hpp"
#include "kernel/common/errorcode/errorcode.hpp"
#include "kernel/common/logger/logger.hpp"
#include "kernel/common/tools/tools.hpp"
#include "kernel/think/interface.hpp"
#include <stdio.h>
#include <map>

/* The same function as "kernel/think/think.hpp" */
#define START_MODULE(module, action, moduleParam, passParam, resultParam)  (thinkMap[module])->Start((int)action, (void*)&moduleParam, passParam, resultParam)
std::map<std::string, think::CInterface*> thinkMap = {
    {"redis", new think::Credis()},    //Need to add "kernel/think/think.cpp" manually
};
/* The same function as "kernel/think/think.hpp" */

using namespace think;
int main(int argc, char *argv[]){
    Json::Value passParam;
    Json::Value returnParam;

    if(argc < 1){
        printf("\n\nplease enter: XXXXXX XXXXXX XXXXXX\n\n\n");
        return 0;
    }

    //STEP filling passParam
    Json::Reader passParamReader;
    std::ifstream passParamJson("../passParam.json");//open file example.json

    if(!passParamReader.parse(passParamJson, passParam)){
        printf("\n\n***Error:missing ../passParam.json\n\n\n");
        passParamJson.close();
        return -2;
    }
    passParamJson.close();

    //STEP redis
    think::Credis::ModuleParam_t moduleParam_1;
    moduleParam_1.connectAdress = "192.168.1.100:6383";//CONFIG_REDISADRESS;    //or set by yourself
    moduleParam_1.connectPwd = CONFIG_REDISPWD;         //or set by yourself

    std::string field;
    if(passParam.isMember("field")){
        field = passParam["field"].asString();
        moduleParam_1.field = (char*)field.c_str();
    }

    std::string key;
    if(passParam.isMember("key")){
        key = passParam["key"].asString();
        moduleParam_1.key = (char*)key.c_str();
    }

    std::string value;
    if(passParam.isMember("value")){
        value = passParam["value"].asString();
        moduleParam_1.value = (char*)value.c_str();
    }

    ERROR_CODE ret = START_MODULE("redis", think::Credis::ONCE, moduleParam_1, passParam, returnParam);

    //STEP get errorCode
    std::cout << "errorCode:" << CErrorCode::getErrorCode(ret) << ",message:" << CErrorCode::getErrorMessage(ret) << std::endl;
    
    return 0;
}
