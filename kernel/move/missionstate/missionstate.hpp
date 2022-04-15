#ifndef   _MOVE_missionstate_HPP_
#define   _MOVE_missionstate_HPP_ 

#include "kernel/common/errorcode/errorcode.hpp"
#include "kernel/move/interfaces.hpp"
#include <string.h>
#include "kernel/think/think.hpp"
#include "kernel/common/transferstation/transferstation.hpp"

//lib
#include <jsoncpp/json/json.h>

/*
**Please do not modify the following
######Import start######
#include "kernel/move/missionstate/missionstate.hpp"
######Import end######

######LinkObjectImport start######
        {"missionstate", new Cmissionstate()},
######LinkObjectImport end######

######Config start######
        //missionstate
######Config end######

######ErrorCodes enum start######
        //missionstate
        ERR_MOVE_MISSIONSTATE_Init_EXCEPTION,
        ERR_MOVE_MISSIONSTATE_ConnectPool_EXCEPTION,
        ERR_MOVE_MISSIONSTATE_DisConnectPool_EXCEPTION,
        ERR_MOVE_MISSIONSTATE_ListenState_EXCEPTION,
        ERR_MOVE_MISSIONSTATE_AckState_EXCEPTION,
######ErrorCodes enum end######

######ErrorCodes start######
        //missionstate
        {ERR_MOVE_MISSIONSTATE_Init_EXCEPTION, {"E-BM001(missionstate)||misson state 初始化崩溃"}},
        {ERR_MOVE_MISSIONSTATE_ConnectPool_EXCEPTION, {"E-BM002(missionstate)||misson state 连接状态池崩溃"}},
        {ERR_MOVE_MISSIONSTATE_DisConnectPool_EXCEPTION, {"E-BM003(missionstate)||misson state 断开状态池崩溃"}},
        {ERR_MOVE_MISSIONSTATE_ListenState_EXCEPTION, {"E-BM004(missionstate)||misson state 获取状态崩溃"}},
        {ERR_MOVE_MISSIONSTATE_AckState_EXCEPTION, {"E-BM005(missionstate)||misson state 状态确认崩溃"}},
######ErrorCodes end######
*/

namespace move{

    class Cmissionstate: public CInterfaces{
        public:
            typedef struct {
                //necessary
                    const char* mesqstate_exchange;
                    const char* mesqstate_bindingkey;
                    const char* mesqstate_queuenamestr;
                    std::string mesqstate_adress;
                    std::string mesqstate_user;
                    std::string mesqstate_pwd;
                
                //optional
                    int mesqstate_channel;

                //Inner use    
                    Json::Value passParam;
                    Json::Value returnParam;
            } ModuleParam_t;

            ERROR_CODE Start(Json::Value);

        private:
            ERROR_CODE DoStart(ModuleParam_t*, Json::Value&, Json::Value&);
            ERROR_CODE Init(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);
            ERROR_CODE ConnectStatePool(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);
            ERROR_CODE DisConnectStatePool(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);
            ERROR_CODE ListenState(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);
            ERROR_CODE AckState(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);

        private:
            think::Crabbitmq::ModuleParam_t moduleParam_rabbitmqState;
            
    };

}
#endif
