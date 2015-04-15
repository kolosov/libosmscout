/*
  This source is part of the Illumination library
  Copyright (C) 2004  Tim Teulings

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

#include <Lum/Dlg/ActionDialog.h>

#include <Lum/Base/L10N.h>

#include <Lum/OS/Theme.h>

#include <Lum/Button.h>
#include <Lum/ButtonRow.h>
#include <Lum/Panel.h>
#include <Lum/Space.h>
#include <Lum/Text.h>

namespace Lum {
  namespace Dlg {

    ActionInfo::ActionInfo(ActionType type,
                           const std::wstring& label,
                           Model::Action *action,
                           bool isDefault)
     : type(type),
       label(label),
       action(action),
       isDefault(isDefault)
    {
      // no code
    }

    ActionInfo::ActionInfo(ActionType type,
                           const std::wstring& label,
                           OS::Image* image,
                           Model::Action *action,
                           bool isDefault)
     : type(type),
       label(label),
       image(image),
       action(action),
       isDefault(isDefault)
    {
      // no code
    }

    ActionInfo::ActionType ActionInfo::GetType() const
    {
      return type;
    }

    std::wstring ActionInfo::GetLabel() const
    {
      return label;
    }

    OS::Image* ActionInfo::GetImage() const
    {
      return image;
    }

    Model::Action* ActionInfo::GetAction() const
    {
      return action;
    }

    bool ActionInfo::IsDefault() const
    {
      return isDefault;
    }

    ActionDialog::ActionDialog()
    {
      // no code
    }

    ActionDialog::~ActionDialog()
    {
      // no code
    }

    void ActionDialog::PreInit()
    {
      Lum::Object             *content;
      std::vector<ActionInfo> actions;

      content=GetContent();

      assert(content!=NULL);

      GetActions(actions);

      // TODO: Add assertion: We should not have default mixed with commit/cancel

      if (OS::display->GetTheme()->GetDialogActionPosition()==OS::Theme::actionPosHorizButtonRowBelow) {
        Button    *button;
        ButtonRow *row;
        Panel     *vPanel;

        vPanel=VPanel::Create();

        vPanel->Add(content);
        vPanel->AddSpace();

        row=new ButtonRow();
        row->SetFlex(true,false);

        // Optionals
        for (size_t i=0; i<actions.size(); i++) {
          if (actions[i].GetType()==ActionInfo::typeOptional) {
            button=Button::Create(actions[i].GetLabel(),
                                  actions[i].GetImage(),
                                  actions[i].GetAction(),
                                  Button::typeNormal,
                                  true,true);
            row->AddOptional(button);
          }
        }

        // Commit
        for (size_t i=0; i<actions.size(); i++) {
          if (actions[i].GetType()==ActionInfo::typeCommit) {
            button=Button::Create(actions[i].GetLabel(),
                                  actions[i].GetImage(),
                                  actions[i].GetAction(),
                                  Button::typeCommit,
                                  true,true);
            row->Add(button);
          }
        }

        // Defaults
        for (size_t i=0; i<actions.size(); i++) {
          if (actions[i].GetType()==ActionInfo::typeDefault &&
              !Lum::OS::display->GetTheme()->ImplicitDefaultAction()) {
            button=Button::Create(actions[i].GetLabel(),
                                  actions[i].GetImage(),
                                  actions[i].GetAction(),
                                  Button::typeDefault,
                                  true,true);
            row->Add(button);
          }
        }

        // Cancel
        for (size_t i=0; i<actions.size(); i++) {
          if (actions[i].GetType()==ActionInfo::typeCancel &&
              !Lum::OS::display->GetTheme()->ImplicitCancelAction()) {
            button=Button::Create(actions[i].GetLabel(),
                                  actions[i].GetImage(),
                                  actions[i].GetAction(),
                                  Button::typeCancel,
                                  true,true);
            row->Add(button);
          }
        }

        // Additional
        for (size_t i=0; i<actions.size(); i++) {
          if (actions[i].GetType()==ActionInfo::typeAdditional) {
            button=Button::Create(actions[i].GetLabel(),
                                  actions[i].GetImage(),
                                  actions[i].GetAction(),
                                  Button::typeNormal,
                                  true,true);
            row->Add(button);
          }
        }

        vPanel->Add(row);

        SetMain(vPanel);
      }
      else if (OS::display->GetTheme()->GetDialogActionPosition()==OS::Theme::actionPosVertButtonRowRight) {
        Button *button;
        Panel  *hPanel;
        Panel  *vPanel;
        bool   addSpace=false;

        hPanel=HPanel::Create(true,true);

        vPanel=VPanel::Create(false,true);

        vPanel->Add(new VSpace(Space::sizeEmpty,true));

        // Optionals
        for (size_t i=0; i<actions.size(); i++) {
          if (actions[i].GetType()==ActionInfo::typeOptional) {
            button=Button::Create(actions[i].GetLabel(),
                                  actions[i].GetImage(),
                                  actions[i].GetAction(),
                                  Button::typeNormal,
                                  true,false);
            button->SetMinWidth(Base::Size::pixel,
                                OS::display->GetTheme()->GetMinimalButtonWidth());
            vPanel->Add(button);
            addSpace=true;
          }
        }

        if (addSpace) {
          vPanel->AddSpace(true);
        }

        // Additional
        for (size_t i=0; i<actions.size(); i++) {
          if (actions[i].GetType()==ActionInfo::typeAdditional) {
            button=Button::Create(actions[i].GetLabel(),
                                  actions[i].GetImage(),
                                  actions[i].GetAction(),
                                  Button::typeNormal,
                                  true,false);
            button->SetMinWidth(Base::Size::pixel,
                                OS::display->GetTheme()->GetMinimalButtonWidth());
            vPanel->Add(button);
          }
        }

        // Commit
        for (size_t i=0; i<actions.size(); i++) {
          if (actions[i].GetType()==ActionInfo::typeCommit) {
            button=Button::Create(actions[i].GetLabel(),
                                  actions[i].GetImage(),
                                  actions[i].GetAction(),
                                  Button::typeCommit,
                                  true,false);
            button->SetMinWidth(Base::Size::pixel,
                                OS::display->GetTheme()->GetMinimalButtonWidth());
            vPanel->Add(button);
          }
        }

        // Defaults
        for (size_t i=0; i<actions.size(); i++) {
          if (actions[i].GetType()==ActionInfo::typeDefault &&
              !Lum::OS::display->GetTheme()->ImplicitDefaultAction()) {
            button=Button::Create(actions[i].GetLabel(),
                                  actions[i].GetImage(),
                                  actions[i].GetAction(),
                                  Button::typeDefault,
                                  true,false);
            button->SetMinWidth(Base::Size::pixel,
                                OS::display->GetTheme()->GetMinimalButtonWidth());
            vPanel->Add(button);
          }
        }

        // Cancel
        for (size_t i=0; i<actions.size(); i++) {
          if (actions[i].GetType()==ActionInfo::typeCancel &&
              !Lum::OS::display->GetTheme()->ImplicitCancelAction()) {
            button=Button::Create(actions[i].GetLabel(),
                                  actions[i].GetImage(),
                                  actions[i].GetAction(),
                                  Button::typeCancel,
                                  true,false);
            button->SetMinWidth(Base::Size::pixel,
                                OS::display->GetTheme()->GetMinimalButtonWidth());
            vPanel->Add(button);
          }
        }

        hPanel->Add(content);
        hPanel->AddSpace();
        hPanel->Add(vPanel);

        SetMain(hPanel);
      }
      else {
        assert(false); // Unsupported enumeration value
      }

      Dialog::PreInit();
    }

    void ActionDialog::CreateActionInfosOkCancel(std::vector<ActionInfo>& actions,
                                                 Model::Action* okAction,
                                                 Model::Action* cancelAction)
    {
      actions.push_back(Dlg::ActionInfo(ActionInfo::typeCommit,
                                        _ld(dlgButtonOk),
                                        OS::display->GetTheme()->GetStockImage(OS::Theme::imagePositive),
                                        okAction,
                                        true));
      actions.push_back(Dlg::ActionInfo(ActionInfo::typeCancel,
                                        _ld(dlgButtonCancel),
                                        OS::display->GetTheme()->GetStockImage(OS::Theme::imageNegative),
                                        cancelAction));
    }

    void ActionDialog::CreateActionInfosOk(std::vector<ActionInfo>& actions,
                                           Model::Action* okAction)
    {
      actions.push_back(Dlg::ActionInfo(ActionInfo::typeCommit,
                                        _ld(dlgButtonOk),
                                        OS::display->GetTheme()->GetStockImage(OS::Theme::imagePositive),
                                        okAction,
                                        true));
    }

    void ActionDialog::CreateActionInfosCancel(std::vector<ActionInfo>& actions,
                                               Model::Action* cancelAction)
    {
      actions.push_back(Dlg::ActionInfo(ActionInfo::typeCancel,
                                        _ld(dlgButtonCancel),
                                        OS::display->GetTheme()->GetStockImage(OS::Theme::imageNegative),
                                        cancelAction));
    }

    void ActionDialog::CreateActionInfosClose(std::vector<ActionInfo>& actions,
                                              Model::Action* closeAction)
    {
      actions.push_back(Dlg::ActionInfo(ActionInfo::typeDefault,
                                        _ld(dlgButtonClose),
                                        OS::display->GetTheme()->GetStockImage(OS::Theme::imageDefault),
                                        closeAction));
    }

    void ActionDialog::CreateActionInfosQuit(std::vector<ActionInfo>& actions,
                                             Model::Action* quitAction)
    {
      actions.push_back(Dlg::ActionInfo(ActionInfo::typeDefault,
                                        _ld(dlgButtonQuit),
                                        OS::display->GetTheme()->GetStockImage(OS::Theme::imageDefault),
                                        quitAction));
    }
  }
}
