#ifndef   _LOGGER_HPP_
#define   _LOGGER_HPP_ 
#include <iostream>  
#include "pthread.h"

/*
**Please do not modify the following
######Config start######
        //logger
        #define LOGGERHEAD "worker"
        #define CONFIG_LOGGERROOT_WORKER "/log/worker/worker/"
        #define CONFIG_LOGGERROOT_MISSION "/log/worker/mission/"
######Config end######
*/

#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"

#ifdef DEBUGON
    #define LOGGER(logger) common::CLogger::Logger(1,logger,__FILE__,__FUNCTION__,__LINE__)
    #define LOGGERMISSIONSHELL(logger) common::CLogger::Logger(0,logger,__FILE__,__FUNCTION__,__LINE__)
    #define LOGGERMISSION(logger) common::CLogger::Logger(0,logger,__FILE__,__FUNCTION__,__LINE__)
    #define ERRORLOGGER(logger) common::CLogger::ErrorLogger(logger,__FILE__,__FUNCTION__,__LINE__)
    #define DEBUGLOGGER(logger) common::CLogger::DebugLogger(logger,__FILE__,__FUNCTION__,__LINE__)
    #define CLEARLOGGER() common::CLogger::ClearLogger()
#else
    #define LOGGER(logger) common::CLogger::LoggerFile(1,0,logger,__FILE__,__FUNCTION__,__LINE__)
    #define LOGGERMISSIONSHELL(logger) common::CLogger::StraightLog(logger)
    #define LOGGERMISSION(logger) common::CLogger::LoggerFile(0,0,logger,__FILE__,__FUNCTION__,__LINE__)
    #define ERRORLOGGER(logger) common::CLogger::LoggerFile(1,1,logger,__FILE__,__FUNCTION__,__LINE__)
    #define DEBUGLOGGER(logger) common::CLogger::FakeLogger(logger)
    #define CLEARLOGGER() common::CLogger::ClearLogger()
#endif

namespace common{
    class CLogger {
        public:
            static std::string identity;
            static std::string hostIp;
            static void InitLogger(int);
            static void ClearLogger();
            static void StraightLog(std::string);
            static void LoggerFile(int, int, std::string, std::string, std::string, int);
            static void FakeLogger(std::string);

            static void Logger(int, std::string, std::string, std::string, int);
            static void ErrorLogger(std::string, std::string, std::string, int);
            static void DebugLogger(std::string, std::string, std::string, int);
            
        public:    //inner use
            static std::string missionId;
            static std::shared_ptr<spdlog::logger> workerLogger;
            static std::shared_ptr<spdlog::logger> missionLogger;
            static pthread_mutex_t loggerLock;
            static std::string LoggerFile_getCurrentTime();
    };
}

#endif