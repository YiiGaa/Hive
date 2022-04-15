cmake_minimum_required(VERSION 3.5)

#the name of Compiled program
project(sample)

#Header file root directory
include_directories(${CMAKE_CURRENT_LIST_DIR}/../../../../)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../../../../kernel/common/lib/include/)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../../../../kernel/think/rabbitmq/lib/include/)

#lib file root directory
link_directories(${CMAKE_CURRENT_LIST_DIR}/../lib)

#c++ file root directory
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/ sample_src)

FILE(GLOB MODULECPP "${CMAKE_SOURCE_DIR}/../../../think/rabbitmq/*.cpp")    #rabbitmq module cpp
FILE(GLOB COMMONCPP "${CMAKE_SOURCE_DIR}/../../../common/*/*.cpp")    #all common cpp
FILE(GLOB CONFIGCPP "${CMAKE_SOURCE_DIR}/../../../config/*.cpp")    #all config cpp
#FILE(GLOB THINKLISTCPP "${CMAKE_SOURCE_DIR}/../../../think/think.cpp")    #all thinklist cpp
FILE(GLOB ROOTCPP "${CMAKE_SOURCE_DIR}/*.cpp")    #all target cpp

FILE(GLOB DEVLIB "${CMAKE_SOURCE_DIR}/../../../think/rabbitmq/devincluce/include.dev")    #rabbitmq library
FILE(GLOB DEVCOMMONLIB "${CMAKE_SOURCE_DIR}/../../../common/lib/devincluce/include.dev")    #common library

FILE(GLOB RUNTIME_LIB "${CMAKE_SOURCE_DIR}/../../../think/*/lib/*.a")    #all .a
FILE(GLOB RUNTIME_CMMONLIB "${CMAKE_SOURCE_DIR}/../../../common/lib/*/*.a")    #all .a
link_libraries(${RUNTIME_LIB} ${RUNTIME_CMMONLIB})

message("")
message("********.cpp list start********")
message("※configs:")
foreach(v ${CONFIGCPP})
    message("   ${v}")
endforeach()

message("")
message("※common:")
foreach(v ${COMMONCPP})
    message("   ${v}")
endforeach()

#message("")
#message("※think list:")
#foreach(v ${THINKLISTCPP})
#    message("   ${v}")
#endforeach()

message("")
message("※modules:")
foreach(v ${MODULECPP})
    message("   ${v}")
endforeach()

message("")
message("※lib:")
foreach(v ${RUNTIME_LIB})
    message("   ${v}")
endforeach()
foreach(v ${RUNTIME_CMMONLIB})
    message("   ${v}")
endforeach()

message("")
message("※target root:")
foreach(v ${ROOTCPP})
    message("   ${v}")
endforeach()

#Building dev lib
set(LINKER_FLAGS "delete head")
foreach(v ${DEVLIB})
    FILE(READ ${v} tmp)
    STRING( REPLACE "\n" " " tmp ${tmp} )
    set(LINKER_FLAGS "${LINKER_FLAGS} ${tmp}")
endforeach()
foreach(v ${DEVCOMMONLIB})
    FILE(READ ${v} tmp)
    STRING( REPLACE "\n" " " tmp ${tmp} )
    set(LINKER_FLAGS "${LINKER_FLAGS} ${tmp}")
endforeach()
STRING( REPLACE "delete head " "" LINKER_FLAGS ${LINKER_FLAGS} )

message("")
message("※dev lib:")
    message("   ${LINKER_FLAGS}")

message("")
message("********.cpp list end********")
message("")

#Building goals
foreach(v ${ROOTCPP})
    STRING( REGEX REPLACE "${CMAKE_SOURCE_DIR}/" "" prjName ${v} )
    STRING( REGEX REPLACE ".cpp" "" prjName ${prjName} )
    add_executable(${prjName} ${v} ${MODULECPP} ${COMMONCPP} ${CONFIGCPP} )
    target_link_libraries(${prjName} ${LINKER_FLAGS})
endforeach()


set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")             # c++11
set(CMAKE_CXX_FLAGS "-g ${CMAKE_CXX_FLAGS}")                     # 调试信息
set(CMAKE_CXX_FLAGS "-Wall ${CMAKE_CXX_FLAGS}")                  # 开启所有警告
set(CMAKE_CXX_FLAGS "-DDEBUGON ${CMAKE_CXX_FLAGS}")              # 定义debug

set(CMAKE_CXX_FLAGS_DEBUG "-O0 ${CMAKE_CXX_FLAGS}")             # 调试包不优化

