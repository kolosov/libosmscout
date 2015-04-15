#ifndef LUM_OS_THREAD_H
#define LUM_OS_THREAD_H

/*
  This source is part of the Illumination library
  Copyright (C) 2006  Tim Teulings

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/

#include <string>

#include <Lum/Private/ImportExport.h>

#include <Lum/Features.h>

#if defined(__WIN32__) || defined(WIN32)
  #include <Lum/OS/Win32/OSAPI.h>
  #if defined(_MSC_VER)
    #pragma warning( disable : 4290 )
  #endif
#elif defined(ILLUMINATION_THREAD_PTHREAD)
  #include <pthread.h>
#endif

#include <Lum/Base/Model.h>

namespace Lum {
  namespace OS {

    class LUMAPI ThreadException
    {
    private:
      std::wstring errorText;

    public:
      ThreadException(const std::wstring& errorText);

      std::wstring GetErrorText() const;
    };

    class LUMAPI ThreadNotSupportedException : public ThreadException
    {
    public:
      ThreadNotSupportedException();
    };

    class LUMAPI Thread : public Lum::Base::MsgObject
    {
    private:
#if defined(__WIN32__) || defined(WIN32)
      HANDLE       handle;
      unsigned int id;
#elif defined(ILLUMINATION_THREAD_PTHREAD)
      pthread_t    handle;
#endif

    public:
      Thread();
      virtual ~Thread();

      virtual void Start() throw (ThreadException);

      virtual void Run() = 0;

      virtual void Join() throw (ThreadException);

      virtual void Finished();

#if defined(__WIN32__) || defined(WIN32)
      HANDLE       GetHandle() const;
#elif defined(ILLUMINATION_THREAD_PTHREAD)
      pthread_t    GetHandle() const;
#endif

      // static Thread* GetCurrent();
    private:
      Thread(const Thread& other);
    };

    class LUMAPI Mutex
    {
    private:
#if defined(__WIN32__) || defined(WIN32)
      HANDLE handle;
#elif defined(ILLUMINATION_THREAD_PTHREAD)
      pthread_mutex_t handle;
#endif
    public:
      Mutex();
      virtual ~Mutex();

      void Lock();
      void Unlock();

#if defined(__WIN32__) || defined(WIN32)
      HANDLE GetHandle() const;
#elif defined(ILLUMINATION_THREAD_PTHREAD)
      pthread_mutex_t*  GetHandle();
#endif

    private:
      Mutex(const Mutex& other);
    };

    class LUMAPI TryMutex
    {
    private:
#if defined(__WIN32__) || defined(WIN32)
      HANDLE handle;
#elif defined(ILLUMINATION_THREAD_PTHREAD)
      pthread_mutex_t handle;
#endif
    public:
      TryMutex();
      virtual ~TryMutex();

      bool TryLock();
      void Lock();
      void Unlock();

    private:
      TryMutex(const TryMutex& other);
    };

    class LUMAPI RWMutex
    {
    private:
#if defined(__WIN32__) || defined(WIN32)
      HANDLE handle;
#elif defined(ILLUMINATION_THREAD_PTHREAD)
      pthread_rwlock_t handle;
#endif
    public:
      RWMutex();
      virtual ~RWMutex();

      bool TryLockRead();
      bool TryLockWrite();
      void LockRead();
      void LockWrite();
      void Unlock();

    private:
      RWMutex(const RWMutex& other);
    };


    template <class V>
    class LUMAPI Guard
    {
    private:
      V& mutex;

    public:
      Guard(V& mutex)
      : mutex(mutex)
      {
        mutex.Lock();
      }

      virtual ~Guard()
      {
        mutex.Unlock();
      }
    };

    template <class V>
    class LUMAPI ReadGuard
    {
    private:
      V& mutex;

    public:
      ReadGuard(V& mutex)
      : mutex(mutex)
      {
        mutex.LockRead();
      }

      virtual ~ReadGuard()
      {
        mutex.Unlock();
      }
    };

    template <class V>
    class LUMAPI WriteGuard
    {
    private:
      V& mutex;

    public:
      WriteGuard(V& mutex)
      : mutex(mutex)
      {
        mutex.LockWrite();
      }

      virtual ~WriteGuard()
      {
        mutex.Unlock();
      }
    };

    class LUMAPI Condition
    {
    private:
#if defined(__WIN32__) || defined(WIN32)
      int              waiters_count;
      CRITICAL_SECTION waiters_count_lock;
      HANDLE           sema;
      HANDLE           waiters_done;
      size_t           was_broadcast;
#elif defined(ILLUMINATION_THREAD_PTHREAD)
      pthread_cond_t handle;
#endif
    public:
      Condition();
      virtual ~Condition();

      void Signal();
      void Broadcast();
      void Wait(Mutex& mutex);

    private:
      Condition(const Condition& other);
    };

    typedef Guard<Mutex>    MutexGuard;
    typedef Guard<RWMutex>  RWMutexReadGuard;
    typedef Guard<RWMutex>  RWMutexWriteGuard;
    typedef Guard<TryMutex> TryMutexGuard;
  }
}

#endif

