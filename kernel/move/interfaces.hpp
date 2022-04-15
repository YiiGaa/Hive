#ifndef   _MOVE_INTERFACES_HPP_
#define   _MOVE_INTERFACES_HPP_ 
#include "kernel/common/errorcode/errorcode.hpp"
#include <jsoncpp/json/json.h>

namespace move{
    class CInterfaces {
        public:
            virtual ERROR_CODE Start(Json::Value moduleParam){return ERR_OK;};
    };
}

#endif