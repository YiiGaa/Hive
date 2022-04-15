#ifndef   _MOVE_missionprogress_HPP_
#define   _MOVE_missionprogress_HPP_ 

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
#include "kernel/move/missionprogress/missionprogress.hpp"
######Import end######

######LinkObjectImport start######
        {"missionprogress", new Cmissionprogress()},
######LinkObjectImport end######

######Config start######
        //missionprogress
######Config end######

######ErrorCodes enum start######
        //missionprogress
        ERR_MOVE_MISSIONPROGRESS_Init_EXCEPTION,
        ERR_MOVE_MISSIONPROGRESS_ConnectPool_EXCEPTION,
        ERR_MOVE_MISSIONPROGRESS_DisConnectPool_EXCEPTION,
        ERR_MOVE_MISSIONPROGRESS_GetingProgress_EXCEPTION,
        ERR_MOVE_MISSIONPROGRESS_ReportingProgress_EXCEPTION,
        ERR_MOVE_MISSIONPROGRESS_ReportingWatchDog_EXCEPTION,
######ErrorCodes enum end######

######ErrorCodes start######
        //missionprogress
        {ERR_MOVE_MISSIONPROGRESS_Init_EXCEPTION, {"E-BM001(missionprogress)||misson progress 初始化崩溃"}},
        {ERR_MOVE_MISSIONPROGRESS_ConnectPool_EXCEPTION, {"E-BM002(missionprogress)||misson progress 连接数据池崩溃"}},
        {ERR_MOVE_MISSIONPROGRESS_DisConnectPool_EXCEPTION, {"E-BM003(missionprogress)||misson progress 断开连接数据池崩溃"}},
        {ERR_MOVE_MISSIONPROGRESS_GetingProgress_EXCEPTION, {"E-BM004(missionprogress)||misson progress 获取进度失败"}},
        {ERR_MOVE_MISSIONPROGRESS_ReportingProgress_EXCEPTION, {"E-BM005(missionprogress)||misson progress 报告进度崩溃"}},
        {ERR_MOVE_MISSIONPROGRESS_ReportingWatchDog_EXCEPTION, {"E-BM006(missionprogress)||misson progress 看门狗通知崩溃"}},
######ErrorCodes end######
*/

namespace move{

    class Cmissionprogress: public CInterfaces{
        public:
            typedef struct {
                //necessary
                    std::string watchdog_adress;
                    std::string watchdog_pwd;
                    std::string progress_adress;
                    std::string progress_pwd;
                    const char* progress_keyname;
                    const char* watchdog_keyname;
                    int progress_timer;

                //Inner use    
                    Json::Value passParam;
                    Json::Value returnParam;
            } ModuleParam_t;

            ERROR_CODE Start(Json::Value);

        private:
            ERROR_CODE DoStart(ModuleParam_t*, Json::Value&, Json::Value&);
            ERROR_CODE Init(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);
            ERROR_CODE ConnectProgressPool(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);
            ERROR_CODE DisConnectProgressPool(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);
            ERROR_CODE GetingProgress(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);
            ERROR_CODE ReportingProgress(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);
            ERROR_CODE ReportingWatchDog(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);

        private:
            think::Credis::ModuleParam_t moduleParam_redisProgress;
            think::Credis::ModuleParam_t moduleParam_redisWatchdog;
            
    };

}
#endif
