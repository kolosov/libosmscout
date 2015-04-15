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

#include <Lum/Dlg/Properties.h>

#include <list>
#include <stack>

#include <Lum/Base/L10N.h>
#include <Lum/Base/String.h>

#include <Lum/Panel.h>
#include <Lum/ScrolledObject.h>
#include <Lum/Slider.h>
#include <Lum/String.h>
#include <Lum/Tab.h>
#include <Lum/Text.h>

#include <iostream>

namespace Lum {
  namespace Dlg {

    Properties::Visitor::~Visitor()
    {
      // no code
    }

    Properties::Properties(Def::PropGroup *props)
    : props(props),
      okAction(new Model::Action())
    {
      Observe(okAction);
      Observe(GetClosedAction());

      PushModels();

      ObserveModels();
    }

    Properties::~Properties()
    {
      PopModels();
    }

    void Properties::VisitModels(Def::PropGroup *props, Visitor& visitor)
    {
      for (std::list<Def::Property*>::const_iterator prop=props->GetProps().begin();
          prop!=props->GetProps().end();
          ++prop) {
        if (dynamic_cast<Def::Boolean*>(*prop)!=NULL) {
          Def::Boolean* boolean=dynamic_cast<Def::Boolean*>(*prop);

          visitor.Visit(boolean->GetBoolean());
        }
        else if (dynamic_cast<Def::Number*>(*prop)!=NULL) {
          Def::Number* number=dynamic_cast<Def::Number*>(*prop);

          visitor.Visit(number->GetValue());
        }
        else if (dynamic_cast<Def::Interval*>(*prop)!=NULL) {
          Def::Interval* interval=dynamic_cast<Def::Interval*>(*prop);

          visitor.Visit(interval->GetStart());
          visitor.Visit(interval->GetEnd());
        }
        else if (dynamic_cast<Def::OneOfN*>(*prop)!=NULL) {
          Def::OneOfN* oneOfN=dynamic_cast<Def::OneOfN*>(*prop);

          visitor.Visit(oneOfN->GetValue());
        }
        else if (dynamic_cast<Def::OneOfMany*>(*prop)!=NULL) {
          Def::OneOfMany* oneOfMany=dynamic_cast<Def::OneOfMany*>(*prop);

          visitor.Visit(oneOfMany->GetValue());
        }
      }

      for (std::list<Def::PropGroup*>::const_iterator group=props->GetGroups().begin();
          group!=props->GetGroups().end();
          ++group) {
          VisitModels(*group,visitor);
      }
    }

    void Properties::VisitModels(Visitor& visitor)
    {
      VisitModels(props,visitor);
    }

    void Properties::ObserveModels()
    {
      struct ObserveVisitor : Visitor
      {
        Properties* dialog;

        void Visit(Lum::Base::Model* model)
        {
          dialog->Observe(model);
        }
      };

      ObserveVisitor visitor;

      visitor.dialog=this;

      VisitModels(visitor);
    }

    void Properties::PushModels()
    {
      struct PushVisitor : Visitor
      {
        void Visit(Lum::Base::Model* model)
        {
          model->Push();
        }
      };

      PushVisitor visitor;

      VisitModels(visitor);
    }

    void Properties::ValidateModels()
    {
      struct SaveVisitor : Visitor
      {
        bool valid;

        SaveVisitor()
        {
          valid=true;
        }

        void Visit(Lum::Base::Model* model)
        {
          if (model->IsNull()) {
            valid=false;
          }
        }
      };

      SaveVisitor visitor;

      VisitModels(visitor);

      if (visitor.valid) {
        okAction->Enable();
      }
      else {
        okAction->Disable();
      }
    }

    void Properties::SaveModels()
    {
      struct SaveVisitor : Visitor
      {
        void Visit(Lum::Base::Model* model)
        {
          model->Save();
        }
      };

      SaveVisitor visitor;

      VisitModels(visitor);
    }

    void Properties::PopModels()
    {
      struct PopVisitor : Visitor
      {
        void Visit(Lum::Base::Model* model)
        {
          model->Pop();
        }
      };

      PopVisitor visitor;

      VisitModels(visitor);
    }

