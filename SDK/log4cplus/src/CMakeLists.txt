cmake_minimum_required (VERSION 3.0)
set(TargetName log4cplus)
project(${TargetName})

set (log4cplus_sources
  appenderattachableimpl.cxx
  appender.cxx
  asyncappender.cxx
  clogger.cxx
  configurator.cxx
  connectorthread.cxx
  consoleappender.cxx
  cygwin-win32.cxx
  env.cxx
  factory.cxx
  fileappender.cxx
  fileinfo.cxx
  filter.cxx
  global-init.cxx
  hierarchy.cxx
  hierarchylocker.cxx
  layout.cxx
  log4judpappender.cxx
  lockfile.cxx
  logger.cxx
  loggerimpl.cxx
  loggingevent.cxx
  loggingmacros.cxx
  loglevel.cxx
  loglog.cxx
  logloguser.cxx
  mdc.cxx
  ndc.cxx
  nullappender.cxx
  objectregistry.cxx
  patternlayout.cxx
  pointer.cxx
  property.cxx
  queue.cxx
  rootlogger.cxx
  sleep.cxx
  snprintf.cxx
  socketappender.cxx
  socketbuffer.cxx
  socket.cxx
  stringhelper.cxx
  stringhelper-clocale.cxx
  stringhelper-cxxlocale.cxx
  stringhelper-iconv.cxx
  syncprims.cxx
  syslogappender.cxx
  threads.cxx
  timehelper.cxx
  tls.cxx
  version.cxx)

if ("${UNIX}" OR "${CYGWIN}")
  set (log4cplus_sources ${log4cplus_sources}
    socket-unix.cxx)
elseif (WIN32)
  set (log4cplus_sources ${log4cplus_sources}
    nteventlogappender.cxx
    socket-win32.cxx
    win32consoleappender.cxx
    win32debugappender.cxx)
endif ()

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../ThirdLibrary/lib/log4cplus)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)


if (WIN32)
  add_definitions (-DMINGW_HAS_SECURE_API=1)
  add_definitions (-D_WIN32_WINNT=${_WIN32_WINNT})
  add_definitions (-DINSIDE_LOG4CPLUS)
endif (WIN32)


if (WIN32)
  set_target_properties (${log4cplus} PROPERTIES
    DEBUG_POSTFIX "D")
  target_link_libraries (${log4cplus} ws2_32 advapi32)
endif ()

add_library (${TargetName} STATIC ${log4cplus_sources})



