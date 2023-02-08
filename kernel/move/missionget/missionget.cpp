#include "kernel/move/missionget/missionget.hpp"
#include "kernel/common/logger/logger.hpp"
#include "kernel/common/tools/tools.hpp"
#include <iostream>

//ERROR_CODE move::Cmissionget::XXXXXX(move::Cmissionget::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
//    //check is error happened at last STEP
//    if(CErrorCode::isErrorHappened(ret)){
//        return ret;
//    }
//
//    try{
//        return ERR_Move_XXXXXX_BLOCK;
//    } catch (std::exception& e){
//        throw;
//        return ERR_Move_XXXXXX_EXCEPTION;
//    } catch (...){
//        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
//        throw;
//        return ERR_Move_XXXXXX_EXCEPTION;
//    }
//
//    return ERR_OK;
//}

ERROR_CODE move::Cmissionget::Init(move::Cmissionget::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    if(CErrorCode::isErrorHappened(ret)){
        return ret;
    }

    try{
        //TOOLS_INITSTRUCT(this->moduleParam_rabbitmqMession);
        this->moduleParam_rabbitmqMession.connectAdress = moduleParam->mesqmission_adress;
        this->moduleParam_rabbitmqMession.connectUser = moduleParam->mesqmission_user;
        this->moduleParam_rabbitmqMession.connectPwd = moduleParam->mesqmission_pwd;
        this->moduleParam_rabbitmqMession.queuenamestr = moduleParam->mesqmission_queuename;
        this->moduleParam_rabbitmqMession.channel = moduleParam->mesqmission_channel;
        this->moduleParam_rabbitmqMession.durable = 1;
        this->moduleParam_rabbitmqMession.autodelete = 0;
        this->moduleParam_rabbitmqMession.noack = 0;
        this->moduleParam_rabbitmqMession.isrpc = 0;
        this->moduleParam_rabbitmqMession.heartbeat = moduleParam->mesqmission_isackfirst==0?60:0;

    } catch (std::exception& e){
        throw;
        return ERR_MOVE_MISSIONGET_Init_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_MOVE_MISSIONGET_Init_EXCEPTION;
    }

    return ERR_OK;
}

ERROR_CODE move::Cmissionget::ConnectMissionPool(move::Cmissionget::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    if(CErrorCode::isErrorHappened(ret)){
        return ret;
    }

    try{
        think::Crabbitmq::ModuleParam_t *moduleParam_1 = &this->moduleParam_rabbitmqMession;

        //SETP connect server
        ret = START_MODULE("rabbitmq", think::Crabbitmq::CONNECT, *moduleParam_1, passParam, returnParam);
        if(CErrorCode::isErrorHappened(ret)){
            return ret;
        }

        //SETP declare queue
        ret = START_MODULE("rabbitmq", think::Crabbitmq::DECLAREQUEUE, *moduleParam_1, passParam, returnParam);
        if(CErrorCode::isErrorHappened(ret)){
            return ret;
        }

        //SETP Qos prefetch
        ret = START_MODULE("rabbitmq", think::Crabbitmq::QOSPREFETCH, *moduleParam_1, passParam, returnParam);
    } catch (std::exception& e){
        throw;
        return ERR_MOVE_MISSIONGET_ConnectPool_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_MOVE_MISSIONGET_ConnectPool_EXCEPTION;
    }

    return ret;
}

ERROR_CODE move::Cmissionget::DisConnectMissionPool(move::Cmissionget::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    // if(CErrorCode::isErrorHappened(ret)){
    //     return ret;
    // }

    try{
        TRANSFERSTATION_CALLEND();

        if(!moduleParam->mesqmission_isackfirst){
            think::Crabbitmq::ModuleParam_t *moduleParam_1 = &this->moduleParam_rabbitmqMession;
            ret = START_MODULE("rabbitmq", think::Crabbitmq::DISCONNECT, *moduleParam_1, passParam, returnParam);
        }
    } catch (std::exception& e){
        throw;
        return ERR_MOVE_MISSIONGET_DisConnectPool_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_MOVE_MISSIONGET_DisConnectPool_EXCEPTION;
    }

    return ret;
}

