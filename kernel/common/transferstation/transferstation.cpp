#include "kernel/common/transferstation/transferstation.hpp"
#include "kernel/common/tools/tools.hpp"

#include <time.h>
#include <sys/time.h>

std::string common::CTransferStation::missionId[CONFIGTRANSFERSTATION_QUEUELEN];
common::CTransferStation::MissionParam_t common::CTransferStation::missionQueue[CONFIGTRANSFERSTATION_QUEUELEN] = {0};
int common::CTransferStation::missionReadPoint = 0;
int common::CTransferStation::missionWritePoint = 0;
int common::CTransferStation::hasRun = 0;
int common::CTransferStation::isEnd = 0;
pthread_mutex_t common::CTransferStation::mutexLock = PTHREAD_MUTEX_INITIALIZER; 
pthread_cond_t common::CTransferStation::condPush = PTHREAD_COND_INITIALIZER;
pthread_cond_t common::CTransferStation::condPop = PTHREAD_COND_INITIALIZER;

pthread_mutex_t common::CTransferStation::stateLock = PTHREAD_MUTEX_INITIALIZER; 
pthread_cond_t common::CTransferStation::condstate = PTHREAD_COND_INITIALIZER;

pthread_mutex_t common::CTransferStation::stateUpdateLock = PTHREAD_MUTEX_INITIALIZER; 
pthread_cond_t common::CTransferStation::condUpdatestate = PTHREAD_COND_INITIALIZER;

std::vector<Json::Value> common::CTransferStation::missionStateUpdate;

void common::CTransferStation::Init(){
    memset(common::CTransferStation::missionQueue, 0, sizeof(common::CTransferStation::missionQueue)*sizeof(MissionParam_t));
    common::CTransferStation::missionReadPoint = 0;
    common::CTransferStation::missionWritePoint = 0;
    return;
}

std::string common::CTransferStation::TransState(common::CTransferStation::STATE param){
    std::string stateStr;
    switch(param){
        case common::CTransferStation::WAIT:
            stateStr = "wait";
            break;
        case common::CTransferStation::ACCEPT:
            stateStr = "accept";
            break;
        case common::CTransferStation::START:
            stateStr = "start";
            break;
        case common::CTransferStation::WAIT_PAUSE:
            stateStr = "wait pause";
            break;
        case common::CTransferStation::PAUSE:
            stateStr = "pause";
            break;
        case common::CTransferStation::STOP:
            stateStr = "stop";
            break;
        case common::CTransferStation::COMPLETE:
            stateStr = "complete";
            break;
        default:
            stateStr = "unknown";
    }
    return stateStr;
}

int common::CTransferStation::PushMission(std::string id, Json::Value param, int timeoutSenconds = 0){
    //lock thread
    pthread_mutex_lock(&common::CTransferStation::mutexLock);

        int result = 0;
        if(id.compare(common::CTransferStation::missionId[common::CTransferStation::missionReadPoint])){
            MissionParam_t missionParam;
            
            //TOOLS_INITSTRUCT(missionParam);
            missionParam.state = ACCEPT;
            missionParam.mission = param;
            common::CTransferStation::missionId[common::CTransferStation::missionWritePoint] = id;

            (common::CTransferStation::missionQueue[common::CTransferStation::missionWritePoint]).state = ACCEPT;
            common::CTransferStation::missionQueue[common::CTransferStation::missionWritePoint] = missionParam;

            common::CTransferStation::missionWritePoint++;
            common::CTransferStation::missionWritePoint = (common::CTransferStation::missionWritePoint >= CONFIGTRANSFERSTATION_QUEUELEN) ? 0 : common::CTransferStation::missionWritePoint;
            common::CTransferStation::hasRun = 1;
            pthread_cond_signal(&common::CTransferStation::condPop);
        }

        if(common::CTransferStation::missionReadPoint != common::CTransferStation::missionWritePoint){
            if(timeoutSenconds == 0){
                pthread_cond_wait(&common::CTransferStation::condPush, &common::CTransferStation::mutexLock); 
            } else {
                struct timespec outTime;
                struct timeval now;
                gettimeofday(&now, NULL);
                outTime.tv_sec = now.tv_sec + timeoutSenconds;
                outTime.tv_nsec = now.tv_usec * 1000;
                pthread_cond_timedwait(&common::CTransferStation::condPush, &common::CTransferStation::mutexLock, &outTime);
            }
        }

        if(common::CTransferStation::missionReadPoint == common::CTransferStation::missionWritePoint){
            //mission finish
            result = -1;
        }

    //unlock
    pthread_mutex_unlock(&common::CTransferStation::mutexLock);

    return result;
}

