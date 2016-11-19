cmake_minimum_required (VERSION 3.0)
set(TargetName CecilyTCP)
project(${TargetName})

#aux_source_directory(. CecilyTCP_SRC)
file(GLOB CecilyTCP_SRC *.h *.cpp )

#option(BUILD_TCP_SHARED "Build DLL" ON)
#option(BUILD_SHARED_LIBS "Build Share Library" ON)

add_definitions(-DWIN32 -D_WINDOWS -D_USRDLL -DCLEARTCP_EXPORTS)



set(tcp_include ../SDK/libuv1.8.0/include ../LogSystem)

IF(CMAKE_CL_64)
set(tcp_lib ../SDK/ThirdLibrary/lib/libuv/x64/${CMAKE_BUILD_TYPE} ../bin/x64/${CMAKE_BUILD_TYPE})
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../bin/x64)
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../bin/x64)
ELSE(CMAKE_CL_64)
set(tcp_lib ../SDK/ThirdLibrary/lib/libuv/x86/${CMAKE_BUILD_TYPE} ../bin/x86/${CMAKE_BUILD_TYPE})
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../bin/x86)
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../bin/x86)
ENDIF(CMAKE_CL_64)

INCLUDE_DIRECTORIES(${tcp_include})
LINK_DIRECTORIES(${tcp_lib})

if(BUILD_SHARED_LIBS)
	add_library(${TargetName} SHARED ${CecilyTCP_SRC})
else(BUILD_SHARED_LIBS)
	add_library(${TargetName} STATIC ${CecilyTCP_SRC})
endif(BUILD_SHARED_LIBS)


if (WIN32)
target_link_libraries(${TargetName} advapi32.lib iphlpapi.lib psapi.lib shell32.lib userenv.lib ws2_32.lib)
endif(WIN32)

add_dependencies(${TargetName} libuv1.8.0 CecilyLogSystem)
