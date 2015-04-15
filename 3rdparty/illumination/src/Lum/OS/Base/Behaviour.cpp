/*
  This source is part of the Illumination library
  Copyright (C) 2009  Tim Teulings

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

#include <Lum/OS/Base/Behaviour.h>

#include <Lum/Images/StatefulImage.h>
#include <Lum/Images/Loader.h>

#include <Lum/Boolean.h>
#include <Lum/Button.h>
#include <Lum/Combo.h>
#include <Lum/Dialog.h>
#include <Lum/Tab.h>

namespace Lum {
  namespace OS {
    namespace Base {

      DefaultBehaviour::DefaultBehaviour()
      {
      }

      DefaultBehaviour::~DefaultBehaviour()
      {
      }

      bool DefaultBehaviour::FocusObjectOnWindowFocusIn() const
      {
        return true;
      }

      bool DefaultBehaviour::ScrollKnobActive() const
      {
        return true;
      }

      DefaultBehaviour::TableActionMode DefaultBehaviour::GetTableDefaultActionMode() const
      {
        return TableActionOnDoubleclick;
      }

      Manager::Behaviour::CustomMenuHandler* DefaultBehaviour::GetCustomMenuHandler(OS::Window* window) const
      {
        return NULL;
      }

      void DefaultBehaviour::ApplyMultiViewDlg(Dialog* dialog,
                                               const Def::MultiView& multiView) const
      {
        Tab                   *tab=Tab::Create(true,true);
        Lum::Model::NumberRef value=multiView.GetValue();
        Def::Menu             *menu=NULL;

        if (multiView.GetPreMenu()!=NULL) {
          menu=new Def::Menu(*multiView.GetPreMenu());

          // TODO: Build View menu

          if (multiView.GetPostMenu()!=NULL) {
            menu->Append(multiView.GetPostMenu());
          }
        }

        if (value.Valid()) {
          tab->SetModel(value);
        }

        for (std::list<Def::MultiView::View>::const_iterator view=multiView.GetViews().begin();
             view!=multiView.GetViews().end();
             ++view) {
          tab->Add(view->GetDesc().GetLabel(),view->GetObject());
        }

        dialog->SetMain(tab);
        dialog->SetMenu(menu);
      }

      Lum::Object* DefaultBehaviour::GetActionControl(const Def::Action& def) const
      {
        bool   hasImageLabel=false;
        Button *control=new Button();

        control->SetModel(def.GetAction());

        if (!def.GetImageFilename().empty()) {
          Images::StatefulImageRef image=new Images::StatefulImage();

          Images::loader->LoadApplication(def.GetImageFilename(),image);

          if (image->IsValid()) {
            control->SetLabel(Lum::Image::Create(image));
            control->SetType(Button::typeIcon);
            hasImageLabel=true;
          }
        }

        if (!hasImageLabel) {
          control->SetText(def.GetDesc().GetLabel());
        }

        switch (def.GetType()) {
        case Def::Action::positive:
          control->SetType(Button::typeCommit);
          break;
        case Def::Action::negative:
          control->SetType(Button::typeCancel);
          break;
        default:
          // do nothing
          break;
        }

        return control;
      }

      Lum::Object* DefaultBehaviour::GetActionControl(const Def::Action& def,
                                            bool horizFlex,
                                            bool vertFlex) const
      {
        Lum::Object* control;

        control=GetActionControl(def);
        control->SetFlex(horizFlex,vertFlex);

        return control;
      }

      Lum::Object* DefaultBehaviour::GetOneOfNControl(const Def::OneOfN& def) const
      {
        IndexCombo            *control;
        Model::StringTableRef choices(new Model::StringTable());

        for (std::list<Def::OneOfN::Choice>::const_iterator iter=def.GetChoices().begin();
            iter!=def.GetChoices().end();
            ++iter) {
          choices->Append(iter->GetDesc().GetLabel());
        }

        control=new IndexCombo();
        control->SetFlex(true,false);
        control->SetModel(def.GetValue());
        control->SetTableModel(choices);
        control->SetTableCellPainter(new StringTablePainter());

        return control;
      }

      Lum::Object* DefaultBehaviour::GetOneOfNControl(const Def::OneOfN& def,
                                                      bool horizFlex,
                                                      bool vertFlex) const
      {
        Lum::Object* control;

        control=GetOneOfNControl(def);
        control->SetFlex(horizFlex,vertFlex);

        return control;
      }

      Lum::Object* DefaultBehaviour::GetOneOfManyControl(const Def::OneOfMany& def) const
      {
        IndexCombo *control;

        control=new IndexCombo();
        control->SetFlex(true,false);
        control->SetModel(def.GetValue());
        control->SetTableModel(def.GetChoices());
        control->SetHeaderModel(def.GetChoicesHeader());
        if (def.GetTableCellPainter()!=NULL) {
          control->SetTableCellPainter(def.GetTableCellPainter());
        }

        return control;
      }

      Lum::Object* DefaultBehaviour::GetOneOfManyControl(const Def::OneOfMany& def,
                                                         bool horizFlex,
                                                         bool vertFlex) const
      {
        Lum::Object* control;

        control=GetOneOfManyControl(def);
        control->SetFlex(horizFlex,vertFlex);

        return control;
      }

      Lum::Object* DefaultBehaviour::GetBooleanControl(const Def::Boolean& def) const
      {
        Boolean *control;

        control=new Boolean();
        control->SetFlex(true,false);
        control->SetModel(def.GetBoolean());

        return control;
      }

      Lum::Object* DefaultBehaviour::GetBooleanControl(const Def::Boolean& def,
                                                       bool horizFlex,
                                                       bool vertFlex) const
      {
        Lum::Object* control;

        control=GetBooleanControl(def);
        control->SetFlex(horizFlex,vertFlex);

        return control;
      }
    }
  }
}
