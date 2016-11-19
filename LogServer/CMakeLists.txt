cmake_minimum_required (VERSION 3.0)
set(TargetName LogServer)
project(${TargetName})

file(GLOB LogServer_SRC *.h *.cpp )

add_definitions(-DWIN32 -D_CONSOLE )


set(LogServer_SRC_include ../LogSystem)

if(CMAKE_CL_64)
set(LogServer_SRC_lib ../bin/x64/${CMAKE_BUILD_TYPE})
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../bin/x64)
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../bin/x64)
else(CMAKE_CL_64)
set(LogServer_SRC_lib ../bin/x86/${CMAKE_BUILD_TYPE})
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../bin/x86)
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../bin/x86)
endif(CMAKE_CL_64)

INCLUDE_DIRECTORIES(${LogServer_SRC_include})
LINK_DIRECTORIES(${LogServer_SRC_lib})

add_executable(${TargetName}  ${LogServer_SRC})


add_dependencies(${TargetName}  CecilyLogSystem)
