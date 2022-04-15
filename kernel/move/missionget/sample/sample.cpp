#include "kernel/move/missionget/missionget.hpp"
#include "kernel/common/errorcode/errorcode.hpp"
#include "kernel/common/logger/logger.hpp"
#include "kernel/common/tools/tools.hpp"
#include "kernel/move/interfaces.hpp"
#include <stdio.h>
#include <map>

#include <unistd.h>

/* The same function as "kernel/move/move.hpp" */
#define START_ACTION(module, moduleParam)  (moveMap[module])->Start(moduleParam)
std::map<std::string, move::CInterfaces*> moveMap = {
    {"missionget", new move::Cmissionget()},   //Need to add "kernel/move/move.cpp" manually
};
/* The same function as "kernel/move/move.hpp" */

void *SampleGetMissionThread(void *arg) {
    Json::Value missionParam;
    Json::Value returnParam;
    Json::Reader reader;
    Json::FastWriter swriter;
    ERROR_CODE ret = ERR_OK;
    int i = 0;
    while(1){
        missionParam = TRANSFERSTATION_GETMISSION();
        std::cout << swriter.write(missionParam) << std::endl;
        sleep(240);

        const char* result = "{\"test\":\"test\"}";
        reader.parse(result, returnParam);
        TRANSFERSTATION_FINISHMISSION(ret, returnParam);
        
    }
} 

using namespace move;
int main(int argc, char *argv[]){
    if(argc < 1){
        printf("\n\nplease enter: XXXXXX XXXXXX XXXXXX\n\n\n");
        return 0;
    }

    //STEP filling passParam
    Json::Reader passParamReader;
    Json::Value targetParam;
    std::ifstream passParamJson("../sample.json");//open file example.json

    if(!passParamReader.parse(passParamJson, targetParam)){
        printf("\n\n***Error:missing %s\n\n", "../sample.json");
        passParamJson.close();
        return -2;
    }
    passParamJson.close();

    //STEP create mission sample thread
    pthread_t pthreadGetmissionId;
    pthread_create(&pthreadGetmissionId, NULL, SampleGetMissionThread, NULL);

    //STEP start missionget
    ERROR_CODE ret = START_ACTION("missionget", targetParam);
    
    //STEP kill mission sample thread
    pthread_cancel(pthreadGetmissionId);
    pthread_join(pthreadGetmissionId, NULL);

    //STEP get errorCode
    std::cout << "errorCode:" << CErrorCode::getErrorCode(ret) << ",message:" << CErrorCode::getErrorMessage(ret) << std::endl;
    
    return 0;
}
