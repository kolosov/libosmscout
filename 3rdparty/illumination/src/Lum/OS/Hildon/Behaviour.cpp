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

#include <Lum/OS/Hildon/Behaviour.h>

#include <hildon/hildon-version.h>

#include <Lum/Array.h>
#include <Lum/BooleanButton.h>
#include <Lum/Button.h>
#include <Lum/Dialog.h>
#include <Lum/Float.h>
#include <Lum/Menu.h>
#include <Lum/Popup.h>
#include <Lum/Tab.h>
#include <Lum/ValueButton.h>
#include <Lum/View.h>

#include <Lum/OS/X11/Display.h>
#include <Lum/OS/X11/Window.h>
#include <Lum/OS/Gtk/GtkTheme.h>

namespace Lum {
  namespace OS {
    namespace Hildon {

#if HILDON_CHECK_VERSION(2,1,0)
      class CustomMenuHandlerPopup : public Manager::Behaviour::CustomMenuHandler
      {
      private:
        static const size_t columns = 2;
        static const size_t rows    = 5;

        class MenuPopup : public Popup
        {
        private:
          Def::Menu                   *def;
          std::set<Lum::Base::Model*> excludes;
          std::set<Model::Action*>    actions;
          std::set<Model::Boolean*>   booleans;

        private:
          void AddToPopup(Float* floatMenu,
                          Def::Menu* def,
                          const std::set<Lum::Base::Model*>& excludes)
          {
            for (std::list<Def::MenuItem*>::const_iterator iter=def->GetItems().begin();
                 iter!=def->GetItems().end();
                 ++iter) {
              assert((*iter)!=NULL);

              if (dynamic_cast<Def::Menu*>(*iter)!=NULL) {
                Def::Menu *sub=dynamic_cast<Def::Menu*>(*iter);

                AddToPopup(floatMenu,sub,excludes);
              }
              else if (dynamic_cast<Def::MenuAction*>(*iter)!=NULL) {
                Def::MenuAction *action=dynamic_cast<Def::MenuAction*>(*iter);

                if (excludes.find(action->GetAction().GetAction())==excludes.end() &&
                  action->GetAction().GetAction()->IsEnabled()) {
                  actions.insert(action->GetAction().GetAction());
                  ObservePrior(action->GetAction().GetAction());

                  Button *button=Button::Create(action->GetAction().GetDesc().GetLabel(),
                                                action->GetAction().GetAction(),
                                                true,false);

                  floatMenu->Add(button);
                }
              }
              else if (dynamic_cast<Def::MenuBoolean*>(*iter)!=NULL) {
                Def::MenuBoolean *boolean=dynamic_cast<Def::MenuBoolean*>(*iter);

                if (excludes.find(boolean->GetBoolean().GetBoolean())==excludes.end() &&
                    boolean->GetBoolean().GetBoolean()->IsEnabled()) {
                  Def::Boolean b=boolean->GetBoolean();

                  booleans.insert(b.GetBoolean());
                  ObservePrior(b.GetBoolean());

                  Lum::Object *object=Manager::Behaviour::Instance()->GetBooleanControl(b,true,false);

                  floatMenu->Add(object);
                }
              }
              else if (dynamic_cast<Def::MenuOneOfN*>(*iter)!=NULL) {
                Def::MenuOneOfN *oneOfN=dynamic_cast<Def::MenuOneOfN*>(*iter);

                if (excludes.find(oneOfN->GetOneOfN().GetValue())==excludes.end()) {
                  // TODO
                }
              }
            }
          }

        public:
          MenuPopup()
          : def(NULL)
          {
            X11::Display *display=dynamic_cast<X11::Display*>(OS::display);

            SetBackground(dynamic_cast<Gtk::GtkTheme*>(OS::display->GetTheme())->appMenuBackground);
            SetType(OS::Window::typeCustom);
            GetWindow()->SetScreenOrientationHint(OS::Window::screenOrientationBothSupported);
            dynamic_cast<X11::Window*>(GetWindow())->SetCustomWindowType(XInternAtom(display->display,
                                                                         "_HILDON_WM_WINDOW_TYPE_APP_MENU",
                                                                         false));

            Observe(GetClosedAction());
          }

