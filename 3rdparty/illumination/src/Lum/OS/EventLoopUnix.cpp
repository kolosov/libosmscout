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

#include <Lum/OS/EventLoopUnix.h>

#include <set>
#include <iostream>

#include <sys/types.h>
#include <unistd.h>

namespace Lum {
  namespace OS {

#if defined(HAVE_SELECT)
    UnixEventLoop::UnixEventLoop()
    {
      if (pipe(signalPipe)!=0) {
        std::cerr << "Cannot create signal pipe!" << std::endl;
      }
    }

    UnixEventLoop::~UnixEventLoop()
    {
      close(signalPipe[0]);
      close(signalPipe[1]);
    }

    void UnixEventLoop::SignalEventLoop()
    {
      //std::cout << "Signaling event loop..." << std::endl;
      write(signalPipe[1],"+",1);
    }

    void UnixEventLoop::HandleAsyncActions()
    {
      Guard<TryMutex> guard(asyncMutex);

      while (asyncActions.begin()!=asyncActions.end()) {
        Model::ActionRef action(*asyncActions.begin());

        asyncActions.erase(asyncActions.begin());
        //std::cout << "Trigger async action..." << std::endl;
        action->Trigger();
      }
    }

    void UnixEventLoop::AddSource(EventSource* source)
    {
      assert(source!=NULL);

      sources.push_back(source);
    }

    void UnixEventLoop::RemoveSource(EventSource* source)
    {
      sources.remove(source);
    }

    void UnixEventLoop::RemoveSource(Model::Action* action)
    {
      assert(action!=NULL);

      std::list<EventSourceRef>::iterator iter;

      iter=sources.begin();
      while (iter!=sources.end()) {
        if ((*iter)->Matches(action)) {
          iter=sources.erase(iter);
        }
        else {
          ++iter;
        }
      }
    }

