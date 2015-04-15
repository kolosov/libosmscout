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

#include <Lum/Dlg/Values.h>

#include <Lum/ButtonRow.h>
#include <Lum/Label.h>
#include <Lum/Panel.h>
#include <Lum/Text.h>

namespace Lum {
  namespace Dlg {

    ValuesInput::Value::Value()
    : control(NULL),
      validator(NULL)
    {
      // no code
    }

    ValuesInput::ValuesInput()
    : result(false),
      okAction(new Lum::Model::Action)
    {
      okAction->Disable();

      Observe(okAction);
      Observe(GetClosedAction());
    }

    ValuesInput::~ValuesInput()
    {
      for (std::list<Value>::iterator iter=values.begin(); iter!=values.end(); ++iter) {
        Forget(iter->model);
        iter->model->Pop();
        delete iter->validator;
      }
    }

    void ValuesInput::AddValue(const Value& value)
    {
      values.push_back(value);

      if (value.model.Valid()) {
        value.model->Push();
        if (value.control!=NULL) {
          value.control->SetModel(value.model.Get());
        }

        Observe(value.model.Get());
      }
    }

    void ValuesInput::Validate()
    {
      for (std::list<Value>::iterator iter=values.begin(); iter!=values.end(); ++iter) {
        if (iter->validator!=NULL && !iter->validator->IsValid(iter->model)) {
          okAction->Disable();
          return;
        }
      }

      okAction->Enable();
    }

    void ValuesInput::SetCaption(const std::wstring& caption)
    {
      this->caption=caption;
    }

    void ValuesInput::SetText(const std::wstring& text)
    {
      this->text=text;
    }

    void ValuesInput::SetNote(const std::wstring& note)
    {
      this->note=note;
    }

    Lum::Object* ValuesInput::GetContent()
    {
      Label       *label;
      Panel       *vPanel;
      Text        *text;

      vPanel=Lum::VPanel::Create(true,false);

      if (!caption.empty()) {
        text=new Lum::Text();
        text->SetFlex(true,false);
        text->SetAlignment(Text::centered);
        text->SetStyle(OS::Font::bold);
        text->SetFont(OS::display->GetFont(OS::Display::fontScaleCaption2));
        text->SetText(caption);

        vPanel->Add(text);
        vPanel->AddSpace();
      }

      text=new Lum::Text(this->text);
      text->SetFlex(true,false);
      vPanel->Add(text);

      if (!note.empty()) {
        vPanel->AddSpace();

        text=new Lum::Text(this->note,
                           Lum::OS::Font::italic,
                           Lum::Text::left,
                           Lum::OS::display->GetFont(Lum::OS::Display::fontScaleFootnote));
        text->SetFlex(true,false);
        vPanel->Add(text);
      }

      vPanel->AddSpace();

      if (values.size()==1 and values.begin()->label.empty()) {
        vPanel->Add(values.begin()->control);
      }
      else {
        label=Lum::Label::Create(true,false);

        for (std::list<Value>::iterator iter=values.begin(); iter!=values.end(); ++iter) {
          label->AddLabel(iter->label,iter->control);
        }
        vPanel->Add(label);
      }

      return vPanel;
    }

    void ValuesInput::GetActions(std::vector<ActionInfo>& actions)
    {
      CreateActionInfosOkCancel(actions,okAction,GetClosedAction());
    }

    void ValuesInput::Resync(Lum::Base::Model* model, const Lum::Base::ResyncMsg& msg)
    {
      if (model==okAction && okAction->IsFinished()) {
        for (std::list<Value>::iterator iter=values.begin(); iter!=values.end(); ++iter) {
          iter->model->Save();
        }
        result=true;
        Exit();
      }
      else if (model==GetClosedAction() && GetClosedAction()->IsFinished()) {
        Exit();
      }
      else {
        for (std::list<Value>::iterator iter=values.begin(); iter!=values.end(); ++iter) {
          if (iter->model.Get()==model) {
            Validate();
            break;
          }
        }
      }

      Dialog::Resync(model,msg);
    }

    bool ValuesInput::HasResult() const
    {
      return result;
    }
  }
}
