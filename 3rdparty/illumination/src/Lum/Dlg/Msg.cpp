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

#include <Lum/Dlg/Msg.h>

#include <Lum/Base/L10N.h>

#include <Lum/ButtonRow.h>
#include <Lum/Panel.h>
#include <Lum/Text.h>

namespace Lum {
  namespace Dlg {
    Msg::Msg()
    {
    }

    Msg::~Msg()
    {
      // no code
    }

    void Msg::SetCaption(const std::wstring& caption)
    {
      this->caption=caption;
    }

    void Msg::SetText(const std::wstring& text)
    {
      this->text=text;
    }

    void Msg::AddButton(const std::wstring& label,Button::Type type,Model::Action* action)
    {
      ButtonInfo info;

      info.label=label;
      info.type=type;
      info.action=action;

      Observe(action);

      list.push_back(info);
    }

    void Msg::AddButtons(const std::wstring& buttons)
    {
      size_t x=0;
      size_t y=0;

      while (y<=buttons.length()) {
        if ((y==buttons.length() && x<y) || buttons[y]==L'|') {
          ButtonInfo info;

          info.label=buttons.substr(x,y-x);
          if (info.label[info.label.length()-1]==L'*') {
            info.action=new Model::Action();
            info.type=Button::typeCommit;
            info.label.erase(info.label.length()-1);
          }
          else if (info.label[info.label.length()-1]==L'^') {
            info.action=GetClosedAction();
            info.type=Button::typeCancel;
            info.label.erase(info.label.length()-1);
          }
          else if (info.label[info.label.length()-1]==L'#') {
            info.action=new Model::Action();
            info.type=Button::typeDefault;
            info.label.erase(info.label.length()-1);
          }
          else {
            info.action=new Model::Action();
            info.type=Button::typeNormal;
          }


          AddButton(info.label,info.type,info.action);

          y++;
          x=y;
        }
        else {
          y++;
        }
      }

    }

    size_t Msg::GetResultIndex() const
    {
      for (size_t x=0; x<list.size(); x++) {
        if (resultAction==list[x].action) {
          return x;
        }
      }

      for (size_t x=0; x<list.size(); x++) {
        if (list[x].type==Button::typeCancel) {
          return x;
        }
      }

      return 0;
    }

    Lum::Object* Msg::GetContent()
    {
      Panel *vPanel;

      vPanel=VPanel::Create(true,false);

      if (!caption.empty()) {
        Text *text;

        text=new Text(caption,OS::Font::italic|OS::Font::bold,Text::left,
                      OS::display->GetFont(OS::Display::fontScaleCaption1));
        text->SetFlex(true,false);

        vPanel->Add(text);
        vPanel->AddSpace();
      }

      vPanel->Add(Text::Create(this->text,true,false));

      return vPanel;
    }

    void Msg::GetActions(std::vector<ActionInfo>& actions)
    {
      for (size_t x=0; x<list.size(); x++) {
        switch (list[x].type) {
        case Button::typeNormal:
          actions.push_back(ActionInfo(ActionInfo::typeAdditional,list[x].label,list[x].action));
          break;
        case Button::typeDefault:
          actions.push_back(ActionInfo(ActionInfo::typeDefault,list[x].label,list[x].action));
          break;
        case Button::typeCommit:
          actions.push_back(ActionInfo(ActionInfo::typeCommit,list[x].label,list[x].action));
          break;
        case Button::typeCancel:
          actions.push_back(ActionInfo(ActionInfo::typeCancel,list[x].label,list[x].action));
          break;
        default:
          assert(false);
        }
      }
    }

    void Msg::Resync(Base::Model* model, const Base::ResyncMsg& msg)
    {
      for (size_t x=0; x<list.size(); x++) {
        if (model==list[x].action && list[x].action->IsFinished()) {
          resultAction=list[x].action;
          Exit();
          return;
        }
      }

      if (model==GetClosedAction() && GetClosedAction()->IsFinished()) {
        resultAction=GetClosedAction();
        Exit();
        return;
      }

      ActionDialog::Resync(model,msg);
    }

    void Msg::ShowOk(Dialog* parent,
                     const std::wstring& caption,
                     const std::wstring& text)
    {
      ShowOk(parent ? parent->GetWindow() : (OS::Window*)NULL ,caption,text);
    }

    void Msg::ShowOk(OS::Window* parent,
                     const std::wstring& caption,
                     const std::wstring& text)
    {
      Msg msg;

      msg.SetParent(parent);
      msg.SetTitle(L""/*caption*/);
      msg.SetCaption(caption);
      msg.SetText(text);
      msg.AddButton(_ld(dlgButtonOk),Button::typeDefault,msg.GetClosedAction());

      if (msg.Open()) {
        msg.EventLoop();
        msg.Close();
      }
    }

    size_t Msg::Ask(Dialog* parent,
                    const std::wstring& caption,
                    const std::wstring& text,
                    const std::wstring& buttons)
    {
      return Ask(parent->GetWindow(),caption,text,buttons);
    }

    size_t Msg::Ask(OS::Window* parent,
                    const std::wstring& caption,
                    const std::wstring& text,
                    const std::wstring& buttons)
    {
      Msg msg;

      msg.SetParent(parent);
      msg.SetTitle(L""/*caption*/);
      msg.SetCaption(caption);
      msg.SetText(text);
      msg.AddButtons(buttons);

      if (msg.Open()) {
        msg.EventLoop();
        msg.Close();
      }

      return msg.GetResultIndex();
    }

    size_t Msg::Ask(OS::Window* parent,
                    const std::wstring& caption,
                    const std::wstring& text,
                    const std::wstring& buttons,
                    int x, int y)
    {
      Msg msg;

      msg.SetParent(parent);
      msg.SetTitle(L""/*caption*/);
      msg.SetPosition(::Lum::OS::Window::positionManual,::Lum::OS::Window::positionManual);
      msg.GetWindow()->SetPos(x,y);
      msg.SetCaption(caption);
      msg.SetText(text);
      msg.AddButtons(buttons);

      if (msg.Open()) {
        msg.EventLoop();
        msg.Close();
      }

      return msg.GetResultIndex();
    }

    size_t Msg::Ask(Dialog* parent,
                    const std::wstring& caption,
                    const std::wstring& text,
                    const std::wstring& buttons,
                    int x, int y)
    {
      return Ask(parent->GetWindow(),caption,text,buttons,x,y);
    }
  }
}