    void UnixEventLoop::Run()
    {
      //std::cout << "EventLoop::Run()" << std::endl;

      loopState.push(true);

      while (loopState.top()) {
        std::list<EventSourceRef>::iterator iter;
        std::set<EventSourceRef>            actions;
        fd_set                              readSet,writeSet,exceptSet;
        int                                 ret;
        int                                 max;
        Lum::Base::SystemTime               timer;
        bool                                foundTimer=false;

        iter=sources.begin();
        while (iter!=sources.end()) {
          if ((*iter)->DataAvailable()) {
            //std::cout << "Trigger without fd or timer" << std::endl;
            actions.insert((*iter));
          }

          ++iter;
        }

        HandleAsyncActions();

        FD_ZERO(&readSet);
        FD_ZERO(&writeSet);
        FD_ZERO(&exceptSet);
        max=0;

        FD_SET(signalPipe[0],&readSet);
        max=std::max(max,signalPipe[0]);

        for (std::list<EventSourceRef>::const_iterator iter=sources.begin();
             iter!=sources.end();
             ++iter) {
          FileIO fileIO;

          if ((*iter)->GetFileIO(fileIO)) {
            if (ioRead & fileIO.conditions) {
              FD_SET(fileIO.file,&readSet);
              max=std::max(max,fileIO.file);
            }

            if (ioWrite & fileIO.conditions) {
              FD_SET(fileIO.file,&writeSet);
              max=std::max(max,fileIO.file);
            }

            if (ioExcept & fileIO.conditions) {
              FD_SET(fileIO.file,&exceptSet);
              max=std::max(max,fileIO.file);
            }
          }
        }

        iter=sources.begin();
        while (iter!=sources.end()) {
          Lum::Base::SystemTime tmp;

          if ((*iter)->GetTimeout(tmp)) {
            if (!foundTimer) {
              foundTimer=true;
              timer=tmp;
            }
            else if (tmp<timer) {
              timer=tmp;
            }
          }

          ++iter;
        }

        if (actions.size()>0) {
          timeval timeout;

          timeout.tv_sec=0;
          timeout.tv_usec=0;

          //std::cout << "Checking without wait " << max+1 << std::endl;
          ret=select(max+1,&readSet,&writeSet,&exceptSet,&timeout);
        }
        else if (foundTimer) {
          timeval timeout;

          Lum::Base::SystemTime now;

          timer.Sub(now);
          timeout.tv_sec=timer.GetTime();
          timeout.tv_usec=timer.GetMicroseconds();

          //std::cout << "Waiting with timeout " << max+1 << std::endl;
          ret=select(max+1,&readSet,&writeSet,&exceptSet,&timeout);
        }
        else {
          //std::cout << "Waiting without timeout " << max+1 << std::endl;
          ret=select(max+1,&readSet,&writeSet,&exceptSet,NULL);
        }

        //std::cout << "select returned " << ret << std::endl;

        Lum::Base::SystemTime now;

        if (ret==-1) {
          std::cerr << "Error while selecting...!" << std::endl;
        }
        else if (ret>=0) {
          if (FD_ISSET(signalPipe[0],&readSet)) {
            //std::cout << "Data for pipe!" << std::endl;
            char buffer[2];

            // We use the signals to interupt the event loop
            // and just read it to clean up the pipe content!
            if (read(signalPipe[0],(void*)buffer,(size_t)1)!=1) {
              std::cerr << "Error getting signal!" << std::endl;
            }
          }

          std::list<EventSourceRef>::iterator iter;

          iter=sources.begin();
          while (iter!=sources.end()) {
            FileIO request;

            if ((*iter)->GetFileIO(request)) {
              unsigned long conditions=0;

              if (request.conditions & ioRead && FD_ISSET(request.file,&readSet)) {
                conditions|=ioRead;
              }
              if (request.conditions & ioWrite && FD_ISSET(request.file,&writeSet)) {
                conditions|=ioWrite;
              }
              if (request.conditions & ioExcept && FD_ISSET(request.file,&exceptSet)) {
                conditions|=ioExcept;
              }

              request.conditions=conditions;

              if (conditions!=0 && (*iter)->DataAvailable(request)) {
                //std::cout << "Event for fd " << request.file << std::endl;
                actions.insert((*iter));
              }
            }

            if ((*iter)->DataAvailable(now)) {
              //std::cout << "Trigger event because of timer" << std::endl;
              actions.insert((*iter));
            }

            ++iter;
          }
        }

        HandleAsyncActions();

        for (std::set<EventSourceRef>::iterator iter=actions.begin();
             iter!=actions.end();
             ++iter) {
          //std::cout << "Triggering action..." << std::endl;
          (*iter)->Trigger();
        }

        for (std::set<EventSourceRef>::iterator iter=actions.begin();
             iter!=actions.end();
             ++iter) {
          if ((*iter)->IsFinished()) {
            RemoveSource((*iter).Get());
          }
        }
      }

      loopState.pop();

      //std::cout << "Exit!" << std::endl;
    }

    void UnixEventLoop::Exit()
    {
      assert(loopState.top());

      loopState.top()=false;
    }

    void UnixEventLoop::QueueActionForAsyncNotification(Model::Action *action)
    {
      assert(action!=NULL);

      Guard<TryMutex> guard(asyncMutex);

      asyncActions.push_back(action);

      SignalEventLoop();
    }

    void UnixEventLoop::QueueActionForEventLoop(Model::Action *action)
    {
      assert(action!=NULL);

      AddSource(new EventLoopEventSource(action));
    }

    void UnixEventLoop::AddTimer(long seconds, long microseconds,
                                 Model::Action* action)
    {
      assert(action!=NULL);

      // Convert relative time stamp to absolute timestamp
      Lum::Base::SystemTime time(seconds,microseconds);
      Lum::Base::SystemTime now;

      time.Add(now);

      AddSource(new TimerEventSource(time,action));
    }

    void UnixEventLoop::RemoveTimer(Model::Action* action)
    {
      assert(action!=NULL);

      RemoveSource(action);
    }

    void UnixEventLoop::AddFile(int file,
                                unsigned long conditions,
                                Model::Action* action)
    {
      assert(action!=NULL);

      AddSource(new FileIOEventSource(file,conditions,action));
    }
#endif
  }
}
