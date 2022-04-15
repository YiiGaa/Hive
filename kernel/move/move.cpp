#include "kernel/move/move.hpp"

namespace move{
    map<string, CInterfaces*> CMove::moveMap = {
        {"missionget", new Cmissionget()},
        {"missionprogress", new Cmissionprogress()},
        {"missionstate", new Cmissionstate()},
        {"taskcenter", new Ctaskcenter()},
    };
}
