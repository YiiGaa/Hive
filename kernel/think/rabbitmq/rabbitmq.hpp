#ifndef   _THINK_rabbitmq_HPP_
#define   _THINK_rabbitmq_HPP_ 

#include "kernel/common/errorcode/errorcode.hpp"
#include "kernel/config/config.hpp"
#include "kernel/think/interface.hpp"
#include <string.h>

//lib
#include <amqp.h>
#include <amqp_tcp_socket.h>

/*
**Please do not modify the following
######Import start######
#include "kernel/think/rabbitmq/rabbitmq.hpp"
######Import end######

######LinkObjectImport start######
        {"rabbitmq", new Crabbitmq()},
######LinkObjectImport end######

######Config start######
        //rabbitmq
        #define CONFIG_RABBITMQUSER "mission"
        #define CONFIG_RABBITMQPWD "pCloud@mission0501!"
        #define CONFIG_RABBITMQADDRESS "10.139.9.28:5672"
######Config end######

######ErrorCodes enum start######
        //rabbitmq
        ERR_THINK_RABBITMQ_BLOCK,
        ERR_THINK_RABBITMQ_CREATECONNECT_BLOCK,
        ERR_THINK_RABBITMQ_OPENCONNECT_BLOCK,
        ERR_THINK_RABBITMQ_LOGGIN_BLOCK,
        ERR_THINK_RABBITMQ_CHANNELOPEN_BLOCK,
        ERR_THINK_RABBITMQ_CONNECT_EXCEPTION,
        ERR_THINK_RABBITMQ_CHANNELCLOSE_BLOCK,
        ERR_THINK_RABBITMQ_CONNECTCLOSE_BLOCK,
        ERR_THINK_RABBITMQ_CONNECTDESTROY_BLOCK,
        ERR_THINK_RABBITMQ_CONNECTCLOSE_EXCEPTION,
        ERR_THINK_RABBITMQ_EXCHANGEDECLARE_BLOCK,
        ERR_THINK_RABBITMQ_EXCHANGEDECLARE_EXCEPTION,
        ERR_THINK_RABBITMQ_QUEUEDECLARE_BLOCK,
        ERR_THINK_RABBITMQ_QUEUEDECLARENAME_BLOCK,
        ERR_THINK_RABBITMQ_QUEUEBLIND_BLOCK,
        ERR_THINK_RABBITMQ_QUEUEDECLARE_EXCEPTION,
        ERR_THINK_RABBITMQ_UNBINDQUEUE_BLOCK,
        ERR_THINK_RABBITMQ_UNBINDQUEUE_EXCEPTION,
        ERR_THINK_RABBITMQ_QOSPREFETCH_BLOCK,
        ERR_THINK_RABBITMQ_QOSPREFETCH_EXCEPTION,
        ERR_THINK_RABBITMQ_CONSUME_BLOCK,
        ERR_THINK_RABBITMQ_GETMESSAGE_BLOCK,
        ERR_THINK_RABBITMQ_GETMESSAGE_EXCEPTION,
        ERR_THINK_RABBITMQ_MESSAGEACK_BLOCK,
        ERR_THINK_RABBITMQ_MESSAGEACK_EXCEPTION,
        ERR_THINK_RABBITMQ_SENDMESSAGE_BLOCK,
        ERR_THINK_RABBITMQ_SENDMESSAGE_EXCEPTION,
        ERR_THINK_RABBITMQ_SENDMESSAGEEMPTY_BLOCK,
        ERR_THINK_RABBITMQ_HEARTBEAT_BLOCK,
        ERR_THINK_RABBITMQ_HEARTBEAT_EXCEPTION,
######ErrorCodes enum end######

######ErrorCodes start######
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
######ErrorCodes end######
*/

namespace think{

    class Crabbitmq: public CInterface{
        public:
            enum ACTION{
                NONE = 0,
                CONNECT,
                DISCONNECT,

                DECLAREEXCHANGE,
                DECLAREQUEUE,
                ACKMESSAGE,
                QOSPREFETCH,

                SENDMESSAGE,
                GETMESSAGE,
                SENDRESPONSEMESSAGE,

                SENDMESSAGEONCE,
                GETMESSAGEONCE,

                SENDHEARTBEAT,

                //for test
                SENDMESSAGEFORQUEUE,
                GETMESSAGEFORQUEUE,

                SENDMESSAGEDISTRIBUTE,
                GETMESSAGEDISTRIBUTE,

                SENDMESSAGESUBSCRIBE,
                GETMESSAGESUBSCRIBE,

                SENDMESSAGEROUTING,
                GETMESSAGEROUTING,
                SENDMESSAGEROUTINGRPC,

                SENDMESSAGETOPIC,
                GETMESSAGETOPIC,

                SENDMESSAGERPC,
                GETMESSAGERPC,
            };

            typedef struct {
                //necessary
                const char* bindingkey;
                std::string connectAdress;
                std::string connectUser;
                std::string connectPwd;
                
                //optional
                int channel;                    //default:1
                int durable;
                int autodelete;
                int noack;
                int heartbeat;

                int isrpc;                      //for sending message
                const char* queuenamerpcstr;
                const char* correlationid;

                const char* exchangetype;
                const char* queuenamestr;
                const char* returnkey;
   
                //SENDMESSAGE
                const char* sendStr;
                const char* routingkey;
                const char* exchange;

                //GETMESSAGE
                

                //Inner use
                amqp_bytes_t    queuename;
                amqp_bytes_t    queuenamerpc;
                amqp_bytes_t    correlationidrpc;
                uint64_t        deliverytag;      //use for ack
                std::string     correlationidget;
                amqp_connection_state_t connection;
            } ModuleParam_t;

            ERROR_CODE Start(int, void*, Json::Value&, Json::Value&);

        private:
            ERROR_CODE DoStart(ACTION, ModuleParam_t*, Json::Value&, Json::Value&);
            ERROR_CODE Connection(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);
            ERROR_CODE DisConnection(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);
            ERROR_CODE DeclareExchange(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);
            ERROR_CODE DeclareQueue(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);
            ERROR_CODE DeclareRPCQueue(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);
            ERROR_CODE UnbindQueue(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);
            ERROR_CODE QosPrefetch(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);
            ERROR_CODE GetMessage(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);
            ERROR_CODE GetResponseMessage(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);
            ERROR_CODE MessageACK(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);
            ERROR_CODE SendMessage(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);
            ERROR_CODE SendResponseMessage(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);
            ERROR_CODE SendHeartBeat(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);

            std::vector<std::string> Connection_SplitStr(std::string, std::string);

            ERROR_CODE ErrorWrapper(amqp_rpc_reply_t x, char const *context);
            
    };

}
#endif