Json::Value common::CTransferStation::GetMission(){
    //lock thread
    pthread_mutex_lock(&common::CTransferStation::mutexLock);

        Json::Value returnMission;
        if(common::CTransferStation::missionReadPoint == common::CTransferStation::missionWritePoint){
            pthread_cond_wait(&common::CTransferStation::condPop, &common::CTransferStation::mutexLock);
        }
        MissionParam_t mission = common::CTransferStation::missionQueue[common::CTransferStation::missionReadPoint];
        returnMission = mission.mission;

        //change state
        (common::CTransferStation::missionQueue[common::CTransferStation::missionReadPoint]).state = START;

        if(!missionStateUpdate.empty()){
            missionStateUpdate.clear();
        }

    //unlock
    pthread_mutex_unlock(&common::CTransferStation::mutexLock);

    return returnMission;
}

std::string common::CTransferStation::GetMissionId(){
    std::string missionId = "none";
    pthread_mutex_lock(&common::CTransferStation::mutexLock);
        if(common::CTransferStation::missionReadPoint == common::CTransferStation::missionWritePoint){
            missionId = "none";
        } else {
            missionId = common::CTransferStation::missionId[common::CTransferStation::missionReadPoint];
        }
    pthread_mutex_unlock(&common::CTransferStation::mutexLock);
    return missionId;
}

void common::CTransferStation::FinishMission(ERROR_CODE ret, Json::Value param){
    //lock thread
    pthread_mutex_lock(&common::CTransferStation::mutexLock);
        (common::CTransferStation::missionQueue[common::CTransferStation::missionReadPoint]).result = param;
        if((common::CTransferStation::missionQueue[common::CTransferStation::missionReadPoint]).state != STOP){
            (common::CTransferStation::missionQueue[common::CTransferStation::missionReadPoint]).state = COMPLETE;
        }

        common::CTransferStation::missionReadPoint++;
        common::CTransferStation::missionReadPoint = (common::CTransferStation::missionReadPoint >= CONFIGTRANSFERSTATION_QUEUELEN) ? 0 : common::CTransferStation::missionReadPoint;

        pthread_cond_signal(&common::CTransferStation::condPush);
        pthread_cond_wait(&common::CTransferStation::condPop, &common::CTransferStation::mutexLock); 

    //unlock
    pthread_mutex_unlock(&common::CTransferStation::mutexLock);

    return ;
}

Json::Value common::CTransferStation::PopMission(){
    //lock thread
    pthread_mutex_lock(&common::CTransferStation::mutexLock);
        Json::Value returnResult;
        int lastReadPoint = common::CTransferStation::missionReadPoint -1;
        lastReadPoint = (lastReadPoint < 0) ? CONFIGTRANSFERSTATION_QUEUELEN -1 : lastReadPoint;
        MissionParam_t mission = common::CTransferStation::missionQueue[lastReadPoint];
        returnResult["result"] = mission.result;
        returnResult["request"] = mission.mission;
        returnResult["state"] = TransState(mission.state);
    //unlock
    pthread_mutex_unlock(&common::CTransferStation::mutexLock);
    return returnResult;
}


void common::CTransferStation::UpdateProgress(Json::Value param){
    //lock thread
    pthread_mutex_lock(&common::CTransferStation::mutexLock);
        time_t seconds = time(NULL);
        
        if(!common::CTransferStation::missionQueue[common::CTransferStation::missionReadPoint].progress.empty()){
            (common::CTransferStation::missionQueue[common::CTransferStation::missionReadPoint]).progress.clear();
        }
        (common::CTransferStation::missionQueue[common::CTransferStation::missionReadPoint]).progress["progress"] = param;
        (common::CTransferStation::missionQueue[common::CTransferStation::missionReadPoint]).progress["timestamp"] =  static_cast<Json::UInt64> (seconds);
    //unlock
    pthread_mutex_unlock(&common::CTransferStation::mutexLock);
    return;
}

void common::CTransferStation::CallEnd(){
    pthread_mutex_lock(&common::CTransferStation::mutexLock);
        common::CTransferStation::isEnd = 1;
    pthread_mutex_unlock(&common::CTransferStation::mutexLock);
}