    void Properties::AddControlForProperty(Label* label,
                                           Def::Property* prop)
    {
      if (dynamic_cast<Def::Boolean*>(prop)!=NULL) {
        Def::Boolean* boolean=dynamic_cast<Def::Boolean*>(prop);

        label->AddLabel(boolean->GetDesc().GetLabel(),
                        Manager::Behaviour::Instance()->GetBooleanControl(*boolean,true,false));
      }
      else if (dynamic_cast<Def::Number*>(prop)!=NULL) {
        Def::Number* number=dynamic_cast<Def::Number*>(prop);

        if (number->GetValue()->GetMaxAsDouble()-
            number->GetValue()->GetMinAsDouble()<20.0) {
          label->AddLabel(number->GetDesc().GetLabel()+L":",
                          HSlider::Create(number->GetValue(),true,true,false));
        }
        else {
          label->AddLabel(number->GetDesc().GetLabel()+L":",
                          String::Create(number->GetValue(),true,false));
        }
      }
      else if (dynamic_cast<Def::Interval*>(prop)!=NULL) {
        Def::Interval *interval=dynamic_cast<Def::Interval*>(prop);
        HPanel        *hPanel;

        hPanel=new HPanel();
        hPanel->SetFlex(true,false);
        hPanel->Add(String::Create(interval->GetStart(),true,false));
        hPanel->Add(Text::Create(L" - "));
        hPanel->Add(String::Create(interval->GetEnd(),true,false));

        label->AddLabel(interval->GetDesc().GetLabel()+L":",hPanel);
      }
      else if (dynamic_cast<Def::OneOfN*>(prop)!=NULL) {
        Def::OneOfN* oneOfN=dynamic_cast<Def::OneOfN*>(prop);

        label->AddLabel(oneOfN->GetDesc().GetLabel()+L":",
                        Manager::Behaviour::Instance()->GetOneOfNControl(*oneOfN,true,false));
      }
      else if (dynamic_cast<Def::OneOfMany*>(prop)!=NULL) {
        Def::OneOfMany* oneOfMany=dynamic_cast<Def::OneOfMany*>(prop);

        label->AddLabel(oneOfMany->GetDesc().GetLabel()+L":",
                        Manager::Behaviour::Instance()->GetOneOfManyControl(*oneOfMany,true,false));
      }
      else {
        std::cerr << "Unsupported property" << std::endl;
      }
    }

    Lum::Object* Properties::GetContent()
    {
      std::list<Def::PropGroup*> groupStack;
      VPanel                     *vPanel;
      Label                      *label=NULL;
      Tab                        *tab=NULL;
      Lum::Object                *top;

      vPanel=VPanel::Create(true,true);

      if (OS::display->GetSize()<OS::Display::sizeNormal) {
        top=ScrolledObject::Create(vPanel,true,true);
      }
      else {
        top=vPanel;
      }

      groupStack.push_front(props);

      while (!groupStack.empty()) {
        Def::PropGroup* props=groupStack.front();
        bool            isGroup=true;

        groupStack.erase(groupStack.begin());

        isGroup=props->GetGroups().size()>0;

        if (isGroup) {
          if (OS::display->GetSize()<OS::Display::sizeNormal) {
            if (label==NULL) {
              label=Label::Create(true,true);
            }

            label->AddLabel(props->GetDesc().GetLabel(),NULL);
          }
          else {
            Tab *newTab=Tab::Create(true,true);

            if (tab!=NULL) {
              tab->Add(props->GetDesc().GetLabel(),newTab);
            }
            else {
              vPanel->Add(newTab);
            }

            tab=newTab;
            label=NULL;
          }

          for (std::list<Def::PropGroup*>::const_iterator group=props->GetGroups().begin();
              group!=props->GetGroups().end();
              ++group) {
            groupStack.push_back(*group);
          }
        }
        else {
          if (label==NULL) {
            label=Label::Create(true,true);
          }

          for (std::list<Def::Property*>::const_iterator prop=props->GetProps().begin();
              prop!=props->GetProps().end();
              ++prop) {
            AddControlForProperty(label,*prop);
          }

          if (tab!=NULL) {
            tab->Add(props->GetDesc().GetLabel(),label);
            label=NULL;
          }
          else {
            vPanel->Add(label);
            label=NULL;
          }
        }
      }

      return top;
    }

    void Properties::GetActions(std::vector<Lum::Dlg::ActionInfo>& actions)
    {
      Lum::Dlg::ActionDialog::CreateActionInfosOkCancel(actions,okAction,GetClosedAction());
    }

    void Properties::Resync(Lum::Base::Model* model, const Lum::Base::ResyncMsg& msg)
    {
      if (model==okAction &&
          okAction->IsFinished()) {
        SaveModels();
        Exit();
      }
      else if (model==GetClosedAction() &&
               GetClosedAction()->IsFinished()) {
        Exit();
      }
      else {
        ValidateModels();
      }

      Dialog::Resync(model,msg);
    }

    void Properties::Show(Dialog* parent, Def::PropGroup* props)
    {
      Show(parent->GetWindow(),props);
    }

    void Properties::Show(OS::Window* parent, Def::PropGroup* props)
    {
      Properties properties(props);

      properties.SetParent(parent);
      properties.SetTitle(L"Config...");

      if (properties.Open()) {
        properties.EventLoop();
        properties.Close();
      }
    }
  }
}
