#ifndef   _THINK_httppost_HPP_
#define   _THINK_httppost_HPP_ 

#include "kernel/common/errorcode/errorcode.hpp"
#include "kernel/think/interface.hpp"
#include <string.h>
#include <map>

//lib

/*
**Please do not modify the following
######Import start######
#include "kernel/think/httppost/httppost.hpp"
######Import end######

######LinkObjectImport start######
        {"httppost", new Chttppost()},
######LinkObjectImport end######

######Config start######
        //httppost
######Config end######

######ErrorCodes enum start######
        //httppost
        ERR_THINK_HTTPPOST_INIT_EXCEPTION,
        ERR_THINK_HTTPPOST_CleanUp_EXCEPTION,
        ERR_THINK_HTTPPOST_POST_EXCEPTION,
        ERR_THINK_HTTPPOST_GET_EXCEPTION,
        ERR_THINK_HTTPPOST_CONNECT_BLOCK,
        ERR_THINK_HTTPPOST_REQUEST_BLOCK,
######ErrorCodes enum end######

######ErrorCodes start######
        //httppost
        {ERR_THINK_HTTPPOST_INIT_EXCEPTION, {"E-BT01(httppost)||httppost 初始化失败"}},  
        {ERR_THINK_HTTPPOST_CleanUp_EXCEPTION, {"E-BT02(httppost)||httppost 清理崩溃"}},  
        {ERR_THINK_HTTPPOST_POST_EXCEPTION, {"E-BT03(httppost)||httppost Post请求崩溃"}},  
        {ERR_THINK_HTTPPOST_GET_EXCEPTION, {"E-BT04(httppost)||httppost Get请求崩溃"}},
        {ERR_THINK_HTTPPOST_CONNECT_BLOCK, {"E-BT05(httppost)||httppost 链接失败"}}, 
        {ERR_THINK_HTTPPOST_REQUEST_BLOCK, {"E-BT06(httppost)||httppost 请求失败"}},    
######ErrorCodes end######
*/

namespace think{

    class Chttppost: public CInterface{
        public:
            enum ACTION{
                NONE = 0,
            };

            typedef struct {
                //necessary
                   char* url;
                   char* requestMethod;
                   char* messageBody;
            } ModuleParam_t;

            ERROR_CODE Start(int, void*, Json::Value&, Json::Value&);
        
        public:
            static size_t ProcessData(void *ptr, size_t size, size_t nmemb, void *stream);

        private:
            ERROR_CODE DoStart(ACTION, ModuleParam_t*, Json::Value&, Json::Value&);
            ERROR_CODE Init(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);
            ERROR_CODE CleanUp(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);
            ERROR_CODE HttpPost(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);
            ERROR_CODE HttpGet(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);
            //selef function

        private:
            //self variable
            
    };

}
#endif
