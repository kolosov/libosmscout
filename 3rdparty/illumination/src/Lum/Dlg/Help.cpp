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

#include <Lum/Dlg/Help.h>

#include <Lum/Edit/Edit.h>
#include <Lum/Panel.h>
#include <Lum/View.h>

namespace Lum {
  namespace Dlg {

    Help::Help(const std::wstring& helpText)
    : editContent(new Edit::Text())
    {
      editContent->SetText(helpText);
      editContent->Disable();

      SetExitAction(GetClosedAction());
    }

    Help::~Help()
    {
      // no code
    }

    Lum::Object* Help::GetContent()
    {
      Edit::Edit  *edit;
      Panel       *panel;
      View        *view;

      panel=VPanel::Create(true,true);
      panel->SetMinHeight(Base::Size::workVRel,20);
      panel->SetHeight(Base::Size::workVRel,80);

      view=new View();
      view->SetFlex(true,true);

      edit=new Edit::Edit();
      edit->SetFlex(true,true);
      edit->SetMinWidth(Base::Size::stdCharWidth,40);
      edit->SetRequiresKnobs(false);
      edit->SetConfig(Edit::Config::GetDefaultConfig());
      edit->SetColorSheme(Edit::ColorSheme::GetDefaultColorSheme());
      edit->SetFont(OS::display->GetFont());
      edit->SetModel(editContent);

      view->SetObject(edit);

      panel->Add(view);

      return panel;
    }

    void Help::GetActions(std::vector<Lum::Dlg::ActionInfo>& actions)
    {
      Lum::Dlg::ActionDialog::CreateActionInfosClose(actions,GetClosedAction());
    }

    void Help::Resync(Base::Model* model, const Base::ResyncMsg& msg)
    {
      Dialog::Resync(model,msg);
    }

    void Help::Show(Dialog* parent, const std::wstring& helpText)
    {
      Help dialog(helpText);

      dialog.SetParent(parent);

      if (dialog.Open()) {
        dialog.EventLoop();
        dialog.Close();
      }
    }
  }
}
