cmake_minimum_required (VERSION 3.0)
set(TargetName log4cplus)
project(${TargetName})

set (log4cplus_sources
  src/appenderattachableimpl.cxx
  src/appender.cxx
  src/asyncappender.cxx
  src/clogger.cxx
  src/configurator.cxx
  src/connectorthread.cxx
  src/consoleappender.cxx
  src/cygwin-win32.cxx
  src/env.cxx
  src/factory.cxx
  src/fileappender.cxx
  src/fileinfo.cxx
  src/filter.cxx
  src/global-init.cxx
  src/hierarchy.cxx
  src/hierarchylocker.cxx
  src/layout.cxx
  src/log4judpappender.cxx
  src/lockfile.cxx
  src/logger.cxx
  src/loggerimpl.cxx
  src/loggingevent.cxx
  src/loggingmacros.cxx
  src/loglevel.cxx
  src/loglog.cxx
  src/logloguser.cxx
  src/mdc.cxx
  src/ndc.cxx
  src/nullappender.cxx
  src/objectregistry.cxx
  src/patternlayout.cxx
  src/pointer.cxx
  src/property.cxx
  src/queue.cxx
  src/rootlogger.cxx
  src/sleep.cxx
  src/snprintf.cxx
  src/socketappender.cxx
  src/socketbuffer.cxx
  src/socket.cxx
  src/stringhelper.cxx
  src/stringhelper-clocale.cxx
  src/stringhelper-cxxlocale.cxx
  src/stringhelper-iconv.cxx
  src/syncprims.cxx
  src/syslogappender.cxx
  src/threads.cxx
  src/timehelper.cxx
  src/tls.cxx
  src/version.cxx)

if ("${UNIX}" OR "${CYGWIN}")
  set (log4cplus_sources ${log4cplus_sources}
    src/socket-unix.cxx)
elseif (WIN32)
  set (log4cplus_sources ${log4cplus_sources}
    src/nteventlogappender.cxx
    src/socket-win32.cxx
    src/win32consoleappender.cxx
    src/win32debugappender.cxx)
endif ()

IF(CMAKE_CL_64)
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdLibrary/lib/log4cplus/x64)
ELSE(CMAKE_CL_64)
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdLibrary/lib/log4cplus/x86)
ENDIF(CMAKE_CL_64)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)


if (WIN32)
  add_definitions (-DMINGW_HAS_SECURE_API=1)
  add_definitions (-D_WIN32_WINNT=${_WIN32_WINNT})
  add_definitions (-DINSIDE_LOG4CPLUS)
endif (WIN32)




add_library (${TargetName} STATIC ${log4cplus_sources})

if (WIN32)
  set_target_properties (${TargetName} PROPERTIES
    DEBUG_POSTFIX "D")
  target_link_libraries (${TargetName} ws2_32 advapi32)
endif ()



