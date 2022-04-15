#include "kernel/think/rabbitmq/rabbitmq.hpp"
#include "kernel/common/errorcode/errorcode.hpp"
#include "kernel/common/logger/logger.hpp"
#include "kernel/common/tools/tools.hpp"
#include "kernel/think/interface.hpp"
#include <stdio.h>
#include <map>


#include <unistd.h>

/* The same function as "kernel/think/think.hpp" */
#define START_MODULE(module, action, moduleParam, passParam, resultParam)  (thinkMap[module])->Start((int)action, (void*)&moduleParam, passParam, resultParam)
std::map<std::string, think::CInterface*> thinkMap = {
    {"rabbitmq", new think::Crabbitmq()},    //Need to add "kernel/think/think.cpp" manually
};
/* The same function as "kernel/think/think.hpp" */

using namespace think;
int main(int argc, char *argv[]){
    Json::Value passParam;
    Json::Value returnParam;

    if(argc < 2){
        printf("\n\nplease enter: target \n\n\n");
        return 0;
    }

    //STEP filling passParam
    Json::Reader passParamReader;
    Json::Value tempParam;
    std::ifstream passParamJson("../sample.json");//open file example.json

    if(!passParamReader.parse(passParamJson, tempParam)){
        printf("\n\n***Error:missing ../sample.json\n\n\n");
        passParamJson.close();
        return -2;
    }
    passParamJson.close();

    if(!tempParam.isMember(argv[1])){
        printf("\n\n***Error:missing %s in sample.json\n\n", argv[1]);
        return -3;
    }
    Json::Value targetParam = tempParam[argv[1]];

    //STEP rabbitmq
    think::Crabbitmq::ModuleParam_t moduleParam_1;
    moduleParam_1.connectAdress = CONFIG_RABBITMQADDRESS;    //or set by yourself
    moduleParam_1.connectUser = CONFIG_RABBITMQUSER;         //or set by yourself
    moduleParam_1.connectPwd = CONFIG_RABBITMQPWD;           //or set by yourself
    moduleParam_1.channel = 1;
    moduleParam_1.durable = 0;
    moduleParam_1.autodelete = 0;
    moduleParam_1.noack = 0;
    moduleParam_1.isrpc = 0;
    moduleParam_1.bindingkey = 0;
    moduleParam_1.queuenamerpcstr = 0;
    moduleParam_1.correlationid = 0;
    moduleParam_1.exchangetype = 0;
    moduleParam_1.queuenamestr = 0;
    moduleParam_1.returnkey = 0;
    moduleParam_1.sendStr = 0;
    moduleParam_1.routingkey = 0;
    moduleParam_1.exchange = 0;
    
    think::Crabbitmq::ACTION action = think::Crabbitmq::NONE;
    //STEP get module param
    if(targetParam.isMember("action")){
        action = (think::Crabbitmq::ACTION)targetParam["action"].asInt();
    }

    if(targetParam.isMember("durable")){
        moduleParam_1.durable = targetParam["durable"].asInt();
    }

    if(targetParam.isMember("autodelete")){
        moduleParam_1.autodelete = targetParam["autodelete"].asInt();
    }

    if(targetParam.isMember("noack")){
        moduleParam_1.noack = targetParam["noack"].asInt();
    }

    if(targetParam.isMember("isrpc")){
        moduleParam_1.isrpc = targetParam["isrpc"].asInt();
    }

    if(targetParam.isMember("channel")){
        moduleParam_1.channel = targetParam["channel"].asInt();
    }

    std::string queuenamestr;
    if(targetParam.isMember("queuenamestr")){
        queuenamestr = targetParam["queuenamestr"].asString();
        moduleParam_1.queuenamestr = (char*)queuenamestr.c_str();
    }

    std::string queuenamerpcstr;
    if(targetParam.isMember("queuenamerpcstr")){
        queuenamerpcstr = targetParam["queuenamerpcstr"].asString();
        moduleParam_1.queuenamerpcstr = (char*)queuenamerpcstr.c_str();
    }

    std::string correlationid;
    if(targetParam.isMember("correlationid")){
        correlationid = targetParam["correlationid"].asString();
        moduleParam_1.correlationid = (char*)correlationid.c_str();
    }

    std::string bindingkey;
    if(targetParam.isMember("bindingkey")){
        bindingkey = targetParam["bindingkey"].asString();
        moduleParam_1.bindingkey = (char*)bindingkey.c_str();
    }

    std::string exchangetype;
    if(targetParam.isMember("exchangetype")){
        exchangetype = targetParam["exchangetype"].asString();
        moduleParam_1.exchangetype = (char*)exchangetype.c_str();
    }

    std::string exchange;
    if(targetParam.isMember("exchange")){
        exchange = targetParam["exchange"].asString();
        moduleParam_1.exchange = (char*)exchange.c_str();
    }

    std::string routingkey;
    if(targetParam.isMember("routingkey")){
        routingkey = targetParam["routingkey"].asString();
        moduleParam_1.routingkey = (char*)routingkey.c_str();
    }

    std::string returnkey;
    if(targetParam.isMember("returnkey")){
        returnkey = targetParam["returnkey"].asString();
        moduleParam_1.returnkey = (char*)returnkey.c_str();
    }

    std::string sendStr;
    Json::FastWriter swriter;
    if(targetParam.isMember("sendStr")){
        passParam["sending"] = targetParam["sendStr"];
        moduleParam_1.sendStr = "sending";
    }

    //STEP action
    ERROR_CODE ret = ERR_OK;
    if(strcmp(argv[1], "exchangeDeclare")==0){
        action = think::Crabbitmq::DECLAREEXCHANGE;
    } else if(strcmp(argv[1], "queueDeclare")==0){
        action = think::Crabbitmq::DECLAREQUEUE;
    } else if(strcmp(argv[1], "sendQueue")==0){
        action = think::Crabbitmq::SENDMESSAGEONCE;
    } else if(strcmp(argv[1], "getQueue")==0){
        action = think::Crabbitmq::GETMESSAGEONCE;
    } else if(strcmp(argv[1], "sendJustQueue")==0){
        action = think::Crabbitmq::SENDMESSAGEFORQUEUE;
    } else if(strcmp(argv[1], "getJustQueue")==0){
        action = think::Crabbitmq::GETMESSAGEFORQUEUE;
    } else if(strcmp(argv[1], "sendDistribute")==0){
        action = think::Crabbitmq::SENDMESSAGEDISTRIBUTE;
    } else if(strcmp(argv[1], "getDistribute")==0){
        action = think::Crabbitmq::GETMESSAGEDISTRIBUTE;
    } else if(strcmp(argv[1], "sendSubscribe")==0){
        action = think::Crabbitmq::SENDMESSAGESUBSCRIBE;
    } else if(strcmp(argv[1], "getSubscribe")==0){
        action = think::Crabbitmq::GETMESSAGESUBSCRIBE;
    } else if(strcmp(argv[1], "sendRouting")==0){
        action = think::Crabbitmq::SENDMESSAGEROUTING;
    } else if(strcmp(argv[1], "getRouting")==0){
        action = think::Crabbitmq::GETMESSAGEROUTING;
    } else if(strcmp(argv[1], "sendTopic")==0){
        action = think::Crabbitmq::SENDMESSAGETOPIC;
    } else if(strcmp(argv[1], "getTopic")==0){
        action = think::Crabbitmq::GETMESSAGETOPIC;
    } else if(strcmp(argv[1], "sendRpc")==0){
        action = think::Crabbitmq::SENDMESSAGERPC;
    } else if(strcmp(argv[1], "getRpc")==0){
        action = think::Crabbitmq::GETMESSAGERPC;
    }
    //move use 
      else if(strcmp(argv[1], "taskAssignment")==0){
        action = think::Crabbitmq::SENDMESSAGEFORQUEUE;
    } else if(strcmp(argv[1], "getTask")==0){
        goto getTaskLable;
    } else if(strcmp(argv[1], "testAllMissionState")==0){
        action = think::Crabbitmq::SENDMESSAGEROUTING;
    } else if(strcmp(argv[1], "getAllMissionState")==0){
        action = think::Crabbitmq::GETMESSAGEROUTING;
    } else if(strcmp(argv[1], "testSingleMissionState")==0){
        //action = think::Crabbitmq::SENDMESSAGERPC;
        action = think::Crabbitmq::SENDMESSAGEFORQUEUE;
    } else if(strcmp(argv[1], "getSingleMissionState")==0){
        //action = think::Crabbitmq::GETMESSAGERPC;
        action = think::Crabbitmq::GETMESSAGEFORQUEUE;
    } else if(strcmp(argv[1], "changeStreamTarget")==0){
        action = think::Crabbitmq::SENDMESSAGEROUTING;
    } else if(strcmp(argv[1], "mapInfo")==0){
        action = think::Crabbitmq::SENDMESSAGEROUTING;
    } else if(strcmp(argv[1], "drawInfo")==0){
        action = think::Crabbitmq::SENDMESSAGEROUTING;
    } 

    ret = START_MODULE("rabbitmq", action, moduleParam_1, passParam, returnParam);
    //STEP get errorCode
    std::cout << "errorCode:" << CErrorCode::getErrorCode(ret) << ",messagess:" << CErrorCode::getErrorMessage(ret) << std::endl;
    return 0;

    getTaskLable:
        ret = START_MODULE("rabbitmq", think::Crabbitmq::CONNECT, moduleParam_1, passParam, returnParam);
        ret = START_MODULE("rabbitmq", think::Crabbitmq::DECLAREQUEUE, moduleParam_1, passParam, returnParam);
        ret = START_MODULE("rabbitmq", think::Crabbitmq::QOSPREFETCH, moduleParam_1, passParam, returnParam);

        int i = 0;
        while(1){
            ret = START_MODULE("rabbitmq", think::Crabbitmq::GETMESSAGE, moduleParam_1, passParam, returnParam);
            std::cout << "errorCode:" << CErrorCode::getErrorCode(ret) << ",message:" << CErrorCode::getErrorMessage(ret) << std::endl;

            std::cout << swriter.write(returnParam) << std::endl;

            sleep(10);

            ret = START_MODULE("rabbitmq", think::Crabbitmq::ACKMESSAGE, moduleParam_1, passParam, returnParam);

            std::cout << "**End:" << i++ << std::endl;
        }

    ret = START_MODULE("rabbitmq", think::Crabbitmq::DISCONNECT, moduleParam_1, passParam, returnParam);
    
    return 0;
}
