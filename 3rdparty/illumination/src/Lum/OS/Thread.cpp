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

#include <Lum/OS/Thread.h>

#include <cassert>
#include <cstring>

#include <Lum/Base/String.h>

#if defined(__WIN32__) || defined(WIN32)
#include <winbase.h>
#include <process.h>
#endif
#include <iostream>
namespace Lum {
  namespace OS {

#if defined(__WIN32__) || defined(WIN32)
    unsigned int WINAPI ThreadWrapper(void* thread)
    {
      try {
        static_cast<Thread*>(thread)->Run();
      }
      catch (...) {
        std::cerr << "Exception caught in thread!" << std::endl;
      }
      static_cast<Thread*>(thread)->Finished();

      return 0;
    }

#elif defined(ILLUMINATION_THREAD_PTHREAD)
    void* ThreadWrapper(void* thread)
    {
      try {
        static_cast<Thread*>(thread)->Run();
      }
      catch (...) {
        std::cerr << "Exception caught in thread!" << std::endl;
      }

      static_cast<Thread*>(thread)->Finished();

      pthread_exit(NULL);

      return NULL;
    }
#endif

    ThreadException::ThreadException(const std::wstring& errorText)
    : errorText(errorText)
    {
      // no code
    }

    std::wstring ThreadException::GetErrorText() const
    {
      return errorText;
    }

    ThreadNotSupportedException::ThreadNotSupportedException()
    : ThreadException(L"Not supported")
    {
      // no code
    }

    Thread::Thread()
#if defined(__WIN32__) || defined(WIN32)
    : handle(0),id(0)
#elif defined(ILLUMINATION_THREAD_PTHREAD)
    : handle(0)
#endif
    {
      // no code
    }

    Thread::Thread(const Thread& /*other*/)
    {
      assert(true);
    }

    Thread::~Thread()
    {
#if defined(__WIN32__) || defined(WIN32)
      if (handle!=0) {
        CloseHandle(handle);
      }
#elif defined(ILLUMINATION_THREAD_PTHREAD)
      // no code
#endif
    }

    void Thread::Start() throw (ThreadException)
    {
#if defined(__WIN32__) || defined(WIN32)
      handle=(HANDLE)_beginthreadex((void*)NULL,0,ThreadWrapper,(void*)this,0,&id);

      if (handle==0) {
        // TODO: Use GetLastError and FormatMessage to get a error message
        throw ThreadException(L"Cannot create thread");
      }
#elif defined(ILLUMINATION_THREAD_PTHREAD)
      int result;

      handle=0;
      result=pthread_create(&handle,NULL,ThreadWrapper,this);

      if (result!=0) {
        std::cerr << "Cannot create thread: " << strerror(result) << std::endl;
        throw ThreadException(::Lum::Base::StringToWString(strerror(result)));
      }
#else
      throw ThreadNotSupportedException();
#endif
    }

    void Thread::Join() throw (ThreadException)
    {
#if defined(__WIN32__) || defined(WIN32)
      WaitForSingleObjectEx(handle,INFINITE,FALSE);
#elif defined(ILLUMINATION_THREAD_PTHREAD)
      void *returnValue=NULL;

      pthread_join(handle,&returnValue);
#else
      throw ThreadNotSupportedException();
#endif
    }

    void Thread::Finished()
    {
      // no code
    }

#if defined(__WIN32__) || defined(WIN32)
    HANDLE Thread::GetHandle() const
    {
      return handle;
    }
#elif defined(ILLUMINATION_THREAD_PTHREAD)
    pthread_t Thread::GetHandle() const
    {
      return handle;
    }
#endif

    Mutex::Mutex()
    {
#if defined(__WIN32__) || defined(WIN32)
      handle=CreateMutex(NULL,FALSE,NULL);
#elif defined(ILLUMINATION_THREAD_PTHREAD)
      pthread_mutex_init(&handle,NULL);
#endif
    }

    Mutex::Mutex(const Mutex& /*other*/)
    {
      assert(true);
    }