          ~MenuPopup()
          {
            // no code
          }

          void SetMenu(Def::Menu* def,
                       const std::set<Lum::Base::Model*>& excludes)
          {
            this->def=def;

            this->excludes=excludes;
          }

          void PreInit()
          {
            Float *floatMenu=new Float();
            View  *view=new View();

            floatMenu->SetFlex(true,true);

            if (display->IsLandscape()) {
              floatMenu->SetPreferredColumns(2);
            }
            else {
              floatMenu->SetPreferredColumns(1);
            }

            AddToPopup(floatMenu,def,excludes);

            view->SetFlex(true,true);
            view->SetObject(floatMenu);

            SetMain(view,false,true);

            Popup::PreInit();
          }

          void Resync(Lum::Base::Model* model, const Lum::Base::ResyncMsg& msg)
          {
            if (model==GetClosedAction() &&
                GetClosedAction()->IsFinished()) {
              Close();
              return;
            }
            else if (dynamic_cast<Model::Action*>(model)!=NULL) {
              Model::ActionRef action=dynamic_cast<Lum::Model::Action*>(model);

              if (actions.find(action)!=actions.end()) {
                if (action->IsEnabled() &&
                    action->IsFinished() &&
                    IsOpen() &&
                    IsShown()) {
                  Close();
                }
                return;
              }
            }
            else if (dynamic_cast<Model::Boolean*>(model)!=NULL) {
              Model::BooleanRef boolean=dynamic_cast<Lum::Model::Boolean*>(model);

              if (booleans.find(boolean)!=booleans.end()) {
                if (boolean->IsEnabled() &&
                    IsOpen() &&
                    IsShown()) {
                  Close();
                }
                return;
              }
            }

            Popup::Resync(model,msg);
          }
        };

      private:
        MenuPopup *popup;

      private:
        void CountModels(Def::Menu* def,
                         size_t& actions,
                         size_t& bools,
                         size_t& oneOfNs,
                         const std::set<Lum::Base::Model*>& excludes)
        {
          for (std::list<Def::MenuItem*>::const_iterator iter=def->GetItems().begin();
               iter!=def->GetItems().end();
               ++iter) {
            assert((*iter)!=NULL);

            if (dynamic_cast<Def::Menu*>(*iter)!=NULL) {
              Def::Menu *sub=dynamic_cast<Def::Menu*>(*iter);

              CountModels(sub,actions,bools,oneOfNs,excludes);
            }
            else if (dynamic_cast<Def::MenuAction*>(*iter)!=NULL) {
              Def::MenuAction *action=dynamic_cast<Def::MenuAction*>(*iter);

              if (excludes.find(action->GetAction().GetAction())==excludes.end()) {
                actions++;
              }
            }
            else if (dynamic_cast<Def::MenuBoolean*>(*iter)!=NULL) {
              Def::MenuBoolean *boolean=dynamic_cast<Def::MenuBoolean*>(*iter);

              if (excludes.find(boolean->GetBoolean().GetBoolean())==excludes.end()) {
                bools++;
              }
            }
            else if (dynamic_cast<Def::MenuOneOfN*>(*iter)!=NULL) {
              Def::MenuOneOfN *oneOfN=dynamic_cast<Def::MenuOneOfN*>(*iter);

              if (excludes.find(oneOfN->GetOneOfN().GetValue())==excludes.end()) {
                oneOfNs++;
              }
            }
          }
        }

      public:
        CustomMenuHandlerPopup(OS::Window* window)
        : Manager::Behaviour::CustomMenuHandler(window),
          popup(NULL)
        {
        }

        ~CustomMenuHandlerPopup()
        {
          delete popup;
        }

        bool SetMenu(Def::Menu* def,
                const std::set<Lum::Base::Model*>& excludes)
        {
          if (popup==NULL) {
            popup=new MenuPopup();
            popup->SetParent(window);
          }

          popup->SetMenu(def,excludes);

          return true;
        }

        bool Open()
        {
          if (popup!=NULL) {
            return popup->Open();
          }
          else {
            return false;
          }
        }
      };
  #else
      class CustomMenuHandlerPopup : public OS::Behaviour::CustomMenuHandler
      {
      private:
        Menu *menu;

