#ifndef   _THINK_redis_HPP_
#define   _THINK_redis_HPP_ 

#include "kernel/common/errorcode/errorcode.hpp"
#include "kernel/config/config.hpp"
#include "kernel/think/interface.hpp"
#include <string.h>

//lib
#include <hiredis/hiredis.h>
#include <sw/redis++/redis++.h>

/*
**Please do not modify the following
######Import start######
#include "kernel/think/redis/redis.hpp"
######Import end######

######LinkObjectImport start######
        {"redis", new Credis()},
######LinkObjectImport end######

######Config start######
        //redis
        #define CONFIG_REDISADRESS "10.139.17.164:6383"
        #define CONFIG_REDISPWD "pCloud@progress0401!"
######Config end######

######ErrorCodes enum start######
        //redis
        ERR_THINK_REDIS_Connect_BLOCK,
        ERR_THINK_REDIS_Connect_PWD_BLOCK,
        ERR_THINK_REDIS_Connect_EXCEPTION,
        ERR_THINK_REDIS_Excute_EXCEPTION,
        ERR_THINK_REDIS_Delete_EXCEPTION,
        ERR_THINK_REDIS_Expire_EXCEPTION,
        ERR_THINK_REDIS_DisConnect_EXCEPTION,  
######ErrorCodes enum end######

######ErrorCodes start######
        //redis
        {ERR_THINK_REDIS_Connect_BLOCK, {"E-BT01(redis)||redis 连接失败"}},
        {ERR_THINK_REDIS_Connect_PWD_BLOCK, {"E-BT02(redis)||redis 认证失败"}},
        {ERR_THINK_REDIS_Connect_EXCEPTION, {"E-BT03(redis)||redis 连接崩溃"}},
        {ERR_THINK_REDIS_Excute_EXCEPTION, {"E-BT04(redis)||redis 执行崩溃"}},
        {ERR_THINK_REDIS_Delete_EXCEPTION, {"E-BT05(redis)||redis 删除崩溃"}},
        {ERR_THINK_REDIS_Expire_EXCEPTION, {"E-BT06(redis)||redis 设置过期时间崩溃"}},
        {ERR_THINK_REDIS_DisConnect_EXCEPTION, {"E-BT07(redis)||redis 断开连接崩溃"}},   
######ErrorCodes end######
*/

namespace think{

    class Credis: public CInterface{
        public:
            enum ACTION{
                NONE = 0,
                CONNECT,
                DISCONNECT,
                UPDATE,
                EXPIRE,
                DELETE,
                ONCE,
            };

            typedef struct {
                //necessary
                const char* field;
                const char* key;
                const char* value;
                std::string connectAdress;
                std::string connectPwd;
                int exprieSecond;

                //inner use
                int isCluster;
                redisContext *connect;
                sw::redis::ConnectionOptions connectionOptions;
            } ModuleParam_t;

            ERROR_CODE Start(int, void*, Json::Value&, Json::Value&);

        private:
            ERROR_CODE DoStart(ACTION, ModuleParam_t*, Json::Value&, Json::Value&);
            ERROR_CODE Connect(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);
            ERROR_CODE Excute(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);
            ERROR_CODE Delete(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);
            ERROR_CODE Expire(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);
            ERROR_CODE DisConnect(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);
            
            std::vector<std::string> Connection_SplitStr(std::string, std::string);

        private:
            //self variable
            
    };

}
#endif