    Mutex::~Mutex()
    {
#if defined(__WIN32__) || defined(WIN32)
      CloseHandle(handle);
#elif defined(ILLUMINATION_THREAD_PTHREAD)
      pthread_mutex_destroy(&handle);
#endif
    }

    void Mutex::Lock()
    {
#if defined(__WIN32__) || defined(WIN32)
      WaitForSingleObject(handle,INFINITE);
#elif defined(ILLUMINATION_THREAD_PTHREAD)
      pthread_mutex_lock(&handle);
#endif
    }

    void Mutex::Unlock()
    {
#if defined(__WIN32__) || defined(WIN32)
      ReleaseMutex(handle);
#elif defined(ILLUMINATION_THREAD_PTHREAD)
      pthread_mutex_unlock(&handle);
#endif
    }

#if defined(__WIN32__) || defined(WIN32)
    HANDLE Mutex::GetHandle() const
    {
      return handle;
    }
#elif defined(ILLUMINATION_THREAD_PTHREAD)
    pthread_mutex_t* Mutex::GetHandle()
    {
      return &handle;
    }
#endif

    TryMutex::TryMutex()
    {
#if defined(__WIN32__) || defined(WIN32)
      handle=CreateMutex(NULL,FALSE,NULL);
#elif defined(ILLUMINATION_THREAD_PTHREAD)
      pthread_mutex_init(&handle,NULL);
#endif
    }

    TryMutex::TryMutex(const TryMutex& /*other*/)
    {
      assert(true);
    }

    TryMutex::~TryMutex()
    {
#if defined(__WIN32__) || defined(WIN32)
      CloseHandle(handle);
#elif defined(ILLUMINATION_THREAD_PTHREAD)
      pthread_mutex_destroy(&handle);
#endif
    }

    bool TryMutex::TryLock()
    {
#if defined(__WIN32__) || defined(WIN32)
      return WaitForSingleObject(handle,0)!=WAIT_OBJECT_0;
#elif defined(ILLUMINATION_THREAD_PTHREAD)
      return pthread_mutex_trylock(&handle)==0;
#endif
    }

    void TryMutex::Lock()
    {
#if defined(__WIN32__) || defined(WIN32)
      WaitForSingleObject(handle,INFINITE);
#elif defined(ILLUMINATION_THREAD_PTHREAD)
      pthread_mutex_lock(&handle);
#endif
    }

    void TryMutex::Unlock()
    {
#if defined(__WIN32__) || defined(WIN32)
      ReleaseMutex(handle);
#elif defined(ILLUMINATION_THREAD_PTHREAD)
      pthread_mutex_unlock(&handle);
#endif
    }


    RWMutex::RWMutex()
    {
#if defined(__WIN32__) || defined(WIN32)
      //InitializeCriticalSection(&handle);
#elif defined(ILLUMINATION_THREAD_PTHREAD)
      pthread_rwlock_init(&handle,NULL);
#endif
    }

    RWMutex::RWMutex(const RWMutex& /*other*/)
    {
      assert(true);
    }

    RWMutex::~RWMutex()
    {
#if defined(__WIN32__) || defined(WIN32)
      //DeleteCriticalSection(&handle);
#elif defined(ILLUMINATION_THREAD_PTHREAD)
      pthread_rwlock_destroy(&handle);
#endif
    }

    bool RWMutex::TryLockRead()
    {
#if defined(__WIN32__) || defined(WIN32)
      //return TryEnterCriticalSection(&handle)!=0;
      return false;
#elif defined(ILLUMINATION_THREAD_PTHREAD)
      return pthread_rwlock_tryrdlock(&handle)==0;
#endif
    }

    bool RWMutex::TryLockWrite()
    {
#if defined(__WIN32__) || defined(WIN32)
      //return TryEnterCriticalSection(&handle)!=0;
      return false;
#elif defined(ILLUMINATION_THREAD_PTHREAD)
      return pthread_rwlock_trywrlock(&handle)==0;
#endif
    }

