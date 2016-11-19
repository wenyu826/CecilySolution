#pragma once

#include "CecilyLogHelp.h"
typedef std::list<log4cplus::thread::AbstractThreadPtr> ThreadQueueType;



class ReaperThread
	: public log4cplus::thread::AbstractThread
{
public:
	ReaperThread (log4cplus::thread::Mutex & mtx_,
		log4cplus::thread::ManualResetEvent & ev_,
		ThreadQueueType & queue_)
		: mtx (mtx_)
		, ev (ev_)
		, queue (queue_)
		, stop (false)
	{ }

	virtual
		~ReaperThread ()
	{ }

	virtual void run ();

	void signal_exit ();

private:
	log4cplus::thread::Mutex & mtx;
	log4cplus::thread::ManualResetEvent & ev;
	ThreadQueueType & queue;
	bool stop;
};

typedef log4cplus::helpers::SharedObjectPtr<ReaperThread> ReaperThreadPtr;


class Reaper
{
public:
	Reaper ()
	{
		reaper_thread = ReaperThreadPtr (new ReaperThread (mtx, ev, queue));
		reaper_thread->start ();
	}

	~Reaper ()
	{
		reaper_thread->signal_exit ();
		reaper_thread->join ();
	}

	void visit (log4cplus::thread::AbstractThreadPtr const & thread_ptr);

private:
	log4cplus::thread::Mutex mtx;
	log4cplus::thread::ManualResetEvent ev;
	ThreadQueueType queue;
	ReaperThreadPtr reaper_thread;
};

class CecilyReaper
{
public:
	CecilyReaper(void);
	~CecilyReaper(void);
};
