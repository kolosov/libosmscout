#ifndef LUM_OS_EVENTLOOPGMAINLOOP_H
#define LUM_OS_EVENTLOOPGMAINLOOP_H

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

#include <map>
#include <glib.h>

namespace Lum {
  namespace OS {

    class LUMAPI GMainLoopEventLoop : public EventLoop
    {
    private:
      std::list<EventSourceRef>         sources;
      std::map<EventSourceRef,GSource*> gsourceMap;
      std::stack<GMainLoop*>            loopStack;

    public:
      GMainContext*                     context;

    public:
      GMainLoopEventLoop();
      ~GMainLoopEventLoop();

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
  }
}

#endif
