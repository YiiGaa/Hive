#include "kernel/think/executeorder/executeorder.hpp"
#include "kernel/common/errorcode/errorcode.hpp"
#include "kernel/common/logger/logger.hpp"
#include "kernel/common/tools/tools.hpp"
#include "kernel/think/interface.hpp"
#include <stdio.h>
#include <map>

/* The same function as "kernel/think/think.hpp" */
#define START_MODULE(module, action, moduleParam, passParam, resultParam)  (thinkMap[module])->Start((int)action, (void*)&moduleParam, passParam, resultParam)
std::map<std::string, think::CInterface*> thinkMap = {
    {"executeorder", new think::Cexecuteorder()},    //Need to add "kernel/think/think.cpp" manually
};
/* The same function as "kernel/think/think.hpp" */

void testProgress(Json::Value param){
    Json::StyledWriter swriter;
    std::cout << swriter.write(param) << std::endl;
}

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

    //STEP executeorder
    think::Cexecuteorder::ModuleParam_t moduleParam_1;
    TOOLS_INITSTRUCT(moduleParam_1);

    moduleParam_1.targetKey         = "target";
    moduleParam_1.progressCallBack  = testProgress;

    ERROR_CODE ret = ERR_OK;
    ret = START_MODULE("executeorder", think::Cexecuteorder::NORMAL, moduleParam_1, passParam, returnParam);

    std::cout << "errorCode:" << CErrorCode::getErrorCode(ret) << ",message:" << CErrorCode::getErrorMessage(ret) << std::endl;
    
    return 0;
}
