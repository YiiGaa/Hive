#include "kernel/move/missionprogress/missionprogress.hpp"
#include "kernel/common/logger/logger.hpp"
#include "kernel/common/tools/tools.hpp"
#include <iostream>

#include <unistd.h>
#include <time.h>

//ERROR_CODE move::Cmissionprogress::XXXXXX(move::Cmissionprogress::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
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

ERROR_CODE move::Cmissionprogress::Init(move::Cmissionprogress::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    if(CErrorCode::isErrorHappened(ret)){
        return ret;
    }

    try{
        this->moduleParam_redisWatchdog.connectAdress = moduleParam->watchdog_adress;
        this->moduleParam_redisWatchdog.connectPwd = moduleParam->watchdog_pwd;
        this->moduleParam_redisWatchdog.exprieSecond = 0;

        this->moduleParam_redisProgress.connectAdress = moduleParam->progress_adress;
        this->moduleParam_redisProgress.connectPwd = moduleParam->progress_pwd;
        this->moduleParam_redisProgress.exprieSecond = moduleParam->progress_timer>60?moduleParam->progress_timer:60;

        passParam["pid"] = (int)getpid();

    } catch (std::exception& e){
        throw;
        return ERR_MOVE_MISSIONPROGRESS_Init_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_MOVE_MISSIONPROGRESS_Init_EXCEPTION;
    }

    return ERR_OK;
}

ERROR_CODE move::Cmissionprogress::ConnectProgressPool(move::Cmissionprogress::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    if(CErrorCode::isErrorHappened(ret)){
        return ret;
    }

    try{
        //connect progress redis
        think::Credis::ModuleParam_t *moduleParam_1 = &this->moduleParam_redisProgress;
        ret = START_MODULE("redis", think::Credis::CONNECT, *moduleParam_1, passParam, returnParam);

        //connect watchdog redis
        think::Credis::ModuleParam_t *moduleParam_2 = &this->moduleParam_redisWatchdog;
        ret = START_MODULE("redis", think::Credis::CONNECT, *moduleParam_2, passParam, returnParam);

    } catch (std::exception& e){
        throw;
        return ERR_MOVE_MISSIONPROGRESS_ConnectPool_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_MOVE_MISSIONPROGRESS_ConnectPool_EXCEPTION;
    }

    return ret;
}

ERROR_CODE move::Cmissionprogress::DisConnectProgressPool(move::Cmissionprogress::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    if(CErrorCode::isErrorHappened(ret)){
        return ret;
    }
    ERROR_CODE innerRet = ERR_OK;

    try{
        //disconnect progress redis
        think::Credis::ModuleParam_t *moduleParam_1 = &this->moduleParam_redisProgress;
        innerRet = START_MODULE("redis", think::Credis::DISCONNECT, *moduleParam_1, passParam, returnParam);

        //disconnect watchdog redis
        think::Credis::ModuleParam_t *moduleParam_2 = &this->moduleParam_redisWatchdog;
        innerRet = START_MODULE("redis", think::Credis::DISCONNECT, *moduleParam_2, passParam, returnParam);

    } catch (std::exception& e){
        throw;
        innerRet = ERR_MOVE_MISSIONPROGRESS_DisConnectPool_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        innerRet = ERR_MOVE_MISSIONPROGRESS_DisConnectPool_EXCEPTION;
    }

    if(!ret){
        ret = innerRet;
    }

    return ret;
}

ERROR_CODE move::Cmissionprogress::GetingProgress(move::Cmissionprogress::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    if(CErrorCode::isErrorHappened(ret)){
        return ret;
    }

    try{
        passParam["progressParam"] = TRANSFERSTATION_GETPROGRESS();

        common::CTransferStation::STATE state = TRANSFERSTATION_GETMISSIONSTATE();
        passParam["state"] = TRANSFERSTATION_TRANSSTATE(state);
        
    } catch (std::exception& e){
        throw;
        return ERR_MOVE_MISSIONPROGRESS_GetingProgress_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_MOVE_MISSIONPROGRESS_GetingProgress_EXCEPTION;
    }

    return ret;
}

ERROR_CODE move::Cmissionprogress::ReportingProgress(move::Cmissionprogress::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    if(CErrorCode::isErrorHappened(ret)){
        return ret;
    }

    try{
        if(!moduleParam->progress_keyname){
            return ret;
        }
        std::string state = passParam["state"].asString();

        think::Credis::ModuleParam_t *moduleParam_1 = &this->moduleParam_redisProgress;

        Json::Value progressParam = passParam["progressParam"];
        std::string missionId;
        if(progressParam.isMember("missionId")){
            missionId = progressParam["missionId"].asString();

            char tempKey[200] = {0};
            snprintf(tempKey, sizeof(tempKey), "%s%s", moduleParam->progress_keyname, missionId.c_str());

            moduleParam_1->key = (char*)tempKey;
            moduleParam_1->field = NULL;
        } else {
            return ret;
        }

        progressParam["state"] = passParam["state"];
        progressParam["identity"] = passParam["identity"];

        Json::FastWriter swriter;
        std::string sendValue = swriter.write(progressParam);
        moduleParam_1->value = (char*)sendValue.c_str();

        //SETP sending progress
        START_MODULE("redis", think::Credis::UPDATE, *moduleParam_1, passParam, returnParam);
        //STEP set expire time
        START_MODULE("redis", think::Credis::EXPIRE, *moduleParam_1, passParam, returnParam);

    } catch (std::exception& e){
        throw;
        return ERR_MOVE_MISSIONPROGRESS_ReportingProgress_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_MOVE_MISSIONPROGRESS_ReportingProgress_EXCEPTION;
    }

    return ret;
}

