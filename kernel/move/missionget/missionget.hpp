#ifndef   _MOVE_missionget_HPP_
#define   _MOVE_missionget_HPP_ 

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
#include "kernel/move/missionget/missionget.hpp"
######Import end######

######LinkObjectImport start######
        {"missionget", new Cmissionget()},
######LinkObjectImport end######

######Config start######
        //missionget
######Config end######

######ErrorCodes enum start######
        //missionget
        ERR_MOVE_MISSIONGET_Init_EXCEPTION,
        ERR_MOVE_MISSIONGET_ConnectPool_EXCEPTION,
        ERR_MOVE_MISSIONGET_DisConnectPool_EXCEPTION,
        ERR_MOVE_MISSIONGET_ListenMission_EXCEPTION,
        ERR_MOVE_MISSIONGET_AckMission_EXCEPTION,
        ERR_MOVE_MISSIONGET_ReportingMission_EXCEPTION,
######ErrorCodes enum end######

######ErrorCodes start######
        //missionget
        {ERR_MOVE_MISSIONGET_Init_EXCEPTION, {"E-BM01(missionget)||mission get 初始化崩溃"}},
        {ERR_MOVE_MISSIONGET_ConnectPool_EXCEPTION, {"E-BM02(missionget)||mission get 连接任务池崩溃"}},
        {ERR_MOVE_MISSIONGET_DisConnectPool_EXCEPTION, {"E-BM03(missionget)||mission get 断开任务池崩溃"}},
        {ERR_MOVE_MISSIONGET_ListenMission_EXCEPTION, {"E-BM04(missionget)||mission get 监听任务崩溃"}},
        {ERR_MOVE_MISSIONGET_AckMission_EXCEPTION, {"E-BM05(missionget)||mission get 确认任务崩溃"}},
        {ERR_MOVE_MISSIONGET_ReportingMission_EXCEPTION, {"E-BM06(missionget)||mission get 报告任务结果崩溃"}},
######ErrorCodes end######
*/

namespace move{

    class Cmissionget: public CInterfaces{
        public:
            typedef struct {
                //necessary
                    const char* mesqmission_queuename;
                    std::string mesqmission_adress;
                    std::string mesqmission_user;
                    std::string mesqmission_pwd;
                
                //opt
                    std::string mesqmission_statequeuename;  //get by missionState Setting
                
                //optional
                    int mesqmission_channel;
                    int mesqmission_isackfirst;

                //Inner use    
                    Json::Value passParam;
                    Json::Value returnParam;
            } ModuleParam_t;

            ERROR_CODE Start(Json::Value);

        private:
            ERROR_CODE DoStart(ModuleParam_t*, Json::Value&, Json::Value&);
            ERROR_CODE Init(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);
            ERROR_CODE ConnectMissionPool(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);
            ERROR_CODE DisConnectMissionPool(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);
            ERROR_CODE ListenMission(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);
            ERROR_CODE AckMission(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);
            ERROR_CODE ReportingResult(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);

        private:
            think::Crabbitmq::ModuleParam_t moduleParam_rabbitmqMession;
            think::Chttppost::ModuleParam_t moduleParam_httpResponse;
            
    };

}
#endif
