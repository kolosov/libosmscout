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

#include <Lum/Dialog.h>

#include <Lum/WindowGroup.h>

#include <Lum/Manager/Behaviour.h>

namespace Lum {

  Dialog::Dialog()
   : menuDef(NULL),
     customMenuHandler(NULL),
     toolbar(NULL),
     main(NULL),
     statusLine(NULL),
     group(NULL),
     seamless(false)
  {
    Observe(GetWindow()->GetResizeAction());
    Observe(GetWindow()->GetMenuAction());

    if (OS::display->GetTheme()->GetMenuType()==OS::Theme::menuTypeCustom) {
      customMenuHandler=Manager::Behaviour::Instance()->GetCustomMenuHandler(GetWindow());
    }
  }

  Dialog::~Dialog()
  {
    delete customMenuHandler;
    delete menuDef;
  }

  void Dialog::SetMenu(Def::Menu* menuDef)
  {
    this->menuDef=menuDef;

    GetWindow()->SetHasMenuHint(menuDef!=NULL);

    if (group!=NULL) {
      HandleMenuChange();
    }
  }

  void Dialog::SetToolbar(Lum::Object* toolbar)
  {
    assert(group==NULL);

    this->toolbar=toolbar;
  }

  void Dialog::SetMain(Lum::Object* main, bool seamless)
  {
    assert(group==NULL);

    this->main=main;
    this->seamless=seamless;
  }

  void Dialog::SetStatusLine(Lum::Object* statusLine)
  {
    assert(group==NULL);

    this->statusLine=statusLine;
  }

  void Dialog::HandleMenuChange()
  {
    if (menuDef!=NULL) {
      switch (OS::display->GetTheme()->GetMenuType()) {
      case OS::Theme::menuTypeStrip:
        group->SetMenuStrip(ConvertToMenuStrip(menuDef));
        break;
      case OS::Theme::menuTypePopup: {
        Menu * m=ConvertToMenu(menuDef);

        m->SetParent(GetWindow());
        group->SetMenu(m->GetWindow());
        }
        break;
      case OS::Theme::menuTypeCustom:
        {
          std::set<Base::Model*> excludes;

          group->GetModels(excludes);
          excludes.insert(GetClosedAction());

          OS::Window* parent=GetWindow()->GetParent();

          while (parent!=NULL) {
            excludes.insert(parent->GetClosedAction());

            parent=parent->GetParent();
          }

          customMenuHandler->SetMenu(menuDef,excludes);
        }
        break;
      }
    }
    else {
      group->SetMenu(NULL);
      group->SetMenuStrip(NULL);
    }
  }

  void Dialog::CalcSize()
  {
    OS::Window *window=GetWindow();

    assert(main!=NULL);

    if (GetWindow()->GetType()==OS::Window::typeDialog) {
      bool hflex=main->HorizontalFlex();
      bool vflex=main->VerticalFlex();

      if (OS::display->GetTheme()->SubdialogsAlwaysFlexHoriz()) {
        hflex=true;
      }

      if (OS::display->GetTheme()->SubdialogsAlwaysFlexVert()) {
        vflex=true;
      }

      main->SetFlex(hflex,vflex);
    }

    group=new WindowGroup();
    if ((GetWindow()->GetType()==OS::Window::typeMain ||
         GetWindow()->GetType()==OS::Window::typeDialog) &&
        !GetWindow()->IsFullScreen()) {
      group->SetBackground(OS::display->GetFill(OS::Display::dialogWindowBackgroundFillIndex));
    }
    else {
      group->SetBackground(OS::display->GetFill(OS::Display::backgroundFillIndex));
    }

    group->SetFlex(main->HorizontalFlex(),main->VerticalFlex());

    group->SetToolbar(toolbar);
    group->SetMain(main,seamless);
    group->SetStatusLine(statusLine);

    HandleMenuChange();

    group->SetWindow(window);
    group->CalcSize();
    group->SetWindow(window);

    if (OS::display->GetType()==OS::Display::typeTextual &&
        window->GetType()==OS::Window::typeMain) {
      group->Resize(OS::display->GetWorkAreaWidth(),OS::display->GetWorkAreaHeight());
    }

    if (!IsOpen()) {
      window->SetSize(group->GetOWidth(),group->GetOHeight());
      window->SetMinMaxSize(group->HorizontalFlex() ? group->GetOMinWidth() : window->GetWidth(),
                            group->VerticalFlex() ? group->GetOMinHeight() : window->GetHeight(),
                            group->HorizontalFlex() ? group->GetOMaxWidth() : window->GetWidth(),
                            group->VerticalFlex() ? group->GetOMaxHeight() : window->GetHeight());

      if (window->GetX()+window->GetWidth()-1>OS::display->GetScreenWidth() &&
          window->GetWidth()<OS::display->GetScreenWidth()) {
        window->SetPos(OS::display->GetScreenWidth()-window->GetWidth(),
                       window->GetY());
      }
      if (window->GetY()+window->GetHeight()-1>OS::display->GetScreenHeight() &&
          window->GetHeight()<OS::display->GetScreenHeight()) {
        window->SetPos(window->GetX(),
                       OS::display->GetScreenHeight()-window->GetHeight());
      }
    }

    Window::SetMain(group);
  }

  void Dialog::ReinitWindow()
  {
    delete menuDef;

    toolbar=NULL;
    main=NULL;
    statusLine=NULL;
    group=NULL;

    Window::ReinitWindow();
  }

  void Dialog::Resync(Base::Model* model, const Base::ResyncMsg& msg)
  {
    if (model==GetWindow()->GetResizeAction() &&
        GetWindow()->GetResizeAction()->IsFinished()) {
      if (GetWindow()->GetType()==OS::Window::typeMain) {
        if (GetWindow()->IsFullScreen()) {
          GetMain()->SetBackground(OS::display->GetFill(OS::Display::backgroundFillIndex));
        }
        else {
          GetMain()->SetBackground(OS::display->GetFill(OS::Display::dialogWindowBackgroundFillIndex));
        }
      }
    }
    else if (model==GetWindow()->GetMenuAction() &&
             GetWindow()->GetMenuAction()->IsFinished()) {
      if (OS::display->GetTheme()->GetMenuType()==OS::Theme::menuTypeCustom &&
          customMenuHandler!=NULL) {
        /* ignore */ customMenuHandler->Open();
      }
    }

    Window::Resync(model,msg);
  }
}

