#include "kernel/move/taskcenter/taskcenter.hpp"
#include "kernel/common/logger/logger.hpp"
#include "kernel/common/tools/tools.hpp"
#include <iostream>
#include <unistd.h>

#include <iostream>
#include <fstream>
#include <execinfo.h>

//ERROR_CODE move::Ctaskcenter::XXXXXX(move::Ctaskcenter::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
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

Json::Value move::Ctaskcenter::progressParam;
Json::Value move::Ctaskcenter::missionPassParam;

ERROR_CODE move::Ctaskcenter::Init(move::Ctaskcenter::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    if(CErrorCode::isErrorHappened(ret)){
        return ret;
    }

    try{
        TOOLS_INITSTRUCT(this->moduleParam_order);

    } catch (std::exception& e){
        throw;
        return ERR_MOVE_TASKCENTER_Init_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_MOVE_TASKCENTER_Init_EXCEPTION;
    }

    return ret;
}

ERROR_CODE move::Ctaskcenter::ListenMission(move::Ctaskcenter::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    if(CErrorCode::isErrorHappened(ret)){
        return ret;
    }

    try{
        if(!passParam.empty()){
            passParam.clear();
        }
        passParam = TRANSFERSTATION_GETMISSION();
        Json::StyledWriter swriter;
        std::string str = swriter.write(passParam);

        LOGGERMISSION("mission start:"+str);
        LOGGER("taskcenter@mission start:"+str);
    } catch (std::exception& e){
        throw;
        return ERR_MOVE_TASKCENTER_ListenMission_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_MOVE_TASKCENTER_ListenMission_EXCEPTION;
    }

    return ret;
}

ERROR_CODE move::Ctaskcenter::FinishMission(move::Ctaskcenter::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    try{
        Json::StyledWriter swriter;
        std::string str = swriter.write(returnParam);
        LOGGER("taskcenter@mission end:"+str);
        LOGGERMISSION("mission end:"+str);

        CLEARLOGGER();

        TRANSFERSTATION_FINISHMISSION(ret, returnParam);
    } catch (std::exception& e){
        throw;
        return ERR_MOVE_TASKCENTER_FinishMission_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_MOVE_TASKCENTER_FinishMission_EXCEPTION;
    }

    return ret;
}

void move::Ctaskcenter::StartMission_Progress(Json::Value param){
    progressParam = param;
    TRANSFERSTATION_UPDATEPROGRESS(progressParam);

    // Json::StyledWriter swriter;
    // std::string str = swriter.write(progressParam);
}

int move::Ctaskcenter::StartMission_StatusCheck(){
    common::CTransferStation::STATE state = TRANSFERSTATION_GETSTATE();
    LOGGERMISSION("mission state update:"+TRANSFERSTATION_TRANSSTATE(state));

    if(state == common::CTransferStation::STOP){
        return -1;
    }
    return state;
}

ERROR_CODE move::Ctaskcenter::StartMission(move::Ctaskcenter::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    if(CErrorCode::isErrorHappened(ret)){
        return ret;
    }

    try{
        if(!returnParam.empty()){
            returnParam.clear();
        }

        //STEP: update PassParam
        if(!missionPassParam.empty()){
            missionPassParam.clear();
        }
        missionPassParam = passParam;
     
        //STEP: init seats
        think::Cexecuteorder::ModuleParam_t *moduleParam_1 = &this->moduleParam_order;
        moduleParam_1->targetKey         = "target";
        moduleParam_1->progressCallBack  = StartMission_Progress;
        moduleParam_1->statusCheckCallBack = StartMission_StatusCheck;

        //SETP connect server
        ret = START_MODULE("executeorder", think::Cexecuteorder::NORMAL, *moduleParam_1, missionPassParam, returnParam);
        
        
    end:
        //std::cout << "errorCode:" << CErrorCode::getErrorCode(ret) << ",message:" << CErrorCode::getErrorMessage(ret) << std::endl;
        // returnParam["errorCode"] = CErrorCode::getErrorCode(ret);
        // returnParam["message"] = CErrorCode::getErrorMessage(ret);
        return ret;
    } catch (std::exception& e){
        throw;
        return ERR_MOVE_TASKCENTER_StartMission_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_MOVE_TASKCENTER_StartMission_EXCEPTION;
    }

    return ret;
}

ERROR_CODE move::Ctaskcenter::DoStart(move::Ctaskcenter::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam){
    ERROR_CODE ret = ERR_OK;

    LOGGER("taskcenter@thread start");
    
    ret = this->Init(moduleParam, passParam, returnParam, ret);

    while(!ret){
        ret = this->ListenMission(moduleParam, passParam, returnParam, ret);
        ret = this->StartMission(moduleParam, passParam, returnParam, ret);
        ret = this->FinishMission(moduleParam, passParam, returnParam, ret);
        ret = ERR_OK;
    }

    LOGGER("taskcenter@thread end");

    return ret;
}

ERROR_CODE move::Ctaskcenter::Start(Json::Value param){
    move::Ctaskcenter::ModuleParam_t moduleParam;
    TOOLS_INITSTRUCT(moduleParam);

    moduleParam.passParam = param["common"];

    Json::Value targetParam = param["taskcenter"];

    //filling param: mesqmission_queuename
    //std::string mesqmission_queuename;
    //if(targetParam.isMember("mesqmission_queuename")){
    //    mesqmission_queuename = targetParam["mesqmission_queuename"].asString();
    //    moduleParam.mesqmission_queuename = (char*)mesqmission_queuename.c_str();
    //}

    return this->DoStart(&moduleParam, moduleParam.passParam, moduleParam.returnParam);
}
