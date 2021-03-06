// -*- C++ -*-
//  Copyright (C) 2010-2015, Vaclav Haisman. All rights reserved.
//  
//  Redistribution and use in source and binary forms, with or without modifica-
//  tion, are permitted provided that the following conditions are met:
//  
//  1. Redistributions of  source code must  retain the above copyright  notice,
//     this list of conditions and the following disclaimer.
//  
//  2. Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//  
//  THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED WARRANTIES,
//  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
//  FITNESS  FOR A PARTICULAR  PURPOSE ARE  DISCLAIMED.  IN NO  EVENT SHALL  THE
//  APACHE SOFTWARE  FOUNDATION  OR ITS CONTRIBUTORS  BE LIABLE FOR  ANY DIRECT,
//  INDIRECT, INCIDENTAL, SPECIAL,  EXEMPLARY, OR CONSEQUENTIAL  DAMAGES (INCLU-
//  DING, BUT NOT LIMITED TO, PROCUREMENT  OF SUBSTITUTE GOODS OR SERVICES; LOSS
//  OF USE, DATA, OR  PROFITS; OR BUSINESS  INTERRUPTION)  HOWEVER CAUSED AND ON
//  ANY  THEORY OF LIABILITY,  WHETHER  IN CONTRACT,  STRICT LIABILITY,  OR TORT
//  (INCLUDING  NEGLIGENCE OR  OTHERWISE) ARISING IN  ANY WAY OUT OF THE  USE OF
//  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef LOG4CPLUS_THREAD_SYNCPRIMS_H
#define LOG4CPLUS_THREAD_SYNCPRIMS_H

#include <log4cplus/config.hxx>

#if defined (LOG4CPLUS_HAVE_PRAGMA_ONCE)
#pragma once
#endif


namespace log4cplus { namespace thread {


template <typename SyncPrim>
class SyncGuard
{
public:
    SyncGuard ();
    SyncGuard (SyncPrim const &);
    ~SyncGuard ();

    void lock ();
    void unlock ();
    void attach (SyncPrim const &);
    void attach_and_lock (SyncPrim const &);
    void detach ();

private:
    SyncPrim const * sp;

    SyncGuard (SyncGuard const &);
    SyncGuard & operator = (SyncGuard const &);
};


class ManualResetEvent;


class MutexImplBase
{
protected:
    ~MutexImplBase ();
};


class LOG4CPLUS_EXPORT Mutex
{
public:
    enum Type
    {
        DEFAULT,
        RECURSIVE
    };

    explicit Mutex (Type = RECURSIVE);
    ~Mutex ();

    void lock () const;
    void unlock () const;

private:
    MutexImplBase * mtx;

    Mutex (Mutex const &);
    Mutex & operator = (Mutex &);
};


typedef SyncGuard<Mutex> MutexGuard;


class SemaphoreImplBase
{
protected:
    ~SemaphoreImplBase ();
};


class LOG4CPLUS_EXPORT Semaphore
{
public:
    Semaphore (unsigned max, unsigned initial);
    ~Semaphore ();

    void lock () const;
    void unlock () const;

private:
    SemaphoreImplBase * sem;

    Semaphore (Semaphore const &);
    Semaphore & operator = (Semaphore const &);
};


typedef SyncGuard<Semaphore> SemaphoreGuard;


class FairMutexImplBase
{
protected:
    ~FairMutexImplBase ();
};


class LOG4CPLUS_EXPORT FairMutex
{
public:
    FairMutex ();
    ~FairMutex ();

    void lock () const;
    void unlock () const;

private:
    FairMutexImplBase * mtx;

    FairMutex (FairMutex const &);
    FairMutex & operator = (FairMutex &);
};


typedef SyncGuard<FairMutex> FairMutexGuard;


class ManualResetEventImplBase
{
protected:
    ~ManualResetEventImplBase ();
};


class LOG4CPLUS_EXPORT ManualResetEvent
{
public:
    ManualResetEvent (bool = false);
    ~ManualResetEvent ();

    void signal () const;
    void wait () const;
    bool timed_wait (unsigned long msec) const;
    void reset () const;

private:
    ManualResetEventImplBase * ev;

