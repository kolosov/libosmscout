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

namespace Lum {
  namespace OS {

    EventSource::~EventSource()
    {
      // no code
    }

    TimerEventSource::TimerEventSource(const Lum::Base::SystemTime& time,
                                       Model::Action* action)
    {
      this->action=action;
      this->time=time;
    }

    bool TimerEventSource::Matches(const Model::Action* action) const
    {
      return this->action==action;
    }

    bool TimerEventSource::DataAvailable() const
    {
      return false;
    }

    bool TimerEventSource::DataAvailable(Lum::Base::SystemTime& time) const
    {
      return time>=this->time;
    }

    bool TimerEventSource::DataAvailable(const FileIO& fileIO) const
    {
      return false;
    }

    bool TimerEventSource::GetTimeout(Lum::Base::SystemTime& time) const
    {
      time=this->time;

      return true;
    }

    bool TimerEventSource::GetFileIO(FileIO& fileIO) const
    {
      return false;
    }

    void TimerEventSource::Trigger()
    {
      action->Trigger();
    }

    bool TimerEventSource::IsFinished() const
    {
      return true;
    }

    FileIOEventSource::FileIOEventSource(int file,
                                         unsigned long conditions,
                                         Model::Action* action)
    {
      this->action=action;
      this->file=file;
      this->conditions=conditions;
    }

    bool FileIOEventSource::Matches(const Model::Action* action) const
    {
      return this->action==action;
    }

    bool FileIOEventSource::DataAvailable() const
    {
      return false;
    }

    bool FileIOEventSource::DataAvailable(Lum::Base::SystemTime& time) const
    {
      return false;
    }

    bool FileIOEventSource::DataAvailable(const FileIO& fileIO) const
    {
      return this->file==fileIO.file && (this->conditions & fileIO.conditions);
    }

    bool FileIOEventSource::GetTimeout(Lum::Base::SystemTime& time) const
    {
      return false;
    }

    bool FileIOEventSource::GetFileIO(FileIO& fileIO) const
    {
      fileIO.file=this->file;
      fileIO.conditions=this->conditions;

      return true;
    }

    void FileIOEventSource::Trigger()
    {
      action->Trigger();
    }

    bool FileIOEventSource::IsFinished() const
    {
      return false;
    }

    EventLoopEventSource::EventLoopEventSource(Model::Action* action)
    {
      this->action=action;
    }

    bool EventLoopEventSource::Matches(const Model::Action* action) const
    {
      return this->action==action;
    }

    bool EventLoopEventSource::DataAvailable() const
    {
      return true;
    }

    bool EventLoopEventSource::DataAvailable(Lum::Base::SystemTime& time) const
    {
      return false;
    }

    bool EventLoopEventSource::DataAvailable(const FileIO& fileIO) const
    {
      return false;
    }

    bool EventLoopEventSource::GetTimeout(Lum::Base::SystemTime& time) const
    {
      return false;
    }

    bool EventLoopEventSource::GetFileIO(FileIO& fileIO) const
    {
      return false;
    }

    void EventLoopEventSource::Trigger()
    {
      action->Trigger();
    }

    bool EventLoopEventSource::IsFinished() const
    {
      return true;
    }

    EventLoop::~EventLoop()
    {
    }
  }
}