      public:
        CustomMenuHandlerPopup(OS::Window* window)
        : OS::Behaviour::CustomMenuHandler(window),
          menu(NULL)
        {
        }

        ~CustomMenuHandlerPopup()
        {
          delete menu;
        }

        bool SetMenu(Def::Menu* def,
                const std::set<Lum::Base::Model*>& excludes)
        {
          if (menu!=NULL) {
            if (menu->IsOpen()) {
              menu->Close();
            }
          }
          delete menu;

          menu=ConvertToMenu(def,excludes);
          menu->SetParent(window);

          return true;
        }

        bool Open()
        {
          if (menu!=NULL && !menu->GetWindow()->IsShown()) {
            menu->SetPos(window->GetX()+12,window->GetY());
            return menu->OpenInStickyMode();
          }
          else {
            return false;
          }
        }
      };

#endif

      Manager::Behaviour::CustomMenuHandler* Behaviour::GetCustomMenuHandler(OS::Window* window) const
      {
        return new CustomMenuHandlerPopup(window);
      }

#if HILDON_CHECK_VERSION(2,1,0)
      class ViewDialog : public Dialog
      {
      private:
        Lum::Object *view;

      public:
        ViewDialog(Lum::Object* view)
         :view(view)
        {
          SetType(OS::Window::typeMain);
          SetExitAction(GetClosedAction());
        }

        void PreInit()
        {
          SetMain(view);

          Dialog::PreInit();
        }
      };

      class MultiViewFloat : public Float
      {
      private:
        Def::MultiView                    multiView;
        Model::NumberRef                  value;
        std::vector<Model::ActionRef>     actions;
        std::vector<Def::MultiView::View> views;
        std::vector<Dialog*>              dialogs;

      public:
        MultiViewFloat(const Def::MultiView& multiView)
         : multiView(multiView),
           value(multiView.GetValue())
        {
          if (!value.Valid()) {
            value=new Model::ULong();
          }

          Observe(value);

          for (std::list<Def::MultiView::View>::const_iterator view=multiView.GetViews().begin();
               view!=multiView.GetViews().end();
               ++view) {
            Button           *button;
            Model::ActionRef action=new Model::Action();

            actions.push_back(action);
            dialogs.push_back(NULL);
            views.push_back(*view);
            Observe(action);

            button=Button::Create(view->GetDesc().GetLabel(),action,true,false);
            button->SetHeight(Lum::Base::Size::pixel,105); //Shouldn't be hardcoded

            Add(button);
          }
        }

        ~MultiViewFloat()
        {
          for (std::vector<Dialog*>::iterator dialog=dialogs.begin();
               dialog!=dialogs.end();
               ++dialog) {
            delete *dialog;
          }
        }

        void Layout()
        {
          assert(GetWindow()!=NULL);
          Observe(GetWindow()->GetOpenedAction());

          Float::Layout();
        }

        void ShowDialog()
        {
          if (dialogs[value->GetUnsignedLong()]==NULL) {
            Def::Menu *menu=NULL;

            if (multiView.GetPreMenu()!=NULL) {
              menu=new Def::Menu(*multiView.GetPreMenu());

              if (views[value->GetUnsignedLong()].GetMenu()!=NULL) {
                menu->Group(views[value->GetUnsignedLong()].GetDesc())
                ->Append(views[value->GetUnsignedLong()].GetMenu())
                ->End();
              }

              if (multiView.GetPostMenu()!=NULL) {
                menu->Append(multiView.GetPostMenu());
              }
            }

            dialogs[value->GetUnsignedLong()]=new ViewDialog(views[value->GetUnsignedLong()].GetObject());
            dialogs[value->GetUnsignedLong()]->SetParent(GetWindow());
            dialogs[value->GetUnsignedLong()]->SetMenu(menu);
          }

          Dialog* dialog=dialogs[value->GetUnsignedLong()];

          if (dialog->Open()) {
            dialog->EventLoop();
            dialog->Close();

            value->SetNull();
          }
        }

