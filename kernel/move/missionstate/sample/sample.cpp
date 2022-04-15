#include "kernel/move/missionstate/missionstate.hpp"
#include "kernel/common/errorcode/errorcode.hpp"
#include "kernel/common/logger/logger.hpp"
#include "kernel/common/tools/tools.hpp"
#include "kernel/move/interfaces.hpp"
#include <stdio.h>
#include <map>

#include <pthread.h>
#include <unistd.h>

/* The same function as "kernel/move/move.hpp" */
#define START_ACTION(module, moduleParam)  (moveMap[module])->Start(moduleParam)
std::map<std::string, move::CInterfaces*> moveMap = {
    {"missionstate", new move::Cmissionstate()},   //Need to add "kernel/move/move.cpp" manually
};
/* The same function as "kernel/move/move.hpp" */

void *SampleMissionThread(void *arg) {
    std::string stateStr;
    while(1){
        common::CTransferStation::STATE state = TRANSFERSTATION_GETMISSIONSTATE();
        std::cout << "mission state:" << TRANSFERSTATION_TRANSSTATE(state) << std::endl;
        state = TRANSFERSTATION_GETSTATE();
        std::cout << "state:" << TRANSFERSTATION_TRANSSTATE(state) << std::endl;
        
        sleep(1);
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
    pthread_t pthreadMissionId;
    pthread_create(&pthreadMissionId, NULL, SampleMissionThread, NULL);

    //STEP start missionstate
    ERROR_CODE ret = START_ACTION("missionstate", targetParam);

    pthread_cancel(pthreadMissionId);
    pthread_join(pthreadMissionId, NULL);

    //STEP get errorCode
    std::cout << "errorCode:" << CErrorCode::getErrorCode(ret) << ",message:" << CErrorCode::getErrorMessage(ret) << std::endl;
    
    return 0;
}
