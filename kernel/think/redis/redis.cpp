#include "kernel/think/redis/redis.hpp"
#include "kernel/common/logger/logger.hpp"
#include "kernel/common/tools/tools.hpp"
#include <iostream>

//ERROR_CODE think::Credis::XXXXXX(think::Credis::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
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
//    }   catch (...){
//        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
//        throw;
//        return ERR_THINK_XXXXXX_EXCEPTION;
//    }
//
//    return ERR_OK;
//}
std::vector<std::string> think::Credis::Connection_SplitStr(std::string str,std::string pattern){ 
    std::string::size_type pos;  
    std::vector<std::string> result; 
    str+=pattern;//扩展字符串以方便操作  
    int size=str.size();    
    for(int i=0; i<size; i++)  
    {    
        pos=str.find(pattern,i);    
        if(pos<size)    
        {     
            std::string s=str.substr(i,pos-i);      
            result.push_back(s);     
            i=pos+pattern.size()-1;    
        }  
    }  
    return result;
}

ERROR_CODE think::Credis::Connect(think::Credis::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    if(CErrorCode::isErrorHappened(ret)){
        return ret;
    }

    try{
        moduleParam->connect = NULL;
        moduleParam->isCluster = 0;
        std::string address = moduleParam->connectAdress;
        std::string password = moduleParam->connectPwd;
        std::vector<std::string> connectList = Connection_SplitStr(address,",");
        sw::redis::ConnectionOptions connectionOptions;

        if(connectList.size()>1){
            for(int i=0;i<connectList.size();i++){
                std::vector<std::string> tempConnect = Connection_SplitStr(connectList[i],":");
                try{
                    connectionOptions.host = tempConnect[0];  // Required.
                    connectionOptions.port = atoi(tempConnect[1].c_str()); // Optional. The default port is 6379.
                    connectionOptions.password = password; // Optional. No password by default.
                    connectionOptions.socket_timeout = std::chrono::milliseconds(5000);
                    
                    moduleParam->connectionOptions = connectionOptions;
                    moduleParam->isCluster = 1;

                    auto clusterConnect = sw::redis::RedisCluster(moduleParam->connectionOptions);
                    clusterConnect.command<void>("AUTH", password);
                    break;
                }catch(const sw::redis::Error &e){
                    if(i == connectList.size()-1){
                        ERRORLOGGER("Can not link Redis: "+address+" PassWord:"+password);
                        return ERR_THINK_REDIS_Connect_BLOCK;
                    }
                }
            }
            return ERR_OK;
        }  
        
        //single redis server
        struct timeval timeout = { 2, 500000 }; // 1.5 seconds
        redisContext *connect = NULL;
        std::cout << "link Redis:" << address << std::endl;
        std::vector<std::string> tempConnect = Connection_SplitStr(connectList[0],":");
        connect = redisConnectWithTimeout(tempConnect[0].c_str(), atoi(tempConnect[1].c_str()), timeout);

        if(connect == NULL || connectList.size()==0){
            ERRORLOGGER("Can not link Redis: "+address);
            return ERR_THINK_REDIS_Connect_BLOCK;
        }

        redisReply *reply = (redisReply *)redisCommand(connect, "AUTH %s", moduleParam->connectPwd.c_str());
        if (reply==NULL || reply->type == REDIS_REPLY_ERROR) {
            ERRORLOGGER("Redis Auth wrong: "+password);
            return ERR_THINK_REDIS_Connect_PWD_BLOCK;
        }
        moduleParam->connect = connect;

    } catch (std::exception& e){
        throw;
        return ERR_THINK_REDIS_Connect_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_THINK_REDIS_Connect_EXCEPTION;
    }

   return ERR_OK;
}

ERROR_CODE think::Credis::Excute(think::Credis::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    if(CErrorCode::isErrorHappened(ret)){
        return ret;
    }

    try{
        std::string key = moduleParam->key;
        std::string value = moduleParam->value;
        std::string address = moduleParam->connectAdress;
        std::string password = moduleParam->connectPwd;

        if(moduleParam->isCluster == 0){
            redisContext *connect = moduleParam->connect;
            if(moduleParam->field){
                std::string field = moduleParam->field;
                redisReply *reply = (redisReply *)redisCommand(connect, "HSET %s %s %s", key.c_str(), field.c_str(), value.c_str());
                std::cout<<reply->str<<std::endl;
                freeReplyObject(reply);
            } else {
                redisReply *reply = (redisReply *)redisCommand(connect, "SET %s %s", key.c_str(), value.c_str());
                freeReplyObject(reply);
            }
        } else {
            auto clusterConnect = sw::redis::RedisCluster(moduleParam->connectionOptions);
            try{
                if(moduleParam->field){
                    std::string field = moduleParam->field;
                    clusterConnect.hset(key, field, value);
                } else {
                    clusterConnect.set(key, value);
                }
            }catch(const sw::redis::IoError &err){
                ERRORLOGGER("Can not link Redis: "+address+" PassWord:"+password);
                return ERR_THINK_REDIS_Connect_BLOCK;
            }catch(const sw::redis::Error &err){
                return ERR_THINK_REDIS_Excute_EXCEPTION;
            }

        }

    } catch (std::exception& e){
        throw;
        return ERR_THINK_REDIS_Excute_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_THINK_REDIS_Excute_EXCEPTION;
    }

   return ERR_OK;
}

