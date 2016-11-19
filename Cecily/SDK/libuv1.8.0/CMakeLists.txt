cmake_minimum_required (VERSION 3.0)
set(TargetName libuv1.8.0)
project(${TargetName})

#CMAKE_CURRENT_SOURCE_DIR


add_definitions(-DWIN32 -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -D_GNU_SOURCE )


file(GLOB libuv_src  ${CMAKE_CURRENT_SOURCE_DIR}/include/*.* ${CMAKE_CURRENT_SOURCE_DIR}/win/*.* ${CMAKE_CURRENT_SOURCE_DIR}/*.*)

IF(CMAKE_CL_64)
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdLibrary/lib/libuv/x64)
ELSE(CMAKE_CL_64)
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdLibrary/lib/libuv/x86)
ENDIF(CMAKE_CL_64)

set(libuv_include 
${CMAKE_CURRENT_SOURCE_DIR}/include
${CMAKE_CURRENT_SOURCE_DIR}
)

include_directories(${libuv_include})

add_library(${TargetName} STATIC ${libuv_src})

