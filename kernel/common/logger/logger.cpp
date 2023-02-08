#include "kernel/config/config.hpp"
#include "kernel/common/logger/logger.hpp"
#include <time.h>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include "kernel/common/transferstation/transferstation.hpp"

#define LOGGERFORMAT "[{}][{}][{}] message:{},{},{}(),{}"

std::shared_ptr<spdlog::logger> common::CLogger::workerLogger = NULL;
std::shared_ptr<spdlog::logger> common::CLogger::missionLogger = NULL;
std::string common::CLogger::identity = "";
std::string common::CLogger::hostIp = "";
std::string common::CLogger::missionId = "";
pthread_mutex_t common::CLogger::loggerLock = PTHREAD_MUTEX_INITIALIZER; 

std::string common::CLogger::LoggerFile_getCurrentTime(){
    char tempTimeString[30];
    time_t rawtime;
    tm* timeInfo;
    time(&rawtime);
    timeInfo = localtime(&rawtime);

    snprintf(tempTimeString, sizeof(tempTimeString), "[%4d-%02d-%02d][%02d:%02d:%02d]",
        (timeInfo->tm_year + 1900),
        timeInfo->tm_mon,
        timeInfo->tm_mday,
        timeInfo->tm_hour,
        timeInfo->tm_min,
        timeInfo->tm_sec
    );

    std::string timeString(tempTimeString, tempTimeString+strlen(tempTimeString));
    return timeString;
}

void common::CLogger::InitLogger(int isWorker){
    if(isWorker == 1){
        std::string root = CONFIG_LOGGERROOT_WORKER;
        std::string head = LOGGERHEAD;
        std::string fileName = root + head + "_" + common::CLogger::identity + ".log";
        workerLogger = spdlog::rotating_logger_mt("worker log", fileName, 1024 * 1024 * 10, 1);
        if(!workerLogger){
            std::cout << " ERROR: worker log create failed!"<< std::endl;
        }
        workerLogger->flush_on(spdlog::level::err);
    } else {
        std::string root = CONFIG_LOGGERROOT_MISSION;
        std::string head = LOGGERHEAD;
        std::string fileName = root + head + "_"+ TRANSFERSTATION_GETMISSIONID()+".log";
        missionLogger = spdlog::rotating_logger_mt("mission log", fileName, 1024 * 1024 * 10, 1);
        if(!missionLogger){
            std::cout << " ERROR: mission log create failed!"<< std::endl;
        }
        common::CLogger::missionId = TRANSFERSTATION_GETMISSIONID();
        workerLogger->flush_on(spdlog::level::err);
    }
    spdlog::flush_every(std::chrono::seconds(3));
}

void common::CLogger::ClearLogger(){
    pthread_mutex_lock(&common::CLogger::loggerLock);
        if(common::CLogger::workerLogger || common::CLogger::missionLogger){
            spdlog::shutdown();
            common::CLogger::workerLogger = NULL;
            common::CLogger::missionLogger = NULL;
        }
    pthread_mutex_unlock(&common::CLogger::loggerLock);
}

void common::CLogger::LoggerFile(int isWorker, int isError, std::string logger, std::string file, std::string function, int line){
    std::string levels = isError==1?"ERROR":"INFO";

    pthread_mutex_lock(&common::CLogger::loggerLock);
        std::shared_ptr<spdlog::logger> loggerPtr = NULL;
        if(isWorker){
            if(!workerLogger){
                InitLogger(1);
            }
            loggerPtr = workerLogger;
        } else {
            if(!missionLogger){
                InitLogger(0);
            }
            loggerPtr = missionLogger;
        }

        if(loggerPtr){
            if(isError){
                loggerPtr->error(LOGGERFORMAT, common::CLogger::identity, missionId, common::CLogger::hostIp, logger, file, function, line);
            } else {
                loggerPtr->info(LOGGERFORMAT, common::CLogger::identity, missionId, common::CLogger::hostIp, logger, file, function, line);
            }
        }
    pthread_mutex_unlock(&common::CLogger::loggerLock);
    return; 
}

void common::CLogger::StraightLog(std::string logger){
    pthread_mutex_lock(&common::CLogger::loggerLock);
        std::shared_ptr<spdlog::logger> loggerPtr = NULL;
        if(!missionLogger){
            InitLogger(0);
        }
        loggerPtr = missionLogger;

        if(loggerPtr){
            loggerPtr->info(logger);
        }
    pthread_mutex_unlock(&common::CLogger::loggerLock);
    return; 
}


void common::CLogger::Logger(int isWorker, std::string logger, std::string file, std::string function, int line){
    if(isWorker){
        std::cout << LOGGERHEAD << "Worker INFO:" << logger << "," << file << "," << function << "()," << line << std::endl;
    } else {
        std::cout << LOGGERHEAD << "Mission INFO:" << logger << "," << file << "," << function << "()," << line << std::endl;
    }
    return; 
}

void common::CLogger::ErrorLogger(int isWorker, std::string logger, std::string file, std::string function, int line){
    if(isWorker){
        std::cout << LOGGERHEAD << "Worker ERROR:" << logger << "," << file << "," << function << "()," << line << std::endl;
    }else {
        std::cout << LOGGERHEAD << "Mission ERROR:" << logger << "," << file << "," << function << "()," << line << std::endl;
    }
    return; 
}

void common::CLogger::DebugLogger(std::string logger, std::string file, std::string function, int line){
    //std::cout << LOGGERHEAD << " DEBUG:" << logger << "," << file << "," << function << "()," << line << std::endl;
    std::cout << LOGGERHEAD << " DEBUG:" << logger <<  std::endl;
    return; 
}

void common::CLogger::FakeLogger(std::string logger){
    return;
}