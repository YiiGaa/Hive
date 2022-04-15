#include "kernel/common/errorcode/errorcode.hpp"

std::map<ERROR_CODE, std::string> CErrorCode::errorMap = {
        //{ERROR_CODE, {"errorCode||message"}}
        {ERR_OK, {"200||OK"}},

        //executeorder
        {ERR_THINK_EXCUTEORDER_BLOCK, {"E-BT01(executeorder)||excute order 执行失败"}},
        {ERR_THINK_EXCUTEORDER_EXCEPTION, {"E-BT02(executeorder)||excute order 崩溃"}}, 
        //httppost
        {ERR_THINK_HTTPPOST_INIT_EXCEPTION, {"E-BT01(httppost)||httppost 初始化失败"}},  
        {ERR_THINK_HTTPPOST_CleanUp_EXCEPTION, {"E-BT02(httppost)||httppost 清理崩溃"}},  
        {ERR_THINK_HTTPPOST_POST_EXCEPTION, {"E-BT03(httppost)||httppost Post请求崩溃"}},  
        {ERR_THINK_HTTPPOST_GET_EXCEPTION, {"E-BT04(httppost)||httppost Get请求崩溃"}},  
        //rabbitMq
        {ERR_THINK_RABBITMQ_BLOCK, {"E-BT01(rabbitmq)||rabbitMq模块错误"}},
        {ERR_THINK_RABBITMQ_CREATECONNECT_BLOCK, {"E-BT02(rabbitmq)||rabbitMq连接创建失败"}},
        {ERR_THINK_RABBITMQ_OPENCONNECT_BLOCK, {"E-BT03(rabbitmq)||rabbitMq连接打开失败"}},
        {ERR_THINK_RABBITMQ_LOGGIN_BLOCK, {"E-BT04(rabbitmq)||rabbitMq连接登录失败"}},
        {ERR_THINK_RABBITMQ_CHANNELOPEN_BLOCK, {"E-BT05(rabbitmq)||rabbitMq频道打开失败"}},
        {ERR_THINK_RABBITMQ_CONNECT_EXCEPTION, {"E-BT06(rabbitmq)||rabbitMq连接打开崩溃"}},
        {ERR_THINK_RABBITMQ_CHANNELCLOSE_BLOCK, {"E-BT07(rabbitmq)||rabbitMq频道关闭失败"}},
        {ERR_THINK_RABBITMQ_CONNECTCLOSE_BLOCK, {"E-BT08(rabbitmq)||rabbitMq连接关闭失败"}},
        {ERR_THINK_RABBITMQ_CONNECTDESTROY_BLOCK, {"E-BT09(rabbitmq)||rabbitMq连接销毁失败"}},
        {ERR_THINK_RABBITMQ_CONNECTCLOSE_EXCEPTION, {"E-BT10(rabbitmq)||rabbitMq连接关闭崩溃"}},
        {ERR_THINK_RABBITMQ_EXCHANGEDECLARE_BLOCK, {"E-BT11(rabbitmq)||rabbitMq exchange创建失败"}},
        {ERR_THINK_RABBITMQ_EXCHANGEDECLARE_EXCEPTION, {"E-BT12(rabbitmq)||rabbitMq exchange创建崩溃"}},
        {ERR_THINK_RABBITMQ_QUEUEDECLARE_BLOCK, {"E-BT13(rabbitmq)||rabbitMq queue创建失败"}},
        {ERR_THINK_RABBITMQ_QUEUEDECLARENAME_BLOCK, {"E-BT14(rabbitmq)||rabbitMq queue创建名称获取失败"}},
        {ERR_THINK_RABBITMQ_QUEUEBLIND_BLOCK, {"E-BT15(rabbitmq)||rabbitMq queue绑定失败"}},
        {ERR_THINK_RABBITMQ_QUEUEDECLARE_EXCEPTION, {"E-BT16(rabbitmq)||rabbitMq queue创建崩溃"}},
        {ERR_THINK_RABBITMQ_UNBINDQUEUE_BLOCK, {"E-BT17(rabbitmq)||rabbitMq queue解绑定失败"}},
        {ERR_THINK_RABBITMQ_UNBINDQUEUE_EXCEPTION, {"E-BT18(rabbitmq)||rabbitMq queue解绑定崩溃"}},
        {ERR_THINK_RABBITMQ_QOSPREFETCH_BLOCK, {"E-BT19(rabbitmq)||rabbitMq qos设定失败"}},
        {ERR_THINK_RABBITMQ_QOSPREFETCH_EXCEPTION, {"E-BT20(rabbitmq)||rabbitMq qos设定崩溃"}},
        {ERR_THINK_RABBITMQ_CONSUME_BLOCK, {"E-BT21(rabbitmq)||rabbitMq消费消息失败"}},
        {ERR_THINK_RABBITMQ_GETMESSAGE_BLOCK, {"E-BT22(rabbitmq)||rabbitMq获取消息失败"}},
        {ERR_THINK_RABBITMQ_GETMESSAGE_EXCEPTION, {"E-BT23(rabbitmq)||rabbitMq获取消息崩溃"}},
        {ERR_THINK_RABBITMQ_MESSAGEACK_BLOCK, {"E-BT24(rabbitmq)||rabbitMq消息确认失败"}},
        {ERR_THINK_RABBITMQ_MESSAGEACK_EXCEPTION, {"E-BT25(rabbitmq)||rabbitMq消息确认崩溃"}},
        {ERR_THINK_RABBITMQ_SENDMESSAGE_BLOCK, {"E-BT26(rabbitmq)||rabbitMq发送消息失败"}},
        {ERR_THINK_RABBITMQ_SENDMESSAGE_EXCEPTION, {"E-BT27(rabbitmq)||rabbitMq发送消息崩溃"}},
        {ERR_THINK_RABBITMQ_SENDMESSAGEEMPTY_BLOCK, {"E-BT28(rabbitmq)||rabbitMq发送消息为空"}}, 
        {ERR_THINK_RABBITMQ_SENDMESSAGEEMPTY_BLOCK, {"E-BT29(rabbitmq)||rabbitMq发送心跳失败"}}, 
        {ERR_THINK_RABBITMQ_SENDMESSAGEEMPTY_BLOCK, {"E-BT30(rabbitmq)||rabbitMq发送心跳崩溃"}}, 
        //redis
        {ERR_THINK_REDIS_Connect_BLOCK, {"E-BT01(redis)||redis 连接失败"}},
        {ERR_THINK_REDIS_Connect_PWD_BLOCK, {"E-BT02(redis)||redis 认证失败"}},
        {ERR_THINK_REDIS_Connect_EXCEPTION, {"E-BT03(redis)||redis 连接崩溃"}},
        {ERR_THINK_REDIS_Excute_EXCEPTION, {"E-BT04(redis)||redis 执行崩溃"}},
        {ERR_THINK_REDIS_Delete_EXCEPTION, {"E-BT05(redis)||redis 删除崩溃"}},
        {ERR_THINK_REDIS_Expire_EXCEPTION, {"E-BT06(redis)||redis 设置过期时间崩溃"}},
        {ERR_THINK_REDIS_DisConnect_EXCEPTION, {"E-BT07(redis)||redis 断开连接崩溃"}},   
        //missionget
        {ERR_MOVE_MISSIONGET_Init_EXCEPTION, {"E-BM01(missionget)||mission get 初始化崩溃"}},
        {ERR_MOVE_MISSIONGET_ConnectPool_EXCEPTION, {"E-BM02(missionget)||mission get 连接任务池崩溃"}},
        {ERR_MOVE_MISSIONGET_DisConnectPool_EXCEPTION, {"E-BM03(missionget)||mission get 断开任务池崩溃"}},
        {ERR_MOVE_MISSIONGET_ListenMission_EXCEPTION, {"E-BM04(missionget)||mission get 监听任务崩溃"}},
        {ERR_MOVE_MISSIONGET_AckMission_EXCEPTION, {"E-BM05(missionget)||mission get 确认任务崩溃"}},
        {ERR_MOVE_MISSIONGET_ReportingMission_EXCEPTION, {"E-BM06(missionget)||mission get 报告任务结果崩溃"}},
        //missionprogress
        {ERR_MOVE_MISSIONPROGRESS_Init_EXCEPTION, {"E-BM001(missionprogress)||misson progress 初始化崩溃"}},
        {ERR_MOVE_MISSIONPROGRESS_ConnectPool_EXCEPTION, {"E-BM002(missionprogress)||misson progress 连接数据池崩溃"}},
        {ERR_MOVE_MISSIONPROGRESS_DisConnectPool_EXCEPTION, {"E-BM003(missionprogress)||misson progress 断开连接数据池崩溃"}},
        {ERR_MOVE_MISSIONPROGRESS_GetingProgress_EXCEPTION, {"E-BM004(missionprogress)||misson progress 获取进度失败"}},
        {ERR_MOVE_MISSIONPROGRESS_ReportingProgress_EXCEPTION, {"E-BM005(missionprogress)||misson progress 报告进度崩溃"}},
        {ERR_MOVE_MISSIONPROGRESS_ReportingWatchDog_EXCEPTION, {"E-BM006(missionprogress)||misson progress 看门狗通知崩溃"}},
        //missionstate
        {ERR_MOVE_MISSIONSTATE_Init_EXCEPTION, {"E-BM001(missionstate)||misson state 初始化崩溃"}},
        {ERR_MOVE_MISSIONSTATE_ConnectPool_EXCEPTION, {"E-BM002(missionstate)||misson state 连接状态池崩溃"}},
        {ERR_MOVE_MISSIONSTATE_DisConnectPool_EXCEPTION, {"E-BM003(missionstate)||misson state 断开状态池崩溃"}},
        {ERR_MOVE_MISSIONSTATE_ListenState_EXCEPTION, {"E-BM004(missionstate)||misson state 获取状态崩溃"}},
        {ERR_MOVE_MISSIONSTATE_AckState_EXCEPTION, {"E-BM005(missionstate)||misson state 状态确认崩溃"}},
        //taskcenter
        {ERR_MOVE_TASKCENTER_Init_EXCEPTION, {"E-BM01(taskcenter)||taskcenter 初始化崩溃"}},
        {ERR_MOVE_TASKCENTER_ListenMission_EXCEPTION, {"E-BM02(taskcenter)||taskcenter 监听任务崩溃"}},
        {ERR_MOVE_TASKCENTER_FinishMission_EXCEPTION, {"E-BM03(taskcenter)||taskcenter 结束任务崩溃"}},
        {ERR_MOVE_TASKCENTER_StartMission_EXCEPTION, {"E-BM04(taskcenter)||taskcenter 开始任务崩溃"}},
};


int CErrorCode::isErrorHappened(ERROR_CODE param){
    if(param == ERR_OK){
        return 0;
    }
    return -1; 
}

std::string CErrorCode::getErrorCode(ERROR_CODE param){
    std::vector<std::string> result = TOOLS_STRINGSPLIT(CErrorCode::errorMap[param],"||");
    return result[0];
}

std::string CErrorCode::getErrorMessage(ERROR_CODE param){
    std::vector<std::string> result = TOOLS_STRINGSPLIT(CErrorCode::errorMap[param],"||");
    return result[1];
}