        void Resync(Lum::Base::Model* model, const Lum::Base::ResyncMsg& msg)
        {
          if (GetWindow()!=NULL &&
              model==GetWindow()->GetOpenedAction() &&
              GetWindow()->GetOpenedAction()->IsFinished() && !value->IsNull()) {

            Float::Resync(model,msg);

            ShowDialog();

            return;
          }

          for (size_t i=0; i<actions.size(); i++) {
            if (model==actions[i]) {
              if (actions[i]->IsEnabled() && actions[i]->IsFinished()) {
                value->Set(i);
              }
            }
          }

          if (model==value && !value->IsNull() && IsVisible()) {
            ShowDialog();
          }

          Float::Resync(model,msg);
        }
      };
#endif

      bool Behaviour::FocusObjectOnWindowFocusIn() const
      {
#if HILDON_CHECK_VERSION(2,1,0)
        return true;
#else
        return false;
#endif
      }

      bool Behaviour::ScrollKnobActive() const
      {
#if HILDON_CHECK_VERSION(2,1,0)
        return false;
#else
        return true;
#endif
      }

      Behaviour::TableActionMode Behaviour::GetTableDefaultActionMode() const
      {
        return TableActionOnSelection;
      }

      void Behaviour::ApplyMultiViewDlg(Dialog* dialog,
                                        const Def::MultiView& multiView) const
      {
#if HILDON_CHECK_VERSION(2,1,0)
        Float     *floatMenu=new MultiViewFloat(multiView);
        View      *view=new View();
        Def::Menu *menu=NULL;

        //dialog->GetWindow()->SetScreenOrientationHint(Window::screenOrientationBothSupported);

        if (display->IsLandscape()) {
          floatMenu->SetPreferredColumns(3);
        }
        else {
          floatMenu->SetPreferredColumns(2);
        }

        view->SetFlex(true,true);
        view->SetObject(floatMenu);

        if (multiView.GetPreMenu()!=NULL) {
          menu=new Def::Menu(*multiView.GetPreMenu());

          // TODO: Build View menu

          if (multiView.GetPostMenu()!=NULL) {
            menu->Append(multiView.GetPostMenu());
          }
        }

        dialog->SetMain(view);
        dialog->SetMenu(menu);
#else
        Tab                  *tab=Tab::Create(true,true);
        Lum::Model::SizeTRef value=multiView.GetValue();
        Def::Menu            *menu=NULL;

        if (multiView.GetPreMenu()!=NULL) {
          menu=new Def::Menu(*multiView.GetPreMenu());

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
#endif
      }

      Lum::Object* Behaviour::GetOneOfNControl(const Def::OneOfN& def) const
      {
#if HILDON_CHECK_VERSION(2,1,0)
        TableIndexValueButton *control;
        Model::StringTableRef choices(new Model::StringTable());

        for (std::list<Def::OneOfN::Choice>::const_iterator iter=def.GetChoices().begin();
            iter!=def.GetChoices().end();
            ++iter) {
          choices->Append(iter->GetDesc().GetLabel());
        }

        control=TableIndexValueButton::Create(def.GetDesc().GetLabel(),
                                              def.GetValue(),
                                              choices,
                                              new StringTablePainter());

        return control;
#else
        return DefaultBehaviour::GetOneOfNControl(def);
#endif
      }

      Lum::Object* Behaviour::GetOneOfManyControl(const Def::OneOfMany& def) const
      {
#if HILDON_CHECK_VERSION(2,1,0)
        TableIndexValueButton *control;

        control=TableIndexValueButton::Create(def.GetDesc().GetLabel(),
                                              def.GetValue(),
                                              def.GetChoices(),
                                              def.GetTableCellPainter());

        return control;
#else
        return DefaultBehaviour::GetOneOfManyControl(def);
#endif
      }

      Lum::Object* Behaviour::GetBooleanControl(const Def::Boolean& def) const
      {
#if HILDON_CHECK_VERSION(2,1,0)
        BooleanButton *control;

        control=new BooleanButton();
        control->SetModel(def.GetBoolean());
        control->SetLabel(def.GetDesc().GetLabel());

        return control;
#else
        return DefaultBehaviour::GetBooleanControl(def);
#endif
      }
    }
  }
}
