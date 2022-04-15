#include "kernel/think/httppost/httppost.hpp"
#include "kernel/common/logger/logger.hpp"
#include "kernel/common/tools/tools.hpp"
#include <iostream>
#include <string>  
#include "curl/curl.h"

//ERROR_CODE think::Chttppost::XXXXXX(think::Chttppost::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
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
//        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
//        throw;
//        return ERR_THINK_XXXXXX_EXCEPTION;
//    }
//
//    return ERR_OK;
//}

size_t think::Chttppost::ProcessData(void *ptr, size_t size, size_t nmemb, void *stream){
    std::string *str = (std::string*)stream;   
    (*str).append((char*)ptr, size*nmemb);
    return size * nmemb;  
}

ERROR_CODE think::Chttppost::Init(think::Chttppost::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    if(CErrorCode::isErrorHappened(ret)){
        return ret;
    }
        
    try{

        curl_global_init(CURL_GLOBAL_ALL);

    } catch (std::exception& e){
        throw;
        return ERR_THINK_HTTPPOST_INIT_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_THINK_HTTPPOST_INIT_EXCEPTION;
    }

    return ERR_OK;
}

ERROR_CODE think::Chttppost::CleanUp(think::Chttppost::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    if(CErrorCode::isErrorHappened(ret)){
        return ret;
    }
        
    try{
        
        curl_global_cleanup();

    } catch (std::exception& e){
        throw;
        return ERR_THINK_HTTPPOST_CleanUp_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_THINK_HTTPPOST_CleanUp_EXCEPTION;
    }

    return ERR_OK;
}

ERROR_CODE think::Chttppost::HttpPost(think::Chttppost::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    if(CErrorCode::isErrorHappened(ret)){
        return ret;
    }

    try{   
        CURL *curl = curl_easy_init();  // init curl
        CURLcode res; // res code

        std::string url = moduleParam->url;
        std::string postParams = moduleParam->messageBody;
        std::string response;

        if (curl) {  
            // set params  
            curl_easy_setopt(curl, CURLOPT_POST, 1); // post req  
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // url  
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postParams.c_str()); // params  
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false); // if want to use https  
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false); // set peer and host verify false  
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);  
            curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);  
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ProcessData);  
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);  
            curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);  
            curl_easy_setopt(curl, CURLOPT_HEADER, 0);  //将响应头信息和相应体一起传给 write_data  
            curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);  
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);  
            curl_slist *plist = curl_slist_append(NULL, "Content-Type:application/json;charset=UTF-8");  
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, plist); 
            // start req  
            
            res = curl_easy_perform(curl); 
            if (res != CURLE_OK){
                //return ERR_THINK_HTTPPOST_INIT_BLOCK;
            } 

            Json::Reader reader;
            if(!returnParam.empty()){
                returnParam.clear();
            }
            reader.parse(response, returnParam);

            Json::StyledWriter swriter;
            std::string str =swriter.write(returnParam);
        } else {
            //return ERR_THINK_HTTPPOST_INIT_BLOCK;
        }
        
        curl_easy_cleanup(curl);  // release curl
        
    } catch (std::exception& e){
        throw;
        return ERR_THINK_HTTPPOST_POST_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_THINK_HTTPPOST_POST_EXCEPTION;
    }

    return ERR_OK;
}

ERROR_CODE think::Chttppost::HttpGet(think::Chttppost::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    if(CErrorCode::isErrorHappened(ret)){
        return ret;
    }

    try{
        //return ERR_THINK_HTTPPOST_INIT_BLOCK;
    } catch (std::exception& e){
        throw;
        return ERR_THINK_HTTPPOST_GET_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_THINK_HTTPPOST_GET_EXCEPTION;
    }

    return ERR_OK;
}

ERROR_CODE think::Chttppost::DoStart(think::Chttppost::ACTION action, think::Chttppost::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam){
    ERROR_CODE ret = ERR_OK;
    std::string requestMethod = moduleParam->requestMethod;

    if(requestMethod.compare("POST") || requestMethod.compare("Post") || requestMethod.compare("post")){
        ret = this->Init(moduleParam, passParam, returnParam, ret);
        ret = this->HttpPost(moduleParam, passParam, returnParam, ret);
        ret = this->CleanUp(moduleParam, passParam, returnParam, ret);
    }
    return ret;
}

ERROR_CODE think::Chttppost::Start(int action, void* param, Json::Value& passParam, Json::Value& returnParam){
    think::Chttppost::ModuleParam_t* moduleParam = (think::Chttppost::ModuleParam_t*)param;
    think::Chttppost::ACTION actions = (think::Chttppost::ACTION)action;

    return this->DoStart(actions, moduleParam, passParam, returnParam);
}
