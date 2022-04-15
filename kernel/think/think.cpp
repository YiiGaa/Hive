#include "kernel/think/think.hpp"

namespace think{
    map<string, CInterface*> CThink::thinkMap = {
        {"executeorder", new Cexecuteorder()},
        {"httppost", new Chttppost()},
        {"rabbitmq", new Crabbitmq()},
        {"redis", new Credis()},
    };
}
