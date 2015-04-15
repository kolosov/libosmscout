#ifndef LUM_OS_EVENTLOOPUNIX_H
#define LUM_OS_EVENTLOOPUNIX_H

/*
  This source is part of the Illumination library
  Copyright (C) 2008  Tim Teulings

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

#include <Lum/OS/EventLoop.h>

#include <Lum/Private/Config.h>

namespace Lum {
  namespace OS {

#if defined(HAVE_SELECT)
    /**
      A MainEvent loop implementation based on Unix select mechanism
     */
    class LUMAPI UnixEventLoop : public EventLoop
    {
    private:
      std::list<EventSourceRef>   sources;

      int                         signalPipe[2];
      TryMutex                    asyncMutex;
      std::list<Model::ActionRef> asyncActions;
      std::stack<bool>            loopState;

    private:
      void SignalEventLoop();
      void HandleAsyncActions();

    public:
      UnixEventLoop();
      ~UnixEventLoop();

      void AddSource(EventSource* source);
      void RemoveSource(EventSource* source);
      void RemoveSource(Model::Action* action);

      void QueueActionForAsyncNotification(Model::Action *action);

      void QueueActionForEventLoop(Model::Action *action);

      void AddTimer(long seconds, long microseconds, Model::Action* action);
      void RemoveTimer(Model::Action* action);

      void AddFile(int file,
                   unsigned long conditions,
                   Model::Action* action);

      void Run();
      void Exit();
    };
#endif

#if defined(HAVE_MSGWAITFORMULTIPLEOBJECTSEX)
    /**
      A MainEvent loop implementation based on Unix select mechanism
     */
    class LUMAPI Win32EventLoop : public EventLoop
    {
    private:
      std::list<EventSourceRef>   sources;

      //int                         signalPipe[2];
      TryMutex                    asyncMutex;
      std::list<Model::ActionRef> asyncActions;
      std::stack<bool>            loopState;

    private:
      void SignalEventLoop();
      void HandleAsyncActions();

    public:
      Win32EventLoop();
      ~Win32EventLoop();

      void AddSource(EventSource* source);
      void RemoveSource(EventSource* source);
      void RemoveSource(Model::Action* action);

      void QueueActionForAsyncNotification(Model::Action *action);

      void QueueActionForEventLoop(Model::Action *action);

      void AddTimer(long seconds, long microseconds, Model::Action* action);
      void RemoveTimer(Model::Action* action);

      void AddFile(int file,
                   unsigned long conditions,
                   Model::Action* action);

      void Run();
      void Exit();
    };
#endif
  }
}

#endif
