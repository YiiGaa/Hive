#include "kernel/move/missionprogress/missionprogress.hpp"
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
    {"missionprogress", new move::Cmissionprogress()},   //Need to add "kernel/move/move.cpp" manually
};
/* The same function as "kernel/move/move.hpp" */

void *SamplePushmissionThread(void *arg) {
    Json::Value missionParam;
    missionParam["key"] = "testing";
    TRANSFERSTATION_PUSHMISSION("uuidTesting", missionParam, 0);
}

void *SampleUpdateprogressThread(void *arg) {
    TRANSFERSTATION_GETMISSION();
    Json::Value progressParam;
    int i = 0;
    while(1){
        progressParam["progress"] = i++;
        TRANSFERSTATION_UPDATEPROGRESS(progressParam);
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
    pthread_t pthreadPushmissionId;
    pthread_t pthreadUpdateProgressId;
    pthread_create(&pthreadPushmissionId, NULL, SamplePushmissionThread, NULL);
    pthread_create(&pthreadUpdateProgressId, NULL, SampleUpdateprogressThread, NULL);

    //STEP start missionprogress
    ERROR_CODE ret = START_ACTION("missionprogress", targetParam);
    
    //STEP kill mission sample thread
    pthread_cancel(pthreadPushmissionId);
    pthread_cancel(pthreadUpdateProgressId);
    pthread_join(pthreadPushmissionId, NULL);
    pthread_join(pthreadUpdateProgressId, NULL);

    //STEP get errorCode
    std::cout << "errorCode:" << CErrorCode::getErrorCode(ret) << ",message:" << CErrorCode::getErrorMessage(ret) << std::endl;

    return 0;
}