ERROR_CODE move::Cmissionprogress::ReportingWatchDog(move::Cmissionprogress::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    if(CErrorCode::isErrorHappened(ret)){
        return ret;
    }

    try{       
        if(!moduleParam->watchdog_keyname){
            return ret;
        }

        think::Credis::ModuleParam_t *moduleParam_1 = &this->moduleParam_redisWatchdog;
        moduleParam_1->key = moduleParam->watchdog_keyname;
        time_t seconds = time(NULL);

        Json::Value watchDogParam;
        watchDogParam["hostName"] = passParam["hostName"];
        watchDogParam["hostIp"] = passParam["hostIp"];
        watchDogParam["pid"] = passParam["pid"];
        watchDogParam["state"] = passParam["state"];
        watchDogParam["identity"] = passParam["identity"];
        watchDogParam["timeStamp"] = static_cast<Json::UInt64> (seconds);

        Json::FastWriter swriter;
        std::string sendValue = swriter.write(watchDogParam);
        moduleParam_1->value = (char*)sendValue.c_str();

        std::string identity = passParam["identity"].asString();
        moduleParam_1->field = (char*)identity.c_str();

        ret = START_MODULE("redis", think::Credis::UPDATE, *moduleParam_1, passParam, returnParam);

    } catch (std::exception& e){
        throw;
        return ERR_MOVE_MISSIONPROGRESS_ReportingWatchDog_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_MOVE_MISSIONPROGRESS_ReportingWatchDog_EXCEPTION;
    }


    return ret;
}

ERROR_CODE move::Cmissionprogress::DoStart(move::Cmissionprogress::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam){
    ERROR_CODE ret = ERR_OK;

    LOGGER("missionprogress@thread start");
    
    ret = this->Init(moduleParam, passParam, returnParam, ret);
    ret = this->ConnectProgressPool(moduleParam, passParam, returnParam, ret);

    while(!ret){
        ret = this->GetingProgress(moduleParam, passParam, returnParam, ret);
        ret = this->ReportingProgress(moduleParam, passParam, returnParam, ret);
        ret = this->ReportingWatchDog(moduleParam, passParam, returnParam, ret);
        ret = ERR_OK;
        sleep(moduleParam->progress_timer);
    }

    ret = this->DisConnectProgressPool(moduleParam, passParam, returnParam, ret);

    LOGGER("missionprogress@thread end");

    return ret;
}

ERROR_CODE move::Cmissionprogress::Start(Json::Value param){
    move::Cmissionprogress::ModuleParam_t moduleParam;
    //TOOLS_INITSTRUCT(moduleParam);

    moduleParam.passParam = param["common"];

    Json::Value targetParam = param["missionprogress"];

    //filling param: progress_keyname
    std::string progress_keyname;
    moduleParam.progress_keyname = NULL;
    if(targetParam.isMember("progress_keyname")){
       progress_keyname = targetParam["progress_keyname"].asString();
       moduleParam.progress_keyname = (char*)progress_keyname.c_str();
    }

    std::string watchdog_keyname;
    moduleParam.watchdog_keyname = NULL;
    if(targetParam.isMember("watchdog_keyname")){
       watchdog_keyname = targetParam["watchdog_keyname"].asString();
       moduleParam.watchdog_keyname = (char*)watchdog_keyname.c_str();
    }

    //filling param: progress_timer
    int progress_timer = 10;
    if(targetParam.isMember("progress_timer")){
        progress_timer = targetParam["progress_timer"].asInt();
    }
    moduleParam.progress_timer = progress_timer;

    //filling param: watchdog_adress
    std::string watchdog_adress;
    if(targetParam.isMember("watchdog_adress")){
        moduleParam.watchdog_adress = targetParam["watchdog_adress"].asString();
    } else {
        moduleParam.watchdog_adress = CONFIG_REDISADRESS;
    }

    //filling param: watchdog_pwd
    std::string watchdog_pwd;
    if(targetParam.isMember("watchdog_pwd")){
        moduleParam.watchdog_pwd = targetParam["watchdog_pwd"].asString();
    } else {
        moduleParam.watchdog_pwd = CONFIG_REDISPWD;
    }

    //filling param: progress_adress
    std::string progress_adress;
    if(targetParam.isMember("progress_adress")){
        moduleParam.progress_adress = targetParam["progress_adress"].asString();
    } else {
        moduleParam.progress_adress = CONFIG_REDISADRESS;
    }

    //filling param: progress_pwd
    std::string progress_pwd;
    if(targetParam.isMember("progress_pwd")){
        moduleParam.progress_pwd = targetParam["progress_pwd"].asString();
    } else {
        moduleParam.progress_pwd = CONFIG_REDISPWD;
    }

    return this->DoStart(&moduleParam, moduleParam.passParam, moduleParam.returnParam);
}
