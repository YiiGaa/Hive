#ifndef   _THINK_executeorder_HPP_
#define   _THINK_executeorder_HPP_ 

#include "kernel/common/errorcode/errorcode.hpp"
#include "kernel/think/interface.hpp"
#include <string.h>

//lib

/*
**Please do not modify the following
######Import start######
#include "kernel/think/executeorder/executeorder.hpp"
######Import end######

######LinkObjectImport start######
        {"executeorder", new Cexecuteorder()},
######LinkObjectImport end######

######Config start######
        //executeorder
######Config end######

######ErrorCodes enum start######
        //executeorder
        ERR_THINK_EXCUTEORDER_BLOCK,
        ERR_THINK_EXCUTEORDER_EXCEPTION,
######ErrorCodes enum end######

######ErrorCodes start######
        //executeorder
        {ERR_THINK_EXCUTEORDER_BLOCK, {"E-BT01(executeorder)||excute order 执行失败"}},
        {ERR_THINK_EXCUTEORDER_EXCEPTION, {"E-BT02(executeorder)||excute order 崩溃"}}, 
######ErrorCodes end######
*/

namespace think{

    class Cexecuteorder: public CInterface{
        public:
            enum ACTION{
                NONE = 0,
                NORMAL,
            };

            typedef struct {
                const char *targetKey;

            //optional
                void (*progressCallBack)(Json::Value);
                int (*statusCheckCallBack)();

            } ModuleParam_t;

            ERROR_CODE Start(int, void*, Json::Value&, Json::Value&);

        private:
            ERROR_CODE DoStart(ACTION, ModuleParam_t*, Json::Value&, Json::Value&);
            ERROR_CODE Excute(ModuleParam_t*, Json::Value&, Json::Value&, ERROR_CODE);
            //selef function

        private:
            static void Excute_GetProgress(char*, Json::Value&, int);
            
    };

}
#endif
