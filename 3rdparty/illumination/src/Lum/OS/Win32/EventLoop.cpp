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

#include <Lum/OS/Win32/EventLoop.h>

#include <set>

namespace Lum {
  namespace OS {
    namespace Win32 {

      Win32EventLoop::Win32EventLoop()
      {
        //if (pipe(signalPipe)!=0) {
        //  std::cerr << "Cannot create signal pipe!" << std::endl;
        //}
      }

      Win32EventLoop::~Win32EventLoop()
      {
        //close(signalPipe[0]);
        //close(signalPipe[1]);
      }

      void Win32EventLoop::SignalEventLoop()
      {
        //std::cout << "Signaling event loop..." << std::endl;
        //write(signalPipe[1],"+",1);
      }

      void Win32EventLoop::HandleAsyncActions()
      {
        Guard<TryMutex> guard(asyncMutex);

        while (asyncActions.begin()!=asyncActions.end()) {
          Model::ActionRef action(*asyncActions.begin());

          asyncActions.erase(asyncActions.begin());
          //std::cout << "Trigger async action..." << std::endl;
          action->Trigger();
        }
      }

      void Win32EventLoop::AddSource(EventSource* source)
      {
        assert(source!=NULL);

        sources.push_back(source);
      }

      void Win32EventLoop::RemoveSource(EventSource* source)
      {
        sources.remove(source);
      }

      void Win32EventLoop::RemoveSource(Model::Action* action)
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

      void Win32EventLoop::Run()
      {
        //std::cout << "EventLoop::Run()" << std::endl;

        loopState.push(true);

        while (loopState.top()) {
          HANDLE                              handles[1];
          std::list<EventSourceRef>::iterator iter;
          std::set<EventSourceRef>            actions;
          DWORD                               result;
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
  /*
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
          }*/

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
            handles[0]=0;
            //std::cout << "Waiting with timeout 0" << std::endl;
            result=::MsgWaitForMultipleObjectsEx(0,handles,0,QS_ALLINPUT,MWMO_INPUTAVAILABLE);
          }
          else if (foundTimer) {
            Lum::Base::SystemTime now;

            timer.Sub(now);

            handles[0]=0;
            //std::cout << "Waiting with timeout " << max+1 << std::endl;
            result=::MsgWaitForMultipleObjectsEx(0,handles,timer.GetTime()*1000+timer.GetMicroseconds()/1000,QS_ALLINPUT,MWMO_INPUTAVAILABLE);
          }
          else {
            handles[0]=0;
            //std::cout << "Waiting without timeout " << std::endl;
            result=::MsgWaitForMultipleObjectsEx(0,handles,INFINITE,QS_ALLINPUT,MWMO_INPUTAVAILABLE);
          }

          //std::cout << "select returned " << ret << std::endl;

          Lum::Base::SystemTime now;

          /*
          if (FD_ISSET(signalPipe[0],&readSet)) {
            //std::cout << "Data for pipe!" << std::endl;
            char buffer[2];

            // We use the signals to interupt the event loop
            // and just read it to clean up the pipe content!
            if (read(signalPipe[0],(void*)buffer,(size_t)1)!=1) {
              std::cerr << "Error getting signal!" << std::endl;
            }
          }*/

          iter=sources.begin();
          while (iter!=sources.end()) {
            /*FileIO request;

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
            else*/ if ((*iter)->DataAvailable(now)) {
              //std::cout << "Trigger event because of timer" << std::endl;
              actions.insert((*iter));
            }

            ++iter;
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

      void Win32EventLoop::Exit()
      {
        assert(loopState.top());

        loopState.top()=false;
      }

      void Win32EventLoop::QueueActionForAsyncNotification(Model::Action *action)
      {
        assert(action!=NULL);

        Guard<TryMutex> guard(asyncMutex);

        asyncActions.push_back(action);

        SignalEventLoop();
      }

      void Win32EventLoop::QueueActionForEventLoop(Model::Action *action)
      {
        assert(action!=NULL);

        AddSource(new EventLoopEventSource(action));
      }

      void Win32EventLoop::AddTimer(long seconds, long microseconds,
                                   Model::Action* action)
      {
        assert(action!=NULL);

        // Convert relative time stamp to absolute timestamp
        Lum::Base::SystemTime time(seconds,microseconds);
        Lum::Base::SystemTime now;

        time.Add(now);

        AddSource(new TimerEventSource(time,action));
      }

      void Win32EventLoop::RemoveTimer(Model::Action* action)
      {
        assert(action!=NULL);

        RemoveSource(action);
      }

      void Win32EventLoop::AddFile(int file,
                                  unsigned long conditions,
                                  Model::Action* action)
      {
        assert(action!=NULL);

        AddSource(new FileIOEventSource(file,conditions,action));
      }
    }
  }
}
