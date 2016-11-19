#include "StdAfx.h"
#include "CecilyReaper.h"

void
ReaperThread::signal_exit ()
{
	log4cplus::thread::MutexGuard guard (mtx);
	stop = true;
	ev.signal ();
}


void
ReaperThread::run ()
{
	ThreadQueueType q;

	while (true)
	{
		ev.timed_wait (30 * 1000);

		{
			log4cplus::thread::MutexGuard guard (mtx);

			// Check exit condition as the very first thing.
			if (stop)
			{
				std::cout << "Reaper thread is stopping..." << std::endl;
				return;
			}

			ev.reset ();
			q.swap (queue);
		}

		if (! q.empty ())
		{
			std::cout << "Reaper thread is reaping " << q.size () << " threads."
				<< std::endl;

			for (ThreadQueueType::iterator it = q.begin (), end_it = q.end ();
				it != end_it; ++it)
			{
				AbstractThread & t = **it;
				t.join ();
			}

			q.clear ();
		}
	}
}


void
Reaper::visit (log4cplus::thread::AbstractThreadPtr const & thread_ptr)
{
	log4cplus::thread::MutexGuard guard (mtx);
	queue.push_back (thread_ptr);
	ev.signal ();
}


CecilyReaper::CecilyReaper(void)
{
}

CecilyReaper::~CecilyReaper(void)
{
}