ERROR_CODE think::Credis::Delete(think::Credis::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    if(CErrorCode::isErrorHappened(ret)){
        return ret;
    }

    try{
        std::string field = moduleParam->field;
        std::string key = moduleParam->key;
        std::string address = moduleParam->connectAdress;
        std::string password = moduleParam->connectPwd;

        if(moduleParam->isCluster == 0){
            redisContext *connect = moduleParam->connect;
            redisReply *reply = (redisReply *)redisCommand(connect, "HDEL %s %s", key.c_str(), field.c_str());
            freeReplyObject(reply);
        } else {
            try{
                auto clusterConnect = sw::redis::RedisCluster(moduleParam->connectionOptions);
                clusterConnect.hdel(key, field);
            }catch(const sw::redis::IoError &err){
                ERRORLOGGER("Can not link Redis: "+address+" PassWord:"+password);
                return ERR_THINK_REDIS_Connect_BLOCK;
            }catch(const sw::redis::Error &err){
                return ERR_THINK_REDIS_Delete_EXCEPTION;
            }
        }

    } catch (std::exception& e){
        throw;
        return ERR_THINK_REDIS_Delete_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_THINK_REDIS_Delete_EXCEPTION;
    }

   return ERR_OK;
}

ERROR_CODE think::Credis::Expire(think::Credis::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    if(CErrorCode::isErrorHappened(ret)){
        return ret;
    }

    try{
        std::string key = moduleParam->key;
        int exprieSecond = moduleParam->exprieSecond;
        std::string address = moduleParam->connectAdress;
        std::string password = moduleParam->connectPwd;

        if(moduleParam->isCluster == 0){
            redisContext *connect = moduleParam->connect;       
            redisReply *reply = (redisReply *)redisCommand(connect, "EXPIRE %s %d", key.c_str(), exprieSecond);
            freeReplyObject(reply);
        }else{
            try{
                auto clusterConnect = sw::redis::RedisCluster(moduleParam->connectionOptions);
                clusterConnect.expire(key, std::chrono::seconds(exprieSecond));
            }catch(const sw::redis::IoError &err){
                ERRORLOGGER("Can not link Redis: "+address+" PassWord:"+password);
                return ERR_THINK_REDIS_Connect_BLOCK;
            }catch(const sw::redis::Error &err){
                return ERR_THINK_REDIS_Expire_EXCEPTION;
            }
        }

    } catch (std::exception& e){
        throw;
        return ERR_THINK_REDIS_Expire_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_THINK_REDIS_Expire_EXCEPTION;
    }

   return ERR_OK;
}

ERROR_CODE think::Credis::DisConnect(think::Credis::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    if(CErrorCode::isErrorHappened(ret)){
        return ret;
    }

    try{
        redisContext *connect = moduleParam->connect;
        if(!(connect == NULL || connect->err) ){
            redisFree(connect);
        }
    
    } catch (std::exception& e){
        throw;
        return ERR_THINK_REDIS_DisConnect_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_THINK_REDIS_DisConnect_EXCEPTION;
    }

   return ERR_OK;
}

ERROR_CODE think::Credis::DoStart(think::Credis::ACTION action, think::Credis::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam){
    ERROR_CODE ret = ERR_OK;
    
    switch(action){
        case CONNECT:
            ret = this->Connect(moduleParam, passParam, returnParam, ret);
            break;
        case DISCONNECT:
            ret = this->DisConnect(moduleParam, passParam, returnParam, ret);
            break;
        case UPDATE:
            ret = this->Excute(moduleParam, passParam, returnParam, ret);
            break;
        case EXPIRE:
            ret = this->Expire(moduleParam, passParam, returnParam, ret);
            break;
        case DELETE:
            ret = this->Delete(moduleParam, passParam, returnParam, ret);
            break;
        case ONCE:
            ret = this->Connect(moduleParam, passParam, returnParam, ret);
            ret = this->Excute(moduleParam, passParam, returnParam, ret);
            ret = this->DisConnect(moduleParam, passParam, returnParam, ret);
            break;
        case NONE:
            break;
    }

    return ret;
}

ERROR_CODE think::Credis::Start(int action, void* param, Json::Value& passParam, Json::Value& returnParam){
    think::Credis::ModuleParam_t* moduleParam = (think::Credis::ModuleParam_t*)param;
    think::Credis::ACTION actions = (think::Credis::ACTION)action;

    return this->DoStart(actions, moduleParam, passParam, returnParam);
}