ERROR_CODE move::Cmissionget::ListenMission(move::Cmissionget::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    if(CErrorCode::isErrorHappened(ret)){
        return ret;
    }

    try{
        think::Crabbitmq::ModuleParam_t *moduleParam_1 = &this->moduleParam_rabbitmqMession;

        ret = START_MODULE("rabbitmq", think::Crabbitmq::GETMESSAGE, *moduleParam_1, passParam, returnParam);

        // Json::StyledWriter swriter;
        // std::string str = swriter.write(returnParam);
        // std::cout<<str<<std::endl;
        // std::cout<<moduleParam_1->correlationidget<<std::endl;

        LOGGER("missionget@mission start");

        Json::Value tempReturnParam;
        if(returnParam.isMember("acceptUrl")){
            TOOLS_INITSTRUCT(this->moduleParam_httpResponse);
            think::Chttppost::ModuleParam_t *moduleParam_2 = &this->moduleParam_httpResponse;
            std::string responseUrl = returnParam["acceptUrl"].asString();
            std::string httpMethod = "post";
            std::string jobid = moduleParam_1->correlationidget;
            std::string workQueue = moduleParam->mesqmission_statequeuename;
            std::string responseStr = "{\"missionId\":\""+jobid+"\",\"state\":\"accept\",\"workQueue\":\""+workQueue+"\"}";

            moduleParam_2->url = (char *)responseUrl.c_str();
            moduleParam_2->requestMethod = (char *)httpMethod.c_str();
            moduleParam_2->messageBody = (char *)responseStr.c_str();

            LOGGER("missionget@report accept");
            LOGGER("missionget@report accept url:"+responseUrl);
            LOGGER("missionget@report accept method:"+httpMethod);
            LOGGER("missionget@report accept message:"+responseStr);

            //retry 30 time, 5s interval
            int retryTime = 30;
            while(retryTime>0 && CErrorCode::isErrorHappened(START_MODULE("httppost", think::Chttppost::NONE, *moduleParam_2, passParam, tempReturnParam))){
                sleep(6);
                retryTime--;
            }
            if(retryTime <= 0){
                ERRORLOGGER("missionget@report accept Error!!!");
            }
        }

        if(moduleParam->mesqmission_isackfirst){
            LOGGER("missionget@mission ack");
            ret = START_MODULE("rabbitmq", think::Crabbitmq::ACKMESSAGE, *moduleParam_1, passParam, returnParam);

            LOGGER("missionget@connect end");
            ret = START_MODULE("rabbitmq", think::Crabbitmq::DISCONNECT, *moduleParam_1, passParam, returnParam);
        }

        while(TRANSFERSTATION_PUSHMISSION(moduleParam_1->correlationidget, returnParam, this->moduleParam_rabbitmqMession.heartbeat) == 0){
            //std::cout << "send heartbeat"<< std::endl;
            ret = START_MODULE("rabbitmq", think::Crabbitmq::SENDHEARTBEAT, *moduleParam_1, passParam, returnParam);    
        }

    } catch (std::exception& e){
        std::cout<<e.what()<<std::endl;
        throw;
        return ERR_MOVE_MISSIONGET_ListenMission_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_MOVE_MISSIONGET_ListenMission_EXCEPTION;
    }

    return ret;
}

ERROR_CODE move::Cmissionget::AckMission(move::Cmissionget::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    // if(CErrorCode::isErrorHappened(ret)){
    //     return ret;
    // }

    try{
        if(!moduleParam->mesqmission_isackfirst){
            LOGGER("missionget@mission ack");
            think::Crabbitmq::ModuleParam_t *moduleParam_1 = &this->moduleParam_rabbitmqMession;

            ret = START_MODULE("rabbitmq", think::Crabbitmq::ACKMESSAGE, *moduleParam_1, passParam, returnParam);
        }

    } catch (std::exception& e){
        throw;
        return ERR_MOVE_MISSIONGET_AckMission_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_MOVE_MISSIONGET_AckMission_EXCEPTION;
    }

    return ret;
}

