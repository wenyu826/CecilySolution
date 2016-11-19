/********************************************************************
	created:	  2016/10/17
	filename: 	  CecilyLogHelp.h
	author:		  ¿ÓŒƒ”Ì
	E-Mail£∫	  hynulwy@126.com
	description:  
*********************************************************************/
#ifndef Cecily_Log_Help_H
#define Cecily_Log_Help_H

#include <log4cplus/configurator.h>
#include <log4cplus/socketappender.h>
#include <log4cplus/helpers/socket.h>
#include <log4cplus/thread/threads.h>
#include <log4cplus/spi/loggingevent.h>
#include <log4cplus/logger.h>
#include <log4cplus/thread/syncprims.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/stringhelper.h>

#include <cstdlib>
#include <list>
#include <iostream>
#include <log4cplus/configurator.h>
#include <log4cplus/socketappender.h>
#include <log4cplus/helpers/socket.h>
#include <log4cplus/thread/threads.h>
#include <log4cplus/spi/loggingevent.h>
#include <log4cplus/logger.h>
#include <log4cplus/thread/syncprims.h>

#include <log4cplus/logger.h>
#include <log4cplus/appender.h>
#include <log4cplus/hierarchy.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/spi/loggerimpl.h>

#ifdef _DEBUG
#pragma comment(lib,"log4cplusD.lib")
#else
#pragma comment(lib,"log4cplus.lib")
#endif



using namespace std;
using namespace log4cplus;
using namespace log4cplus::helpers;

#endif