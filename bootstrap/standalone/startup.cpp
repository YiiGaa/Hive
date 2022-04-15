#include "kernel/move/missionget/missionget.hpp"
#include <stdio.h>
#include "kernel/move/move.hpp"
#include "kernel/common/logger/logger.hpp"
#include "kernel/common/tools/tools.hpp"
#include <pthread.h>
#include <vector>

Json::Value propertiestParam;
std::vector<std::string> moveList;

int allStartIndex = 0;
pthread_mutex_t allStartMutex = PTHREAD_MUTEX_INITIALIZER; 
pthread_cond_t allStartCond = PTHREAD_COND_INITIALIZER;

void *MoveStart(void *arg) {
    int key = 0;
    pthread_mutex_lock(&allStartMutex);
        key = allStartIndex;
        allStartIndex++;
        std::cout<< "create move thread: " << moveList[key] << std::endl;
    pthread_mutex_unlock(&allStartMutex);

    START_ACTION(moveList[key], propertiestParam);

    pthread_cond_signal(&allStartCond);

    pthread_exit((void *)0);
} 

int main(int argc, char *argv[]){
    std::vector<pthread_t> pthreadList;
    pthread_t mainPthread;
    //STEP filling properties
    // Json::Reader passParamReader;
    // std::ifstream passParamJson("../properties/testing.json");//open file example.json

    // if(!passParamReader.parse(passParamJson, propertiestParam)){
    //     printf("\n\n***Error:missing %s\n\n", "../properties/testing.json");
    //     passParamJson.close();
    //     return -2;
    // }
    // passParamJson.close();

    //STEP filling properties
    if(argc < 2){
        printf("\n\nplease enter: propertiestParam string \n\n\n");
        return -1;
    }
    Json::Reader passParamReader;
    if(!passParamReader.parse(argv[1], propertiestParam)){
        printf("\n\n***Error:missing %s\n\n", "passParam");
        return -2;
    }

    //STEP setting logging
    if(propertiestParam.isMember("common") && propertiestParam["common"].isMember("identity")){
        common::CLogger::identity = propertiestParam["common"]["identity"].asString();
        if(propertiestParam["common"].isMember("hostIp")){
            common::CLogger::hostIp = propertiestParam["common"]["hostIp"].asString();
        }
    }

    //STEP Ergodic structure
    Json::Value::Members members = propertiestParam.getMemberNames();  
    int num = 0;
    int mainIndex = 0;
    for (auto iter = members.begin(); iter != members.end(); iter++) {      
		std::string key = (std::string)*iter;
		
        //when not equal to common
        if(key.compare("common")){
            //Check whether moveList include
            if(move::CMove::moveMap.find(key) == move::CMove::moveMap.end()){
                std::cout<<"warning:not find "<< key << " in move List." << std::endl;
                continue;
            }

            pthread_t tempPthreadId;
            moveList.push_back(key);
            pthread_create(&tempPthreadId, NULL, MoveStart, NULL);

            if(propertiestParam[key].isMember("isMain")){
                if(propertiestParam[key]["isMain"].asInt()){  //markDown main thread index
                    mainIndex = num;
                    mainPthread = tempPthreadId;
                }
            }

            num++;
            pthreadList.push_back(tempPthreadId);
        }
    }

    //Join thread
    //pthread_join(pthreadList[mainIndex], NULL);
    pthread_mutex_lock(&allStartMutex);
       pthread_cond_wait(&allStartCond, &allStartMutex);
    pthread_mutex_unlock(&allStartMutex);
    
    //wait end time
    if(propertiestParam.isMember("common") && propertiestParam["common"].isMember("endWaitTime")){
        int waitEndTime = propertiestParam["common"]["endWaitTime"].asInt();
        sleep(waitEndTime);
    }   

    for(int i=0; i<pthreadList.size(); i++){
        pthread_t tempPthread = pthreadList[i];

        pthread_cancel(tempPthread);
        pthread_join(tempPthread, NULL);
    }

    CLEARLOGGER();
    
    return 0;
}