ERROR_CODE move::Cmissionget::ReportingResult(move::Cmissionget::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){

    try{
        Json::Value returnResult;
        returnResult = TRANSFERSTATION_POPMISSISON();

        LOGGER("missionget@mission end");

        Json::Value requestParam = returnResult["request"];
        if(requestParam.isMember("responseUrl")) {
            Json::StyledWriter swriter;

            TOOLS_INITSTRUCT(this->moduleParam_httpResponse);
            think::Chttppost::ModuleParam_t *moduleParam_2 = &this->moduleParam_httpResponse;
            think::Crabbitmq::ModuleParam_t *moduleParam_1 = &this->moduleParam_rabbitmqMession;
            std::string jobid = moduleParam_1->correlationidget;
            std::string responseUrl = requestParam["responseUrl"].asString();
            std::string httpMethod = "post";
            std::string workQueue = moduleParam->mesqmission_statequeuename;
            returnResult["result"]["missionId"] = jobid;
            returnResult["result"]["state"] = returnResult["state"];
            returnResult["result"]["workQueue"] = workQueue;
            

            std::string responseStr = swriter.write(returnResult["result"]);

            moduleParam_2->url = (char *)responseUrl.c_str();
            moduleParam_2->requestMethod = (char *)httpMethod.c_str();
            moduleParam_2->messageBody = (char *)responseStr.c_str();

            LOGGER("missionget@report result");
            LOGGER("missionget@report result url:"+responseUrl);
            LOGGER("missionget@report result method:"+httpMethod);
            LOGGER("missionget@report result message:"+responseStr);

            //retry 30 time, 5s interval
            int retryTime = 30;
            while(retryTime>0 && CErrorCode::isErrorHappened(START_MODULE("httppost", think::Chttppost::NONE, *moduleParam_2, passParam, returnParam))){
                sleep(6);
                retryTime--;
            }
            if(retryTime <= 0){
                ERRORLOGGER("missionget@report result Error!!!");
            }
        }

    } catch (std::exception& e){
        std::cout<<e.what()<<std::endl;
        throw;
        return ERR_MOVE_MISSIONGET_ReportingMission_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_MOVE_MISSIONGET_ReportingMission_EXCEPTION;
    }

    return ret;
}

ERROR_CODE move::Cmissionget::DoStart(move::Cmissionget::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam){
    ERROR_CODE ret = ERR_OK;

    LOGGER("missionget@thread start");
    
    ret = this->Init(moduleParam, passParam, returnParam, ret);

    ret = this->ConnectMissionPool(moduleParam, passParam, returnParam, ret);
    int loop = passParam.isMember("loop")?passParam["loop"].asInt():1;
    while(loop>0){
        ret = this->ListenMission(moduleParam, passParam, returnParam, ret);
        ret = this->AckMission(moduleParam, passParam, returnParam, ret);
        ret = this->ReportingResult(moduleParam, passParam, returnParam, ret);
        loop--;
        ret = ERR_OK;
    }
    ret = this->DisConnectMissionPool(moduleParam, passParam, returnParam, ret);

    LOGGER("missionget@thread end");

    return ret;
}

ERROR_CODE move::Cmissionget::Start(Json::Value param){
    move::Cmissionget::ModuleParam_t moduleParam;
    //TOOLS_INITSTRUCT(moduleParam);

    moduleParam.passParam = param["common"];

    Json::Value targetParam = param["missionget"];
    Json::Value stateParam = param["missionstate"];

    //filling param: mesqmission_queuename
    std::string mesqmission_queuename;
    if(targetParam.isMember("mesqmission_queuename")){
        mesqmission_queuename = targetParam["mesqmission_queuename"].asString();
        moduleParam.mesqmission_queuename = (char*)mesqmission_queuename.c_str();
    }

    //filling param: mesqmission_statequeuename
    std::string mesqmission_statequeuename;
    if(stateParam.isMember("mesqstate_queuenamestr")){
        moduleParam.mesqmission_statequeuename = stateParam["mesqstate_queuenamestr"].asString();
    } else {
        moduleParam.mesqmission_statequeuename = "";
    }

    //filling param: mesqmission_channel
    if(targetParam.isMember("mesqmission_channel")){
        moduleParam.mesqmission_channel = targetParam["mesqmission_channel"].asInt();
    }

    //filling param: mesqmission_isackfirst
    if(targetParam.isMember("mesqmission_isackfirst")){
        moduleParam.mesqmission_isackfirst = targetParam["mesqmission_isackfirst"].asInt();
    } else {
        moduleParam.mesqmission_isackfirst = 0;
    }

    //filling param: mesqmission_adress
    std::string mesqmission_adress;
    if(targetParam.isMember("mesqmission_adress")){
        moduleParam.mesqmission_adress = targetParam["mesqmission_adress"].asString();
    } else {
        moduleParam.mesqmission_adress = CONFIG_RABBITMQADDRESS;
    }

    //filling param: mesqmission_user
    std::string mesqmission_user;
    if(targetParam.isMember("mesqmission_user")){
        moduleParam.mesqmission_user = targetParam["mesqmission_user"].asString();
    } else {
        moduleParam.mesqmission_user = CONFIG_RABBITMQUSER;
    }

    //filling param: mesqmission_pwd
    std::string mesqmission_pwd;
    if(targetParam.isMember("mesqmission_pwd")){
        moduleParam.mesqmission_pwd = targetParam["mesqmission_pwd"].asString();
    } else {
        moduleParam.mesqmission_pwd = CONFIG_RABBITMQPWD;
    }

    return this->DoStart(&moduleParam, moduleParam.passParam, moduleParam.returnParam);
}
