
cmake_minimum_required(VERSION 3.0)

SET(TargetName TestCecilyMQ)

PROJECT(${TargetName})


SET(CMAKE_MFC_FLAG 2)

ADD_DEFINITIONS(-D_AFXDLL  -DWIN32 -D_WINDOWS)

file(GLOB src_list *.h *.cpp *.rc res/*.rc2)

if(CMAKE_CL_64)
set(link_dirs ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/x64)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/x64)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/x64)
else(CMAKE_CL_64)
set(link_dirs ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/x86)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/x86)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/x86)
endif(CMAKE_CL_64)



set(libs CecilyMQ.lib)

set(include_dirs ${CMAKE_CURRENT_SOURCE_DIR}/../../MQ)

include_directories(${include_dirs})
link_directories(${link_dirs})


if(BUILD_PRECOMPILED_HEADER)
USE_MSVC_PCH(${TargetName} stdafx.h stdafx.cpp)
endif()



add_executable(${TargetName} WIN32 ${src_list})

target_link_libraries(${TargetName} ${libs})

add_dependencies(${TargetName} CecilyMQ)