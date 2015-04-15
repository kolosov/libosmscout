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

#include <Lum/OS/EventLoopGMainLoop.h>

namespace Lum {
  namespace OS {

    struct InternalGSource
    {
      GSource     gsource;
      EventSource *eventSource;
      EventLoop   *eventLoop;
      GPollFD     poll;
    };

    gboolean eventPrepare(GSource* source, gint* timeout)
    {
      Lum::Base::SystemTime now;
      Lum::Base::SystemTime time;
      EventSource*          eventSource=((InternalGSource*)source)->eventSource;
      bool                  result;

      *timeout=-1;

      result=eventSource->DataAvailable() || eventSource->DataAvailable(now);

      if (!result && eventSource->GetTimeout(time)) {
        time.Sub(now);
        *timeout=time.GetTime()*1000+time.GetMicroseconds()/1000;
      }

      return result;
    }

    gboolean eventCheck(GSource *source)
    {
      GTimeVal              timeval;
      FileIO                io;
      InternalGSource*      gSource=(InternalGSource*)source;
      EventSource*          eventSource=gSource->eventSource;

      if (eventSource->GetFileIO(io) && io.file==gSource->poll.fd) {
        FileIO ioResult;

        ioResult.file=io.file;
        ioResult.conditions=0;

        if (gSource->poll.revents & G_IO_IN) {
          ioResult.conditions|=ioRead;
        }
        if (gSource->poll.revents & G_IO_OUT) {
          ioResult.conditions|=ioWrite;
        }

        if (!eventSource->DataAvailable(ioResult)) {
          return false;
        }
        else {
          return true;
        }
      }
      else {
        g_source_get_current_time(source,&timeval);

        Lum::Base::SystemTime now(timeval.tv_sec,timeval.tv_usec);

        return eventSource->DataAvailable() || eventSource->DataAvailable(now);
      }
    }

    gboolean eventDispatch(GSource    *source,
                           GSourceFunc callback,
                           gpointer    user_data)
    {
      EventSourceRef eventSource=((InternalGSource*)source)->eventSource;
      EventLoop*     eventLoop=((InternalGSource*)source)->eventLoop;

      if (eventSource->IsFinished()) {
        eventLoop->RemoveSource(eventSource);
      }

      eventSource->Trigger();

      return true;
    }

    gboolean idle_callback(gpointer data)
    {
      Model::Action *action=static_cast<Model::Action*>(data);

      if (!g_source_is_destroyed(g_main_current_source())) {
        action->Trigger();
      }

      action->RemoveReference();

      return false;
    }

    static GSourceFuncs eventFuncs = {
                                      eventPrepare,
                                      eventCheck,
                                      eventDispatch,
                                      NULL
                                    };

    /*
    class AsyncActionsEventSource : public EventSource
    {
    private:
      GMainLoopEventLoop* loop;

    public:
      AsyncActionsEventSource(GMainLoopEventLoop* loop)
      : loop(loop)
      {
        // no code
      }

      bool Matches(const Model::Action* action) const
      {
        return false;
      }

      bool GetTimeout(Lum::Base::SystemTime& time) const
      {
        return false;
      }

      bool GetFileIO(FileIO& fileIO) const
      {
        return false;
      }

      bool DataAvailable() const
      {
        loop->HandleAsyncActions();

        return false;
      }

      bool DataAvailable(Lum::Base::SystemTime& time) const
      {
        loop->HandleAsyncActions();

        return false;
      }

      bool DataAvailable(const FileIO& fileIO) const
      {
        return false;
      }

      void Trigger()
      {
      }

      bool IsFinished() const
      {
        return false;
      }
    };*/

    GMainLoopEventLoop::GMainLoopEventLoop()
    {
      context=g_main_context_default();

      //AddSource(new AsyncActionsEventSource(this));
    }

    GMainLoopEventLoop::~GMainLoopEventLoop()
    {
      // We do not unref the global default context,
      // since this will break deintialisation
      // of the ORBIT part of gconf
    }

