#ifndef   _TRANSFERSTATION_HPP_
#define   _TRANSFERSTATION_HPP_ 

#define TRANSFERSTATION_INIT() common::CTransferStation::Init()

#define TRANSFERSTATION_PUSHMISSION(id, mission, timeoutSenconds) common::CTransferStation::PushMission(id, mission, timeoutSenconds)
#define TRANSFERSTATION_GETMISSION() common::CTransferStation::GetMission()
#define TRANSFERSTATION_GETMISSIONID() common::CTransferStation::GetMissionId()
#define TRANSFERSTATION_FINISHMISSION(ret, result) common::CTransferStation::FinishMission(ret, result)
#define TRANSFERSTATION_POPMISSISON() common::CTransferStation::PopMission()

#define TRANSFERSTATION_UPDATEPROGRESS(progress) common::CTransferStation::UpdateProgress(progress)
#define TRANSFERSTATION_GETPROGRESS() common::CTransferStation::GetProgress()

#define TRANSFERSTATION_GETMISSIONSTATE() common::CTransferStation::GetMissionState()
#define TRANSFERSTATION_GETSTATE() common::CTransferStation::GetState()
#define TRANSFERSTATION_TRANSSTATE(state) common::CTransferStation::TransState(state)
#define TRANSFERSTATION_PUSHSTATE(state) common::CTransferStation::PushState(state)
#define TRANSFERSTATION_PUSHSTATEUPDATE(operation) common::CTransferStation::PushStateUpdate(operation)
#define TRANSFERSTATION_GETSTATEUPDATE(stateAction) common::CTransferStation::GetStateUpdate(stateAction)

#define TRANSFERSTATION_CALLEND() common::CTransferStation::CallEnd()

#include "pthread.h"
#include <string>
#include "kernel/common/transferstation/config.hpp"
#include <jsoncpp/json/json.h>
#include "kernel/common/errorcode/errorcode.hpp"

namespace common{
    class CTransferStation {

        public:
            enum STATE{
                NONE = 0,
                WAIT,
                ACCEPT,
                START,
                WAIT_PAUSE,
                PAUSE,
                STOP,
                COMPLETE,
            };
            typedef struct {
                Json::Value mission;
                Json::Value result;
                STATE state;
                Json::Value progress;
            } MissionParam_t;
            static std::string missionId[CONFIGTRANSFERSTATION_QUEUELEN];
            static MissionParam_t missionQueue[CONFIGTRANSFERSTATION_QUEUELEN];
            static int missionReadPoint;
            static int missionWritePoint;
            static std::vector<Json::Value> missionStateUpdate;
            static int isEnd;
            static int hasRun;


        public:
            // static void InitStruct(void *, int);
            // static int ReadJson(const char *, Json::Value &);
            static void Init();
            //static int CheckMissionId(std::string);
            static int PushMission(std::string, Json::Value, int);
            static Json::Value GetMission();
            static std::string GetMissionId();
            static void FinishMission(ERROR_CODE, Json::Value);
            static Json::Value PopMission();
            static void UpdateProgress(Json::Value);
            static Json::Value GetProgress();
            static void PushState(Json::Value);
            static STATE GetMissionState();
            static STATE GetState();
            static void PushStateUpdate(Json::Value);
            static Json::Value GetStateUpdate(std::string);
            static std::string TransState(STATE);
            static void CallEnd();

            //Mission Lock
            static pthread_mutex_t mutexLock;  
            static pthread_cond_t condPush;
            static pthread_cond_t condPop;

            //state Lock
            static pthread_mutex_t stateLock;  
            static pthread_cond_t condstate;

            //state update(Operation update) Lock
            static pthread_mutex_t stateUpdateLock;  
            static pthread_cond_t condUpdatestate;

    };
}

#endif