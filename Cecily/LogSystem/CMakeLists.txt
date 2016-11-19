
cmake_minimum_required (VERSION 3.0)
set(TargetName CecilyLogSystem)
project(${TargetName})





if(CMAKE_CL_64)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../bin/x64)
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../bin/x64)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../SDK/ThirdLibrary/lib/log4cplus/x64/${CMAKE_BUILD_TYPE})
else(CMAKE_CL_64)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../bin/x86)
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../bin/x86)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../SDK/ThirdLibrary/lib/log4cplus/x86/${CMAKE_BUILD_TYPE})
endif(CMAKE_CL_64)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../SDK/log4cplus/include)


add_definitions(-D_WIN32_WINNT=${_WIN32_WINNT})
add_definitions(-DCLEARLOGSYSTEM_EXPORTS)

FILE(GLOB LOG_SYSTEM_SRC *.h *.cpp)

if(BUILD_SHARED_LIBS)
add_library(${TargetName} SHARED ${LOG_SYSTEM_SRC})
else(BUILD_SHARED_LIBS)
add_library(${TargetName} STATIC ${LOG_SYSTEM_SRC})
endif(BUILD_SHARED_LIBS)

target_link_libraries(${TargetName} ws2_32.lib)

add_dependencies(${TargetName} log4cplus)