    void GMainLoopEventLoop::AddSource(EventSource* source)
    {
      assert(source!=NULL);

      InternalGSource* gSource=(InternalGSource*)g_source_new(&eventFuncs,sizeof(InternalGSource));

      gSource->eventSource=source;
      gSource->eventLoop=this;
      gSource->poll.fd=-1;
      gSource->poll.events=0;
      gSource->poll.revents=0;

      FileIO io;

      if (source->GetFileIO(io)) {
        gSource->poll.fd=io.file;
        gSource->poll.events=0;
        if (io.conditions & ioRead) {
          gSource->poll.events|=G_IO_IN;
        }
        if (io.conditions & ioWrite) {
          gSource->poll.events|=G_IO_OUT;
        }
        if (io.conditions & ioExcept) {
          gSource->poll.events|=(G_IO_HUP | G_IO_ERR);
        }

        g_source_add_poll((GSource*)gSource,&gSource->poll);
        g_source_set_priority((GSource*)gSource,-100);
        g_source_set_can_recurse((GSource*)gSource,true);
      }

      g_source_attach((GSource*)gSource,context);

      gsourceMap[source]=(GSource*)gSource;
      sources.push_back(source);
    }

    void GMainLoopEventLoop::RemoveSource(EventSource* source)
    {
      assert(source!=NULL);

      std::map<EventSourceRef,GSource*>::iterator entry=gsourceMap.find(source);

      assert(entry!=gsourceMap.end());

      GSource* gSource=entry->second;

      g_source_destroy(gSource);
      g_source_unref(gSource);

      gsourceMap.erase(source);
      sources.remove(source);
    }

    void GMainLoopEventLoop::RemoveSource(Model::Action* action)
    {
      assert(action!=NULL);

      std::list<EventSourceRef>::iterator iter;

      iter=sources.begin();
      while (iter!=sources.end()) {
        if ((*iter)->Matches(action)) {
          std::map<EventSourceRef,GSource*>::iterator entry=gsourceMap.find(*iter);

          assert(entry!=gsourceMap.end());

          GSource* gSource=entry->second;

          g_source_destroy(gSource);
          g_source_unref(gSource);
          gsourceMap.erase(entry);

          iter=sources.erase(iter);
        }
        else {
          ++iter;
        }
      }
    }

    void GMainLoopEventLoop::Run()
    {
      GMainLoop* loop;

      loop=g_main_loop_new(context,false);

      loopStack.push(loop);

      g_main_loop_run(loop);

      loopStack.pop();

      g_main_loop_unref(loop);
    }

    void GMainLoopEventLoop::Exit()
    {
      g_main_loop_quit(loopStack.top());
    }

    void GMainLoopEventLoop::QueueActionForAsyncNotification(Model::Action *action)
    {
      assert(action!=NULL);

      action->AddReference();

      GSource *source;

      source=g_idle_source_new();
      g_source_set_callback(source,idle_callback,action,NULL);
      g_source_attach(source,context);
      g_source_unref(source);
    }

    void GMainLoopEventLoop::QueueActionForEventLoop(Model::Action *action)
    {
      assert(action!=NULL);

      action->AddReference();

      GSource *source;

      source=g_idle_source_new();
      g_source_set_callback(source,idle_callback,action,NULL);
      g_source_attach(source,context);
      g_source_unref(source);
    }

    void GMainLoopEventLoop::AddTimer(long seconds, long microseconds,
                                      Model::Action* action)
    {
      assert(action!=NULL);

      // Convert relative time stamp to absolute timestamp
      Lum::Base::SystemTime time(seconds,microseconds);
      Lum::Base::SystemTime now;

      time.Add(now);

      AddSource(new TimerEventSource(time,action));
    }

    void GMainLoopEventLoop::RemoveTimer(Model::Action* action)
    {
      assert(action!=NULL);

      RemoveSource(action);
    }

    void GMainLoopEventLoop::AddFile(int file,
                                     unsigned long conditions,
                                     Model::Action* action)
    {
      assert(action!=NULL);

      AddSource(new FileIOEventSource(file,conditions,action));
    }
  }
}