    void RWMutex::LockRead()
    {
#if defined(__WIN32__) || defined(WIN32)
      //EnterCriticalSection(&handle);
#elif defined(ILLUMINATION_THREAD_PTHREAD)
      pthread_rwlock_rdlock(&handle);
#endif
    }

    void RWMutex::LockWrite()
    {
#if defined(__WIN32__) || defined(WIN32)
      //EnterCriticalSection(&handle);
#elif defined(ILLUMINATION_THREAD_PTHREAD)
      pthread_rwlock_wrlock(&handle);
#endif
    }

    void RWMutex::Unlock()
    {
#if defined(__WIN32__) || defined(WIN32)
      //LeaveCriticalSection(&handle);
#elif defined(ILLUMINATION_THREAD_PTHREAD)
      pthread_rwlock_unlock(&handle);
#endif
    }

    Condition::Condition()
    {
#if defined(__WIN32__) || defined(WIN32)
      waiters_count=0;
      was_broadcast=0;
      sema=CreateSemaphore(NULL,0,0x7fffffff,NULL);
      InitializeCriticalSection(&waiters_count_lock);
      waiters_done=CreateEvent (NULL,FALSE,FALSE,NULL);
#elif defined(ILLUMINATION_THREAD_PTHREAD)
      int result;

      result=pthread_cond_init(&handle,NULL);

      if (result!=0) {
        std::cerr << "Cannot create thread: " << strerror(result) << std::endl;
        throw ThreadException(::Lum::Base::StringToWString(strerror(result)));
      }
#endif
    }

    /*
     * See http://www.cs.wustl.edu/~schmidt/win32-cv-1.html for details about
     * the windows version of conditions
     */
     
    Condition::Condition(const Condition& /*other*/)
    {
      assert(true);
    }

    Condition::~Condition()
    {
#if defined(__WIN32__) || defined(WIN32)
#elif defined(ILLUMINATION_THREAD_PTHREAD)
      pthread_cond_destroy(&handle);
#endif
    }

    void Condition::Signal()
    {
#if defined(__WIN32__) || defined(WIN32)
    EnterCriticalSection(&waiters_count_lock);
    int have_waiters=waiters_count>0;
    LeaveCriticalSection(&waiters_count_lock);

    if (have_waiters) {
      ReleaseSemaphore(sema,1,0);
    }
#elif defined(ILLUMINATION_THREAD_PTHREAD)
      pthread_cond_signal(&handle);
#endif
    }

    void Condition::Broadcast()
    {
#if defined(__WIN32__) || defined(WIN32)
      EnterCriticalSection(&waiters_count_lock);
      int have_waiters=0;

      if (waiters_count>0) {
        was_broadcast=1;
        have_waiters=1;
      }

      if (have_waiters) {
        ReleaseSemaphore(sema,waiters_count,0);

        LeaveCriticalSection(&waiters_count_lock);

        WaitForSingleObject(waiters_done,INFINITE);
        was_broadcast=0;
      }
      else {
        LeaveCriticalSection(&waiters_count_lock);
      }
#elif defined(ILLUMINATION_THREAD_PTHREAD)
      pthread_cond_broadcast(&handle);
#endif
    }

    void Condition::Wait(Mutex& mutex)
    {
#if defined(__WIN32__) || defined(WIN32)
      // Avoid race conditions.
      EnterCriticalSection(&waiters_count_lock);
      waiters_count++;
      LeaveCriticalSection(&waiters_count_lock);

      SignalObjectAndWait(mutex.GetHandle(),sema,INFINITE,FALSE);

      EnterCriticalSection(&waiters_count_lock);

      waiters_count--;

      int last_waiter = was_broadcast && waiters_count==0;

      LeaveCriticalSection(&waiters_count_lock);

      if (last_waiter) {
        SignalObjectAndWait(waiters_done,mutex.GetHandle(),INFINITE,FALSE);
      }
      else {
        WaitForSingleObject(mutex.GetHandle(),INFINITE);
      }
#elif defined(ILLUMINATION_THREAD_PTHREAD)
      pthread_cond_wait(&handle,mutex.GetHandle());
#endif
    }

  }
}

