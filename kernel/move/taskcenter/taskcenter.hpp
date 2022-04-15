#ifndef   _MOVE_taskcenter_HPP_
#define   _MOVE_taskcenter_HPP_ 

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
#include "kernel/move/taskcenter/taskcenter.hpp"
######Import end######

######LinkObjectImport start######
        {"taskcenter", new Ctaskcenter()},
######LinkObjectImport end######

######Config start######
        //taskcenter
        #define CONFIG_TASKCENTERSTREAMRETRYTIME 2
######Config end######

######ErrorCodes enum start######
        //taskcenter
        ERR_MOVE_TASKCENTER_Init_EXCEPTION,
        ERR_MOVE_TASKCENTER_ListenMission_EXCEPTION,
        ERR_MOVE_TASKCENTER_FinishMission_EXCEPTION,
        ERR_MOVE_TASKCENTER_StartMission_EXCEPTION,
######ErrorCodes enum end######

######ErrorCodes start######
        //taskcenter
        {ERR_MOVE_TASKCENTER_Init_EXCEPTION, {"E-BM01(taskcenter)||taskcenter 初始化崩溃"}},
        {ERR_MOVE_TASKCENTER_ListenMission_EXCEPTION, {"E-BM02(taskcenter)||taskcenter 监听任务崩溃"}},
        {ERR_MOVE_TASKCENTER_FinishMission_EXCEPTION, {"E-BM03(taskcenter)||taskcenter 结束任务崩溃"}},
        {ERR_MOVE_TASKCENTER_StartMission_EXCEPTION, {"E-BM04(taskcenter)||taskcenter 开始任务崩溃"}},
######ErrorCodes end######
*/

namespace move{

    class Ctaskcenter: public CInterfaces{
        public:
            typedef struct {
                //necessary

                //Inner use    
                    Json::Value passParam;
                    Json::Value returnParam;
            } ModuleParam_t;

            ERROR_CODE Start(Json::Value);

        private:
            ERROR_CODE DoStart(ModuleParam_t*, Json::Value&, Json::Value&);
            ERROR_CODE Init(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);
            ERROR_CODE ListenMission(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);
            ERROR_CODE FinishMission(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);
            ERROR_CODE StartMission(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);

        public://Subsidiary Function
            static void StartMission_Progress(Json::Value);
            static int StartMission_StatusCheck();

        public:
            static Json::Value progressParam;
            static Json::Value missionPassParam;
            
            think::Cexecuteorder::ModuleParam_t moduleParam_order;
    };

}
#endif
