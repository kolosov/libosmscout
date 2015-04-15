#ifndef LUM_DLG_PROGRESS_H
#define LUM_DLG_PROGRESS_H

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

#include <Lum/Model/Action.h>
#include <Lum/Model/Number.h>

#include <Lum/OS/Thread.h>

#include <Lum/Dialog.h>

namespace Lum {
  namespace Dlg {

    class Progress;

    /**
      Base class for (abortable) background task with visual feedback. If the background
      task runs longer than a given time a progress dialog will be opened showing the
      caption and action text as (dynamically) set by the task to gether with a progress
      bar showing the current progress.

      If the task normally finishes finishedAction will be triggered. If the task gets aborted
      the abortedAction will be triggered instead.

      A task will automatically get an abort button, if there is a watcher for the abortAction.
     */
    class LUMAPI ProgressTask : public OS::Thread
    {
    private:
      bool              needsJoin;
      bool              aborted;
      bool              finished;
      mutable OS::Mutex mutex;
      Progress          *progress;
      Model::ActionRef  timer;
      Model::ActionRef  openTimer;
      Model::ActionRef  abortAction;
      Model::ActionRef  abortedAction;
      Model::ActionRef  finishedAction;

      OS::Window        *parent;
      std::wstring      caption;
      bool              updateCaption;
      std::wstring      action;
      bool              updateAction;
      double            percent;
      Model::NumberRef  percentModel;

    private:
      void Cleanup();

    public:
      ProgressTask();
      ~ProgressTask();

      void Resync(Base::Model* model, const Base::ResyncMsg& msg);

      void Start() throw (OS::ThreadException);

      void Finished();

      bool IsFinished() const;
      bool IsAborted() const;

      void SetParent(OS::Window *window);
      void SetParent(Dialog *dialog);
      void SetCaption(const std::wstring& caption);
      void SetAction(const std::wstring& action);
      void SetProgress(double progress);

      Model::Action* GetAbortedAction() const;
      Model::Action* GetFinishedAction() const;
    };
  }
}

#endif
