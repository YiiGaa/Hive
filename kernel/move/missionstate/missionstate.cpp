#include "kernel/move/missionstate/missionstate.hpp"
#include "kernel/common/logger/logger.hpp"
#include "kernel/common/tools/tools.hpp"
#include <iostream>

//ERROR_CODE move::Cmissionstate::XXXXXX(move::Cmissionstate::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
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

ERROR_CODE move::Cmissionstate::Init(move::Cmissionstate::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    if(CErrorCode::isErrorHappened(ret)){
        return ret;
    }

    try{
        //TOOLS_INITSTRUCT(this->moduleParam_rabbitmqState);
        this->moduleParam_rabbitmqState.connectAdress = moduleParam->mesqstate_adress;
        this->moduleParam_rabbitmqState.connectUser = moduleParam->mesqstate_user;
        this->moduleParam_rabbitmqState.connectPwd = moduleParam->mesqstate_pwd;
        this->moduleParam_rabbitmqState.exchange = moduleParam->mesqstate_exchange;
        this->moduleParam_rabbitmqState.channel = moduleParam->mesqstate_channel;
        this->moduleParam_rabbitmqState.exchangetype = "direct";
        this->moduleParam_rabbitmqState.bindingkey = moduleParam->mesqstate_bindingkey;
        this->moduleParam_rabbitmqState.queuenamestr = moduleParam->mesqstate_queuenamestr;
        this->moduleParam_rabbitmqState.durable = 0;
        this->moduleParam_rabbitmqState.autodelete = 1;
        this->moduleParam_rabbitmqState.noack = 1;
        this->moduleParam_rabbitmqState.isrpc = 0;
        this->moduleParam_rabbitmqState.heartbeat = 60;

    } catch (std::exception& e){
        throw;
        return ERR_MOVE_MISSIONSTATE_Init_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_MOVE_MISSIONSTATE_Init_EXCEPTION;
    }

    return ERR_OK;
}

ERROR_CODE move::Cmissionstate::ConnectStatePool(move::Cmissionstate::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    if(CErrorCode::isErrorHappened(ret)){
        return ret;
    }

    try{
        think::Crabbitmq::ModuleParam_t *moduleParam_1 = &this->moduleParam_rabbitmqState;

        //SETP connect server
        ret = START_MODULE("rabbitmq", think::Crabbitmq::CONNECT, *moduleParam_1, passParam, returnParam);
        if(CErrorCode::isErrorHappened(ret)){
            return ret;
        }

        //SETP declare exchange
        ret = START_MODULE("rabbitmq", think::Crabbitmq::DECLAREEXCHANGE, *moduleParam_1, passParam, returnParam);
        if(CErrorCode::isErrorHappened(ret)){
            return ret;
        }

        //SETP declare queue
        ret = START_MODULE("rabbitmq", think::Crabbitmq::DECLAREQUEUE, *moduleParam_1, passParam, returnParam);
    } catch (std::exception& e){
        throw;
        return ERR_MOVE_MISSIONSTATE_ConnectPool_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_MOVE_MISSIONSTATE_ConnectPool_EXCEPTION;
    }

    return ret;
}

ERROR_CODE move::Cmissionstate::DisConnectStatePool(move::Cmissionstate::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    if(CErrorCode::isErrorHappened(ret)){
        return ret;
    }

    try{
        think::Crabbitmq::ModuleParam_t *moduleParam_1 = &this->moduleParam_rabbitmqState;

        ret = START_MODULE("rabbitmq", think::Crabbitmq::DISCONNECT, *moduleParam_1, passParam, returnParam);

    } catch (std::exception& e){
        throw;
        return ERR_MOVE_MISSIONSTATE_DisConnectPool_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_MOVE_MISSIONSTATE_DisConnectPool_EXCEPTION;
    }

    return ret;
}

ERROR_CODE move::Cmissionstate::ListenState(move::Cmissionstate::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    if(CErrorCode::isErrorHappened(ret)){
        return ret;
    }

    try{
        think::Crabbitmq::ModuleParam_t *moduleParam_1 = &this->moduleParam_rabbitmqState;

        ret = START_MODULE("rabbitmq", think::Crabbitmq::GETMESSAGE, *moduleParam_1, passParam, returnParam);
        if(ret < 0){
            this->DisConnectStatePool(moduleParam, passParam, returnParam, ERR_OK);
            this->ConnectStatePool(moduleParam, passParam, returnParam, ERR_OK);
            ret = START_MODULE("rabbitmq", think::Crabbitmq::GETMESSAGE, *moduleParam_1, passParam, returnParam);
        }
        Json::StyledWriter swriter;
        std::string str =swriter.write(returnParam);
        LOGGER("missionstate@state get:"+str);

        std::string action = returnParam["action"].asString();
        common::CTransferStation::STATE state = TRANSFERSTATION_GETMISSIONSTATE();
        if(!moduleParam_1->correlationidget.compare("all") || !moduleParam_1->correlationidget.compare(TRANSFERSTATION_GETMISSIONID())){
            if(!action.compare("pause")||!action.compare("start")||!action.compare("stop")){
                TRANSFERSTATION_PUSHSTATE(returnParam);
            } else if(!action.compare("update") && 
                    (state == common::CTransferStation::ACCEPT || 
                    state == common::CTransferStation::START || 
                    state == common::CTransferStation::PAUSE)
                ){
                TRANSFERSTATION_PUSHSTATEUPDATE(returnParam);
            }
        }

    } catch (std::exception& e){
        throw;
        return ERR_MOVE_MISSIONSTATE_ListenState_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_MOVE_MISSIONSTATE_ListenState_EXCEPTION;
    }

    return ret;
}

