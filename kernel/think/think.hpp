#ifndef   _THINK_HPP_
#define   _THINK_HPP_ 

#include <iostream>
#include <map>
#include "kernel/think/interface.hpp"

#include "kernel/think/executeorder/executeorder.hpp"
#include "kernel/think/httppost/httppost.hpp"
#include "kernel/think/rabbitmq/rabbitmq.hpp"
#include "kernel/think/redis/redis.hpp"

#define START_MODULE(module, action, moduleParam, passParam, resultParam)  (think::CThink::thinkMap[module])->Start((int)action, (void*)&moduleParam, passParam, resultParam)

namespace think{
    using namespace std;
    class CThink {
        public:
            static map<string, CInterface*> thinkMap;
    };
}

#endif
