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

#include <Lum/Dlg/Progress.h>

#include <Lum/Dlg/ActionDialog.h>

#include <Lum/ButtonRow.h>
#include <Lum/FuelGauge.h>
#include <Lum/Panel.h>
#include <Lum/Text.h>

namespace Lum {
  namespace Dlg {

    class Progress : public ActionDialog
    {
    private:
      std::wstring     caption;
      std::wstring     action;
      Model::NumberRef percent;
      Text             *captionText;
      Text             *actionText;
      Model::ActionRef abortAction;

    public:
      Progress(const std::wstring& caption,
               const std::wstring& action,
               Model::NumberRef& percent,Model::Action* abortAction)
      : caption(caption),
        action(action),
        percent(percent),
        captionText(NULL),
        actionText(NULL),
        abortAction(abortAction)
      {
        GetWindow()->SetScreenOrientationHint(OS::Window::screenOrientationBothSupported);

        if (abortAction!=NULL) {
          Observe(GetClosedAction());
          Observe(abortAction);
        }
      }

      void SetCaption(const std::wstring& caption)
      {
        this->caption=caption;
        if (captionText!=NULL) {
          captionText->SetText(caption,
                               OS::Font::italic|OS::Font::bold,Text::left,
                               OS::display->GetFont(OS::Display::fontScaleCaption1));
        }
      }

      void SetAction(const std::wstring& action)
      {
        this->action=action;
        if (actionText!=NULL) {
          actionText->SetText(action);
        }
      }

      Lum::Object* GetContent()
      {
        FuelGauge   *fuelGauge;
        Panel       *vPanel;

        vPanel=VPanel::Create(true,false);

        captionText=new Text(caption,
                             OS::Font::italic|OS::Font::bold,Text::left,
                             OS::display->GetFont(OS::Display::fontScaleCaption1));
        captionText->SetFlex(true,false);
        captionText->SetMinWidth(Lum::Base::Size::pixel,
                                 Lum::Base::GetSize(Lum::Base::Size::stdCharWidth,30,
                                                    OS::display->GetFont(OS::Display::fontScaleCaption1)));
        vPanel->Add(captionText);
        vPanel->AddSpace();

        actionText=new Text();
        actionText->SetFlex(true,false);
        actionText->SetMinWidth(Lum::Base::Size::stdCharWidth,40);
        actionText->SetText(action);
        vPanel->Add(actionText);
        vPanel->AddSpace();

        fuelGauge=new FuelGauge();
        fuelGauge->SetFlex(true,false);
        fuelGauge->SetModel(percent);
        vPanel->Add(fuelGauge);

        return vPanel;
      }

      void GetActions(std::vector<ActionInfo>& actions)
      {
        if (abortAction.Valid()) {
          CreateActionInfosCancel(actions,abortAction);
        }
      }

      void Resync(Base::Model* model, const Base::ResyncMsg& msg)
      {
        if (abortAction.Valid() &&
            (model==GetClosedAction() && GetClosedAction()->IsFinished())) {
          abortAction->Trigger();
        }

        ActionDialog::Resync(model,msg);
      }
    };

    ProgressTask::ProgressTask()
    : needsJoin(false),
      aborted(false),
      finished(true),
      progress(NULL),
      timer(new Model::Action),
      openTimer(new Model::Action),
      abortAction(new Model::Action),
      abortedAction(new Model::Action),
      finishedAction(new Model::Action),
      parent(NULL),
      updateCaption(false),
      updateAction(false),
      percent(0),
      percentModel(new Model::Number(0.0,100.0))
    {
      Observe(abortAction);

      Observe(timer);
      Observe(openTimer);
    }

    ProgressTask::~ProgressTask()
    {
      OS::MutexGuard guard(mutex);

      Cleanup();
    }

    void ProgressTask::Cleanup()
    {
      if (needsJoin) {
        Join();
        needsJoin=false;
      }

      if (progress!=NULL) {
        progress->Close();
        delete progress;
        progress=NULL;
      }

      if (parent!=NULL && !parent->IsEnabled()) {
        parent->Enable();
      }
    }

    void ProgressTask::Resync(Base::Model* model, const Base::ResyncMsg& msg)
    {
      if (model==timer && timer->IsFinished()) {
        OS::MutexGuard guard(mutex);

        if (needsJoin) {
          OS::display->RemoveTimer(openTimer);

          Cleanup();

          if (aborted) {
            abortedAction->Trigger();
          }
          else {
            finishedAction->Trigger();
          }
        }
        else {
          percentModel->Set(percent);
          if (updateCaption) {
            if (progress!=NULL) {
              progress->SetCaption(caption);
            }
            updateCaption=false;
          }
          if (updateAction) {
            if (progress!=NULL) {
              progress->SetAction(action);
            }
            updateAction=false;
          }
          OS::display->AddTimer(0,100000,timer);
        }
      }
      else if (model==openTimer && openTimer->IsFinished()) {
        if (abortedAction->IsObserved()) {
          progress=new Progress(caption,action,percentModel,abortAction);
        }
        else {
          progress=new Progress(caption,action,percentModel,NULL);
        }

        progress->SetParent(parent);
        progress->Open();
      }
      else if (model==abortAction && abortAction->IsFinished()) {
        OS::MutexGuard guard(mutex);

        aborted=true;
      }
    }

    void ProgressTask::Start() throw (OS::ThreadException)
    {
      OS::MutexGuard guard(mutex);

      assert(finished==true);

      aborted=false;
      finished=false;
      needsJoin=false;

      percent=0;

      OS::display->AddTimer(0,100000,timer);
      OS::display->AddTimer(0,500000,openTimer);

      if (parent!=NULL) {
        parent->Disable();
      }

      Thread::Start();
    }

    void ProgressTask::Finished()
    {
      OS::MutexGuard guard(mutex);

      finished=true;
      needsJoin=true;
    }

    bool ProgressTask::IsAborted() const
    {
      OS::MutexGuard guard(mutex);

      return aborted;
    }

    bool ProgressTask::IsFinished() const
    {
      OS::MutexGuard guard(mutex);

      return finished;
    }

    void ProgressTask::SetParent(OS::Window *window)
    {
      parent=window;
    }

    void ProgressTask::SetParent(Dialog *dialog)
    {
      parent=dialog->GetWindow();
    }

    void ProgressTask::SetCaption(const std::wstring& caption)
    {
      OS::MutexGuard guard(mutex);

      this->caption=caption;
      updateCaption=true;
    }

    void ProgressTask::SetAction(const std::wstring& action)
    {
      OS::MutexGuard guard(mutex);

      this->action=action;
      updateAction=true;
    }

    void ProgressTask::SetProgress(double progress)
    {
      OS::MutexGuard guard(mutex);

      percent=progress;
    }

    Model::Action* ProgressTask::GetAbortedAction() const
    {
      return abortedAction.Get();
    }

    Model::Action* ProgressTask::GetFinishedAction() const
    {
      return finishedAction.Get();
    }
  }
}
