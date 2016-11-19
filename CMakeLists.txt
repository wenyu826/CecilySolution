
cmake_minimum_required (VERSION 3.0)
set(TargetName CecilySolution)
project(${TargetName})


option(Build_TEST "编译测试程序" ON)
option(BUILD_SHARED_LIBS "编译动态链接库" ON)
option(Build_LOG_SERVER "编译日志服务程序" ON)

set(_WIN32_WINNT 0x0502 CACHE STRING "Define Windows API version to use.")

#rabbitmq
if (WIN32)
set(HAVE_SELECT 1)
else()
check_symbol_exists(select sys/select.h HAVE_SELECT)
endif()
if (NOT HAVE_SELECT)
message(FATAL_ERROR "rabbitmq-c requires poll() or select() to be available")
endif()

#option(BUILD_SHARED_RabbitMQ_LIBS "Build rabbitmq-c as a shared library" ON)
#rabbitmq
###################################################log4cplus

###################################################log4cplus



add_subdirectory(TCP)
add_subdirectory(SDK)
add_subdirectory(MQ)
add_subdirectory(LogSystem)

if(Build_LOG_SERVER)
add_subdirectory(LogServer)
endif(Build_LOG_SERVER)

if(Build_TEST)
add_subdirectory(tests)
endif(Build_TEST)

