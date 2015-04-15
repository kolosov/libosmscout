/*
  This source is part of the Illumination library
  Copyright (C) 2007  Tim Teulings

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

#include <Lum/Dlg/Password.h>

#include <Lum/Base/L10N.h>

#include <Lum/Boolean.h>
#include <Lum/ButtonRow.h>
#include <Lum/Label.h>
#include <Lum/Panel.h>
#include <Lum/String.h>
#include <Lum/Text.h>

namespace Lum {
  namespace Dlg {
    Password::Password(Model::String* username, Model::String* password)
    : result(false),
      username(username),
      password(password),
      okAction(new Lum::Model::Action)
    {
      Observe(okAction);
      Observe(GetClosedAction());
    }

    Password::~Password()
    {
      // no code
    }

    void Password::SetCaption(const std::wstring& caption)
    {
      this->caption=caption;
    }

    void Password::SetText(const std::wstring& text)
    {
      this->text=text;
    }

    void Password::SetComment(const std::wstring& comment)
    {
      this->comment=comment;
    }

    void Password::SetSavePasswordModel(Model::Boolean* savePassword)
    {
      assert(savePassword!=NULL);

      this->savePassword=savePassword;
    }

    Lum::Object* Password::GetContent()
    {
      Label       *label;
      Panel       *vPanel;
      String      *string;
      Text        *text;

      vPanel=VPanel::Create(true,false);

      if (!caption.empty()) {
        text=new Lum::Text();
        text->SetFlex(true,false);
        text->SetAlignment(Text::centered);
        text->SetStyle(OS::Font::bold);
        text->SetFont(OS::display->GetFont(OS::Display::fontScaleCaption1));
        text->SetText(caption);

        vPanel->Add(text);
        vPanel->AddSpace();
      }

      text=new Text();
      text->SetFlex(true,false);
      text->SetText(this->text);

      vPanel->Add(text);
      vPanel->AddSpace();

      label=Label::Create(true,false);

      label->AddLabel(_l(L"DLG_PASSWORD_LABEL_USERNAME",L"Username:"),
                      String::Create(username,20,true,false));

      string=new String();
      string->SetFlex(true,false);
      string->SetMinWidth(Base::Size::stdCharWidth,20);
      string->SetModel(password);
      string->SetMode(Lum::String::password);
      label->AddLabel(_l(L"DLG_PASSWORD_LABEL_PASSWORD",L"Password:"),string);

      if (savePassword.Valid()) {
        label->AddLabel(_l(L"DLG_PASSWORD_LABEL_SAVE_PASSWORD",L"Save password:"),
                        Boolean::Create(savePassword));
      }

      vPanel->Add(label);

      if (!comment.empty()) {
        vPanel->AddSpace();

        text=new Text();
        text->SetFlex(true,false);
        text->SetStyle(OS::Font::italic);
        text->SetText(comment);

        vPanel->Add(text);
      }

      return vPanel;
    }

    void Password::GetActions(std::vector<ActionInfo>& actions)
    {
      CreateActionInfosOkCancel(actions,okAction,GetClosedAction());
    }

    void Password::Resync(Lum::Base::Model* model, const Lum::Base::ResyncMsg& msg)
    {
      if (model==okAction && okAction->IsFinished()) {
        username->Save();
        password->Save();
        if (savePassword.Valid()) {
          savePassword->Save();
        }

        result=true;
        Exit();
      }
      else if (model==GetClosedAction() && GetClosedAction()->IsFinished()) {
        Exit();
      }
      else {
        ActionDialog::Resync(model,msg);
      }
    }

    bool Password::Success() const
    {
      return result;
    }

    bool Password::GetPassword(OS::Window* parent,
                               const std::wstring& caption,
                               const std::wstring& text,
                               const std::wstring& comment,
                               Model::String* username,
                               Model::String* password,
                               Model::Boolean* savePassword)
    {
      assert(username!=NULL && password!=NULL);

      Password *dlg;
      bool      res;

      username->Push();
      password->Push();
      if (savePassword!=NULL) {
        savePassword->Push();
      }

      dlg=new Password(username,password);
      dlg->SetParent(parent);
      dlg->SetTitle(L""/*caption*/);
      dlg->SetCaption(caption);
      dlg->SetText(text);
      dlg->SetComment(comment);

      if (savePassword!=NULL) {
        dlg->SetSavePasswordModel(savePassword);
      }

      if (dlg->Open()) {
        dlg->EventLoop();
        dlg->Close();
      }

      res=dlg->Success();

      delete dlg;

      if (savePassword!=NULL) {
        savePassword->Pop();
      }
      username->Pop();
      password->Pop();

      return res;
    }

    bool Password::GetPassword(OS::Window* parent,
                               const std::wstring& caption,
                               const std::wstring& text,
                               std::wstring& username,
                               std::wstring& password)
    {
      Lum::Model::StringRef user=new Lum::Model::String(username);
      Lum::Model::StringRef pass=new Lum::Model::String(password);
      bool                  result=false;

      result=GetPassword(parent,caption,text,L"",user,pass,NULL);

      if (result) {
        username=user->Get();
        password=pass->Get();
      }

      return result;
    }

    bool Password::GetPassword(OS::Window* parent,
                               const std::wstring& caption,
                               const std::wstring& text,
                               const std::wstring& comment,
                               std::wstring& username,
                               std::wstring& password,
                               bool& savePassword)
    {
      Lum::Model::StringRef  user=new Lum::Model::String(username);
      Lum::Model::StringRef  pass=new Lum::Model::String(password);
      Lum::Model::BooleanRef save=new Lum::Model::Boolean(savePassword);
      bool                   result=false;

      result=GetPassword(parent,caption,text,comment,user,pass,save);

      if (result) {
        username=user->Get();
        password=pass->Get();
        savePassword=save->Get();
      }

      return result;
    }

  }
}