    ManualResetEvent (ManualResetEvent const &);
    ManualResetEvent & operator = (ManualResetEvent const &);
};


class SharedMutexImplBase
{
protected:
    ~SharedMutexImplBase ();
};


template <typename SyncPrim, void (SyncPrim:: * lock_func) () const,
    void (SyncPrim:: * unlock_func) () const>
class SyncGuardFunc
{
public:
    SyncGuardFunc (SyncPrim const &);
    ~SyncGuardFunc ();

    void lock ();
    void unlock ();
    void attach (SyncPrim const &);
    void detach ();

private:
    SyncPrim const * sp;

    SyncGuardFunc (SyncGuardFunc const &);
    SyncGuardFunc & operator = (SyncGuardFunc const &);
};


class LOG4CPLUS_EXPORT SharedMutex
{
public:
    SharedMutex ();
    ~SharedMutex ();

    void rdlock () const;
    void rdunlock () const;

    void wrlock () const;
    void wrunlock () const;

private:
    SharedMutexImplBase * sm;

    SharedMutex (SharedMutex const &);
    SharedMutex & operator = (SharedMutex const &);
};


typedef SyncGuardFunc<SharedMutex, &SharedMutex::rdlock,
    &SharedMutex::rdunlock> SharedMutexReaderGuard;


typedef SyncGuardFunc<SharedMutex, &SharedMutex::wrlock,
    &SharedMutex::wrunlock> SharedMutexWriterGuard;


//
//
//

template <typename SyncPrim>
inline
SyncGuard<SyncPrim>::SyncGuard ()
    : sp (0)
{ }


template <typename SyncPrim>
inline
SyncGuard<SyncPrim>::SyncGuard (SyncPrim const & m)
    : sp (&m)
{
    sp->lock ();
}


template <typename SyncPrim>
inline
SyncGuard<SyncPrim>::~SyncGuard ()
{
    if (sp)
        sp->unlock ();
}


template <typename SyncPrim>
inline
void
SyncGuard<SyncPrim>::lock ()
{
    sp->lock ();
}


template <typename SyncPrim>
inline
void
SyncGuard<SyncPrim>::unlock ()
{
    sp->unlock ();
}


template <typename SyncPrim>
inline
void
SyncGuard<SyncPrim>::attach (SyncPrim const & m)
{
    sp = &m;
}


template <typename SyncPrim>
inline
void
SyncGuard<SyncPrim>::attach_and_lock (SyncPrim const & m)
{
    attach (m);
    try
    {
        lock();
    }
    catch (...)
    {
        detach ();
        throw;
    }
}


template <typename SyncPrim>
inline
void
SyncGuard<SyncPrim>::detach ()
{
    sp = 0;
}


//
//
//

template <typename SyncPrim, void (SyncPrim:: * lock_func) () const,
    void (SyncPrim:: * unlock_func) () const>
inline
SyncGuardFunc<SyncPrim, lock_func, unlock_func>::SyncGuardFunc (SyncPrim const & m)
    : sp (&m)
{
    (sp->*lock_func) ();
}


template <typename SyncPrim, void (SyncPrim:: * lock_func) () const,
    void (SyncPrim:: * unlock_func) () const>
inline
SyncGuardFunc<SyncPrim, lock_func, unlock_func>::~SyncGuardFunc ()
{
    if (sp)
        (sp->*unlock_func) ();
}


template <typename SyncPrim, void (SyncPrim:: * lock_func) () const,
    void (SyncPrim:: * unlock_func) () const>
inline
void
SyncGuardFunc<SyncPrim, lock_func, unlock_func>::lock ()
{
    (sp->*lock_func) ();
}


template <typename SyncPrim, void (SyncPrim:: * lock_func) () const,
    void (SyncPrim:: * unlock_func) () const>
inline
void
SyncGuardFunc<SyncPrim, lock_func, unlock_func>::unlock ()
{
    (sp->*unlock_func) ();
}


template <typename SyncPrim, void (SyncPrim:: * lock_func) () const,
    void (SyncPrim:: * unlock_func) () const>
inline
void
SyncGuardFunc<SyncPrim, lock_func, unlock_func>::attach (SyncPrim const & m)
{
    sp = &m;
}


template <typename SyncPrim, void (SyncPrim:: * lock_func) () const,
    void (SyncPrim:: * unlock_func) () const>
inline
void
SyncGuardFunc<SyncPrim, lock_func, unlock_func>::detach ()
{
    sp = 0;
}


} } // namespace log4cplus { namespace thread { 


#endif // LOG4CPLUS_THREAD_SYNCPRIMS_H
