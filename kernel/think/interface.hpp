#ifndef   _INTERFACE_HPP_
#define   _INTERFACE_HPP_ 
#include "kernel/common/errorcode/errorcode.hpp"
#include <jsoncpp/json/json.h>

namespace think{
    class CInterface {
        public:
            virtual ERROR_CODE Start(int action, void* moduleParam, Json::Value& passParam, Json::Value& resultParam){return ERR_OK;};
    };
}

#endif