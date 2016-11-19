cmake_minimum_required (VERSION 3.0)
set(TargetName CecilyMQ)
project(${TargetName})

#aux_source_directory(. CecilyTCP_SRC)
file(GLOB CecilyMQ_SRC *.h *.cpp )

add_definitions(-DWIN32 -D_WINDOWS -D_USRDLL -DCLEARMQ_EXPORTS)



set(mq_include ../SDK/rabbitmq ../SDK/rabbitmq/win32/msinttypes ../LogSystem ../SDK/ThirdLibrary/include/activemq-cpp)

IF(CMAKE_CL_64)
set(mq_lib ../SDK/ThirdLibrary/lib/rabbitmq/x64/${CMAKE_BUILD_TYPE} ../bin/x64/${CMAKE_BUILD_TYPE} ../SDK/ThirdLibrary/lib/activemq-cpp/x64/${CMAKE_BUILD_TYPE})
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../bin/x64)
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../bin/x64)
ELSE(CMAKE_CL_64)
set(mq_lib ../SDK/ThirdLibrary/lib/rabbitmq/x86/${CMAKE_BUILD_TYPE} ../bin/x86/${CMAKE_BUILD_TYPE} ../SDK/ThirdLibrary/lib/activemq-cpp/x86/${CMAKE_BUILD_TYPE})
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../bin/x86)
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../bin/x86)
ENDIF(CMAKE_CL_64)

INCLUDE_DIRECTORIES(${mq_include})
LINK_DIRECTORIES(${mq_lib})


if(BUILD_SHARED_LIBS)
	add_library(${TargetName} SHARED ${CecilyMQ_SRC})
else(BUILD_SHARED_LIBS)
	add_library(${TargetName} STATIC ${CecilyMQ_SRC})
endif(BUILD_SHARED_LIBS)


add_dependencies(${TargetName} rabbitmq-static CecilyLogSystem)
