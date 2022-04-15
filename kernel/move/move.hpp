#ifndef   _MOVE_HPP_
#define   _MOVE_HPP_ 

#include <iostream>
#include <map>
#include "kernel/move/interfaces.hpp"

#include "kernel/move/missionget/missionget.hpp"
#include "kernel/move/missionprogress/missionprogress.hpp"
#include "kernel/move/missionstate/missionstate.hpp"
#include "kernel/move/taskcenter/taskcenter.hpp"

#define START_ACTION(module, moduleParam)  (move::CMove::moveMap[module])->Start(moduleParam)

namespace move{
    using namespace std;
    class CMove {
        public:
            static map<string, CInterfaces*> moveMap;
    };
}

#endif