Json::Value common::CTransferStation::GetProgress(){
    //lock thread
    pthread_mutex_lock(&common::CTransferStation::mutexLock);
        Json::Value returnProgress;
        if(common::CTransferStation::missionReadPoint != common::CTransferStation::missionWritePoint){
            returnProgress = (common::CTransferStation::missionQueue[common::CTransferStation::missionReadPoint]).progress;
            returnProgress["missionId"] = common::CTransferStation::missionId[common::CTransferStation::missionReadPoint];
        }else if(common::CTransferStation::hasRun && common::CTransferStation::isEnd){
            int lastReadPoint = common::CTransferStation::missionReadPoint - 1;
            lastReadPoint = (lastReadPoint < 0) ? CONFIGTRANSFERSTATION_QUEUELEN -1 : lastReadPoint;
            returnProgress = (common::CTransferStation::missionQueue[lastReadPoint]).progress;
            returnProgress["missionId"] = common::CTransferStation::missionId[lastReadPoint];
        }
    //unlock
    pthread_mutex_unlock(&common::CTransferStation::mutexLock);
    return returnProgress;
}

void common::CTransferStation::PushState(Json::Value param){
    pthread_mutex_lock(&common::CTransferStation::stateLock);
        std::string state;
        state = param["action"].asString();

        if(!state.compare("pause")){
            if((common::CTransferStation::missionQueue[common::CTransferStation::missionReadPoint]).state != PAUSE){
                (common::CTransferStation::missionQueue[common::CTransferStation::missionReadPoint]).state = WAIT_PAUSE;
            }
        } else {
            if(!state.compare("start")){
                (common::CTransferStation::missionQueue[common::CTransferStation::missionReadPoint]).state = START;
            } else if(!state.compare("stop")){
                (common::CTransferStation::missionQueue[common::CTransferStation::missionReadPoint]).state = STOP;
            }
            pthread_cond_signal(&common::CTransferStation::condstate);
        }
        
    pthread_mutex_unlock(&common::CTransferStation::stateLock);
    return;
}

common::CTransferStation::STATE common::CTransferStation::GetMissionState(){
    pthread_mutex_lock(&common::CTransferStation::stateLock);
        STATE returnState;
        if(common::CTransferStation::missionReadPoint == common::CTransferStation::missionWritePoint && common::CTransferStation::hasRun && common::CTransferStation::isEnd){
            int lastReadPoint = common::CTransferStation::missionReadPoint - 1;
            lastReadPoint = (lastReadPoint < 0) ? CONFIGTRANSFERSTATION_QUEUELEN -1 : lastReadPoint;
            returnState = (common::CTransferStation::missionQueue[lastReadPoint]).state;
        } else if(common::CTransferStation::missionReadPoint == common::CTransferStation::missionWritePoint){
            returnState = WAIT;
        } else {
            returnState = (common::CTransferStation::missionQueue[common::CTransferStation::missionReadPoint]).state;
        }
    pthread_mutex_unlock(&common::CTransferStation::stateLock);
    
    return returnState;
}

common::CTransferStation::STATE common::CTransferStation::GetState(){
    pthread_mutex_lock(&common::CTransferStation::stateLock);
        STATE returnState;

        switch((common::CTransferStation::missionQueue[common::CTransferStation::missionReadPoint]).state){
            case PAUSE:
                (common::CTransferStation::missionQueue[common::CTransferStation::missionReadPoint]).state = PAUSE;
                pthread_cond_wait(&common::CTransferStation::condstate, &common::CTransferStation::stateLock);
            case WAIT_PAUSE:
                (common::CTransferStation::missionQueue[common::CTransferStation::missionReadPoint]).state = PAUSE;
                pthread_cond_wait(&common::CTransferStation::condstate, &common::CTransferStation::stateLock);
            default:
                returnState = (common::CTransferStation::missionQueue[common::CTransferStation::missionReadPoint]).state;
        }
    pthread_mutex_unlock(&common::CTransferStation::stateLock);
    
    return returnState;
}


void common::CTransferStation::PushStateUpdate(Json::Value param){
    pthread_mutex_lock(&common::CTransferStation::stateUpdateLock);
        Json::Value targetParam = param;
        missionStateUpdate.push_back(targetParam);

    pthread_mutex_unlock(&common::CTransferStation::stateUpdateLock);
}

Json::Value common::CTransferStation::GetStateUpdate(std::string param){
    pthread_mutex_lock(&common::CTransferStation::stateUpdateLock);
        Json::Value returnJson;
        int i = 0;
        for(i=0;i<missionStateUpdate.size();i++){
            if(missionStateUpdate[i].isMember(param)){
                returnJson = missionStateUpdate[i];
                break;
            }
        }
        if(i<missionStateUpdate.size()){
            missionStateUpdate.erase(missionStateUpdate.begin()+i);
        }
    pthread_mutex_unlock(&common::CTransferStation::stateUpdateLock);
    return returnJson;
}

