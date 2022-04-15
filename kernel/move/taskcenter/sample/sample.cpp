#include "kernel/move/taskcenter/taskcenter.hpp"
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
    {"taskcenter", new move::Ctaskcenter()},   //Need to add "kernel/move/move.cpp" manually
};
/* The same function as "kernel/move/move.hpp" */
Json::Value targetParam;

void *SamplePushMissionThread(void *arg) {
    Json::StyledWriter swriter;

    std::cout << "@@mission start: " << swriter.write(targetParam) << std::endl;
    TRANSFERSTATION_PUSHMISSION("mission id" ,targetParam);

    Json::Value returnResult;
    returnResult = TRANSFERSTATION_POPMISSISON();
    std::cout << "@@mission result: " << swriter.write(returnResult) << std::endl;
}

void *SampleProgressThread(void *arg) {
    Json::StyledWriter swriter;
    while(1){
        Json::Value progress = TRANSFERSTATION_GETPROGRESS();
        common::CTransferStation::STATE state = TRANSFERSTATION_GETMISSIONSTATE();

        std::cout << "**state: " << TRANSFERSTATION_TRANSSTATE(state) << std::endl;
        std::cout << "**progress: " << swriter.write(progress) << std::endl;

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
    std::ifstream passParamJson("../sample.json");//open file example.json

    if(!passParamReader.parse(passParamJson, targetParam)){
        printf("\n\n***Error:missing %s\n\n", "../sample.json");
        passParamJson.close();
        return -2;
    }
    passParamJson.close();

    //STEP create mission sample thread
    pthread_t pthreadPushmissionId;
    pthread_create(&pthreadPushmissionId, NULL, SamplePushMissionThread, NULL);
    pthread_t pthreadGetmissionId;
    pthread_create(&pthreadGetmissionId, NULL, SampleProgressThread, NULL);

    //STEP start taskcenter
    ERROR_CODE ret = START_ACTION("taskcenter", targetParam);

    //STEP kill mission sample thread
    pthread_cancel(pthreadPushmissionId);
    pthread_join(pthreadPushmissionId, NULL);

    //STEP get errorCode
    std::cout << "errorCode:" << CErrorCode::getErrorCode(ret) << ",message:" << CErrorCode::getErrorMessage(ret) << std::endl;
    
    return 0;
}