ERROR_CODE move::Cmissionstate::AckState(move::Cmissionstate::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    if(CErrorCode::isErrorHappened(ret)){
        return ret;
    }

    try{
        think::Crabbitmq::ModuleParam_t *moduleParam_1 = &this->moduleParam_rabbitmqState;

        ret = START_MODULE("rabbitmq", think::Crabbitmq::ACKMESSAGE, *moduleParam_1, passParam, returnParam);

    } catch (std::exception& e){
        throw;
        return ERR_MOVE_MISSIONSTATE_AckState_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_MOVE_MISSIONSTATE_AckState_EXCEPTION;
    }

    return ret;
}

ERROR_CODE move::Cmissionstate::DoStart(move::Cmissionstate::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam){
    ERROR_CODE ret = ERR_OK;

    LOGGER("missionstate@thread start");
    
    ret = this->Init(moduleParam, passParam, returnParam, ret);
    ret = this->ConnectStatePool(moduleParam, passParam, returnParam, ret);

    while(!ret){
        ret = this->ListenState(moduleParam, passParam, returnParam, ret);
        //ret = this->AckState(moduleParam, passParam, returnParam, ret);
        ret = ERR_OK;
    }

    ret = this->DisConnectStatePool(moduleParam, passParam, returnParam, ret);

    LOGGER("missionstate@thread end");

    return ret;
}

ERROR_CODE move::Cmissionstate::Start(Json::Value param){
    move::Cmissionstate::ModuleParam_t moduleParam;
    //TOOLS_INITSTRUCT(moduleParam);

    moduleParam.passParam = param["common"];

    Json::Value targetParam = param["missionstate"];

    //filling param: mesqstate_exchange
    std::string mesqstate_exchange;
    if(targetParam.isMember("mesqstate_exchange")){
        mesqstate_exchange = targetParam["mesqstate_exchange"].asString();
        moduleParam.mesqstate_exchange = (char*)mesqstate_exchange.c_str();
    }

    //filling param: mesqstate_bindingkey
    std::string mesqstate_bindingkey;
    if(targetParam.isMember("mesqstate_bindingkey")){
        mesqstate_bindingkey = targetParam["mesqstate_bindingkey"].asString();
        moduleParam.mesqstate_bindingkey = (char*)mesqstate_bindingkey.c_str();
    }

    //filling param: mesqstate_queuenamestr
    std::string mesqstate_queuenamestr;
    if(targetParam.isMember("mesqstate_queuenamestr")){
        mesqstate_queuenamestr = targetParam["mesqstate_queuenamestr"].asString();
        moduleParam.mesqstate_queuenamestr = (char*)mesqstate_queuenamestr.c_str();
    }

    //filling param: mesqstate_channel
    if(targetParam.isMember("mesqstate_channel")){
        moduleParam.mesqstate_channel = targetParam["mesqstate_channel"].asInt();
    }

    //filling param: mesqstate_adress
    std::string mesqstate_adress;
    if(targetParam.isMember("mesqstate_adress")){
        moduleParam.mesqstate_adress = targetParam["mesqstate_adress"].asString();
    } else {
        moduleParam.mesqstate_adress = CONFIG_RABBITMQADDRESS;
    }

    //filling param: mesqstate_user
    std::string mesqstate_user;
    if(targetParam.isMember("mesqstate_user")){
        moduleParam.mesqstate_user = targetParam["mesqstate_user"].asString();
    } else {
        moduleParam.mesqstate_user = CONFIG_RABBITMQUSER;
    }

    //filling param: mesqstate_pwd
    std::string mesqstate_pwd;
    if(targetParam.isMember("mesqstate_pwd")){
        moduleParam.mesqstate_pwd = targetParam["mesqstate_pwd"].asString();
    } else {
        moduleParam.mesqstate_pwd = CONFIG_RABBITMQPWD;
    }

    return this->DoStart(&moduleParam, moduleParam.passParam, moduleParam.returnParam);
}
