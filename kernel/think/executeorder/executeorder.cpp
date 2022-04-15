#include "kernel/think/executeorder/executeorder.hpp"
#include "kernel/common/logger/logger.hpp"
#include "kernel/common/tools/tools.hpp"
#include <iostream>
#include <stdio.h>
#include <unistd.h>

//ERROR_CODE think::Cexecuteorder::XXXXXX(think::Cexecuteorder::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
//    //check is error happened at last STEP
//    if(CErrorCode::isErrorHappened(ret)){
//        return ret;
//    }
//
//    try{
//        return ERR_THINK_XXXXXX_BLOCK;
//    } catch (std::exception& e){
//        throw;
//        return ERR_THINK_XXXXXX_EXCEPTION;
//    } catch (...){
//       ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
//        throw;
//        return ERR_THINK_XXXXXX_EXCEPTION;
//    }
//
//    return ERR_OK;
//}

void think::Cexecuteorder::Excute_GetProgress(char* screenPrint, Json::Value& param, int maxLine){
    param.append(screenPrint);
    LOGGERMISSIONSHELL(screenPrint);

    if(maxLine>0){
        Json::Value deletes;
        if(param.size()>maxLine){
            param.removeIndex(0, &deletes);
        }
    }
}

ERROR_CODE think::Cexecuteorder::Excute(think::Cexecuteorder::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    if(CErrorCode::isErrorHappened(ret)){
        return ret;
    }

    try{
        const char* command = NULL;
        Json::Value progressList;
        int loopTime = 1;
        int executTime = 0;
        int failTime = 0;

        int isErrorContinue = 1;
        int isMarkAllProgress = 0;
        int markProgressLength = 1;
        int interval = 0;
        int progressLength = 1;
        std::string commandStr;

        void (*progressCallBack)(Json::Value);
        int (*statusCheckCallBack)(); 

        progressCallBack = moduleParam->progressCallBack;
        statusCheckCallBack = moduleParam->statusCheckCallBack;
        (passParam[moduleParam->targetKey].isMember("isErrorContinue")) ? isErrorContinue = passParam[moduleParam->targetKey]["isErrorContinue"].asInt() : NULL;
        (passParam[moduleParam->targetKey].isMember("isMarkAllProgress")) ? isMarkAllProgress = passParam[moduleParam->targetKey]["isMarkAllProgress"].asInt() : NULL;
        (passParam[moduleParam->targetKey].isMember("markProgressLength")) ? markProgressLength = passParam[moduleParam->targetKey]["markProgressLength"].asInt() : NULL;
        (passParam[moduleParam->targetKey].isMember("interval")) ? interval = passParam[moduleParam->targetKey]["interval"].asInt() : NULL;
        (passParam[moduleParam->targetKey].isMember("progressLength")) ? progressLength = passParam[moduleParam->targetKey]["progressLength"].asInt() : NULL;
        if(passParam[moduleParam->targetKey].isMember("command")){
            commandStr = passParam[moduleParam->targetKey]["command"].asString();
            command = (char*)commandStr.c_str();
        }
        if(passParam[moduleParam->targetKey].isMember("loop")){
            loopTime = passParam[moduleParam->targetKey]["loop"].asInt();
        }    
        returnParam["count"] = loopTime;

        int index = 0;
        int isBreak = 0;
        do{
            char screenPrint[1024];

            //progress callback
            if(progressCallBack){
                snprintf(screenPrint, sizeof(screenPrint), "**command: %s", command);
                Excute_GetProgress(screenPrint, progressList, progressLength);
                snprintf(screenPrint, sizeof(screenPrint), "**********Loop: %d**********", index);
                Excute_GetProgress(screenPrint, progressList, progressLength);
                progressCallBack(progressList);
            }

            Json::Value tempList;

            //Excute command
            loopTime--;
            FILE* fp = NULL;
            fp = popen(command, "r");     
            if(fp != NULL){
                while(fgets(screenPrint, sizeof(screenPrint), fp) != NULL){
                    screenPrint[strlen(screenPrint)-1]=0;

                    if(markProgressLength){
                        tempList.append(screenPrint);
                        
                        if(markProgressLength>0){
                            Json::Value deletes;
                            if(tempList.size()>markProgressLength){
                                tempList.removeIndex(0, &deletes);
                            }
                        }
                    }

                    //progress callback
                    if(progressCallBack){
                        Excute_GetProgress(screenPrint, progressList, progressLength);
                        progressCallBack(progressList);
                    }

                    //status check callback
                    if(statusCheckCallBack){
                        int isStop = statusCheckCallBack();
                        if(isStop == -1){
                            isBreak = 1;
                            pclose(fp);
                            fp = NULL;
                            break;
                        }
                    }
                }
                if(!isBreak){
                    pclose(fp);
                    fp = NULL;
                }
            } else {
                failTime++;
                if(isErrorContinue){
                    ret = ERR_THINK_EXCUTEORDER_BLOCK;
                    break;
                } else {
                    continue;
                }
            }

            if(isBreak){
                break;
            }

            if(isMarkAllProgress && markProgressLength){
                char tempKey[200] = {0};
                snprintf(tempKey, sizeof(tempKey), "%s%d", "result", executTime);
                returnParam[tempKey] = tempList;
            } else if(markProgressLength) {
                returnParam["result"] = tempList;
            }          
            

            executTime ++;
            index ++;

            if(statusCheckCallBack){
                int isStop = statusCheckCallBack();
                if(isStop == -1){
                    break;
                }
            }

            sleep(interval);

            if(statusCheckCallBack){
                int isStop = statusCheckCallBack();
                if(isStop == -1){
                    break;
                }
            }
        } while(returnParam["count"] == -1 || loopTime>0);

        returnParam["execute"] = executTime;
        //returnParam["failTime"] = failTime;


    } catch (std::exception& e){
        throw;
        return ERR_THINK_EXCUTEORDER_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_THINK_EXCUTEORDER_EXCEPTION;
    }

    return ret;
}

ERROR_CODE think::Cexecuteorder::DoStart(think::Cexecuteorder::ACTION action, think::Cexecuteorder::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam){
    ERROR_CODE ret = ERR_OK;

    switch(action){
        case NORMAL:
            ret = this->Excute(moduleParam, passParam, returnParam, ret);
            break;
        case NONE:
            break;
    }

    return ret;
}

ERROR_CODE think::Cexecuteorder::Start(int action,void* param, Json::Value& passParam, Json::Value& returnParam){
    think::Cexecuteorder::ModuleParam_t* moduleParam = (think::Cexecuteorder::ModuleParam_t*)param;
    think::Cexecuteorder::ACTION actions = (think::Cexecuteorder::ACTION)action;

    return this->DoStart(actions, moduleParam, passParam, returnParam);
}
