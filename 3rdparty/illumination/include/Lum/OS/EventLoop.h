#ifndef LUM_OS_EVENTLOOP_H
#define LUM_OS_EVENTLOOP_H

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

#include <stack>
#include <vector>

#include <Lum/Base/DateTime.h>
#include <Lum/Base/Reference.h>

#include <Lum/Model/Action.h>

#include <Lum/OS/Thread.h>

namespace Lum {
  namespace OS {

    enum IOCondition {
      ioRead   = 1 << 0,
      ioWrite  = 1 << 1,
      ioExcept = 1 << 2
    };

    class LUMAPI FileIO
    {
    public:
      int           file;
      unsigned long conditions;
    };

    class LUMAPI EventSource : public Lum::Base::Referencable
    {
    public:
      virtual ~EventSource();

      virtual bool Matches(const Model::Action* action) const = 0;

      virtual bool GetTimeout(Lum::Base::SystemTime& time) const = 0;
      virtual bool GetFileIO(FileIO& fileIO) const = 0;

      virtual bool DataAvailable() const = 0;
      virtual bool DataAvailable(Lum::Base::SystemTime& time) const = 0;
      virtual bool DataAvailable(const FileIO& fileIO) const = 0;
      virtual void Trigger() = 0;
      virtual bool IsFinished() const = 0;
    };

    typedef Lum::Base::Reference<EventSource> EventSourceRef;

    class LUMAPI TimerEventSource : public EventSource
    {
    private:
      Lum::Base::SystemTime time;
      Model::ActionRef      action;

    public:
      TimerEventSource(const Lum::Base::SystemTime& time,
                       Model::Action* action);

      bool Matches(const Model::Action* action) const;

      bool GetTimeout(Lum::Base::SystemTime& time) const;
      bool GetFileIO(FileIO& fileIO) const;

      bool DataAvailable() const;
      bool DataAvailable(Lum::Base::SystemTime& time) const;
      bool DataAvailable(const FileIO& fileIO) const;
      void Trigger();
      bool IsFinished() const;
    };

    class LUMAPI FileIOEventSource : public EventSource
    {
    private:
      int              file;
      unsigned long    conditions;
      Model::ActionRef action;

    public:
      FileIOEventSource(int file,
                        unsigned long conditions,
                        Model::Action* action);

      bool Matches(const Model::Action* action) const;

      bool GetTimeout(Lum::Base::SystemTime& time) const;
      bool GetFileIO(FileIO& fileIO) const;

      bool DataAvailable() const;
      bool DataAvailable(Lum::Base::SystemTime& time) const;
      bool DataAvailable(const FileIO& fileIO) const;
      void Trigger();
      bool IsFinished() const;
    };

    class LUMAPI EventLoopEventSource : public EventSource
    {
    private:
      Model::ActionRef action;

    public:
      EventLoopEventSource(Model::Action* action);

      bool Matches(const Model::Action* action) const;

      bool GetTimeout(Lum::Base::SystemTime& time) const;
      bool GetFileIO(FileIO& fileIO) const;

      bool DataAvailable() const;
      bool DataAvailable(Lum::Base::SystemTime& time) const;
      bool DataAvailable(const FileIO& fileIO) const;
      void Trigger();
      bool IsFinished() const;
    };

    /**
      Class abstracting general EventLoop handling.

      There might be different implementations for different operating systems,
      drivers, capabilities...
    */
    class LUMAPI EventLoop
    {
    public:
      virtual ~EventLoop();

      virtual void AddSource(EventSource* source) = 0;
      virtual void RemoveSource(EventSource* source) = 0;
      virtual void RemoveSource(Model::Action* action) = 0;

      virtual void QueueActionForAsyncNotification(Model::Action *action) = 0;

      virtual void QueueActionForEventLoop(Model::Action *action) = 0;

      virtual void AddTimer(long seconds, long microseconds, Model::Action* action) = 0;
      virtual void RemoveTimer(Model::Action* action) = 0;

      virtual void AddFile(int file,
                           unsigned long conditions,
                           Model::Action* action) = 0;

      virtual void Run() = 0;
      virtual void Exit() = 0;
    };
  }
}

#endif
