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

#include <Lum/Popup.h>

#include <Lum/Base/Util.h>

#include <Lum/OS/Theme.h>

#include <Lum/Panel.h>

namespace Lum {

  Popup::Popup()
   : container(NULL),
     main(NULL),
     reference(NULL),
     seamless(true)
  {
    if (OS::display->GetTheme()->PopupsAreDialogs()) {
      SetType(OS::Window::typeDialog);
    }
    else {
      SetType(OS::Window::typePopup);
    }

    Observe(GetWindow()->GetClosedAction());
    Observe(GetWindow()->GetFocusOutAction());
    Observe(GetWindow()->GetEventAction());
  }

  Popup::~Popup()
  {
    // no code
  }

  /**
    Set the main, top-level content object of the popup.

    The popup will possibly wrap the main object in another container
    object.
  */
  void Popup::SetMain(Lum::Object* main, bool seamless, bool border)
  {
    delete this->main;
    this->main=main;

    if (OS::display->GetTheme()->PopupsAreDialogs()) {
      this->seamless=false;
    }
    else {
      this->seamless=seamless;
    }
    this->border=border;
  }

  /**
    Set an non-default background for the top-level container object.

    Background must be set before the popup opens.
  */
  void Popup::SetBackground(OS::Fill* background)
  {
    this->background=background;
  }

  /**
    Set an reference object (in another window) the popup should be aligned to
    in position and width.
  */
  void Popup::SetReference(Lum::Object* object)
  {
    reference=object;
  }

  /**
    Return the assigned reference object.
  */
  Lum::Object* Popup::GetReference() const
  {
    return reference;
  }

  void Popup::ReinitWindow()
  {
    container=NULL;
    main=NULL;

    Window::ReinitWindow();
  }

  void Popup::CalcSize()
  {
    assert(main!=NULL);

    OS::Window *window=GetWindow();

    if (container==NULL) {
      container=new Lum::PopupGroup();
      if (background.Valid() && background->HasBorder()) {
        container->SetBackground(background);
      }
      else if (border) {
        container->SetBackground(OS::display->GetFill(OS::Display::popupWindowBackgroundFillIndex));
      }
      else {
        container->SetBackground(OS::display->GetFill(OS::Display::backgroundFillIndex));
      }
      container->SetFlex(main->HorizontalFlex(),main->VerticalFlex()); // To resize to same size of reference
      container->SetMain(main,seamless);
    }

    container->SetWindow(window);
    container->CalcSize();
    container->SetWindow(window);

    if (GetReference()!=NULL &&
        GetType()==OS::Window::typePopup) {
      // Is is nice, when reference and popup have the same width
      if (GetReference()->GetOWidth()>container->GetOWidth()) {
        container->ResizeWidth(GetReference()->GetOWidth());
      }
    }

    if (!IsOpen()) {
      window->SetSize(container->GetOWidth(),container->GetOHeight());
      window->SetMinMaxSize(window->GetWidth(),
                            window->GetHeight(),
                            window->GetWidth(),
                            window->GetHeight());

      if (reference!=NULL) {
        OS::Window* parent;
        int         x,y;

        parent=reference->GetWindow();
        x=parent->GetX()+reference->GetOX();

        if (parent->GetY()+reference->GetOY()+reference->GetOHeight()+window->GetHeight()<OS::display->GetScreenHeight()) {
          y=parent->GetY()+reference->GetOY()+reference->GetOHeight();
        }
        else {
          y=parent->GetY()+reference->GetOY()-window->GetHeight();
        }

        x=Base::RoundRange(x,0,(int)(OS::display->GetScreenWidth()-1-window->GetWidth()));
        y=Base::RoundRange(y,0,(int)(OS::display->GetScreenHeight()-1-window->GetHeight()));

        window->SetPos(x,y);
        SetPosition(OS::Window::positionManual,OS::Window::positionManual);
      }
      else {
        if (window->GetX()+window->GetWidth()-1>OS::display->GetScreenWidth()
            && window->GetWidth()<OS::display->GetScreenWidth()) {
          window->SetPos(OS::display->GetScreenWidth()-window->GetWidth(),
                         window->GetY());
        }
        if (window->GetY()+window->GetHeight()-1>OS::display->GetScreenHeight()
            && window->GetHeight()<OS::display->GetScreenHeight()) {
          window->SetPos(window->GetX(),
                         OS::display->GetScreenHeight()-window->GetHeight());
        }
      }
    }

    Window::SetMain(container);
  }

  bool Popup::Open(bool activate)
  {
    // Popups should recalculate their size on each Open() call
    if (!IsOpen() && GetWindow()->IsInited()) {
      ReinitWindow();
    }

    return Window::Open(activate);
  }

  void Popup::Resync(Base::Model* model, const Base::ResyncMsg& msg)
  {
    if (model==GetWindow()->GetClosedAction() &&
        GetWindow()->GetClosedAction()->IsFinished()) {
      if (GetWindow()->IsInEventLoop()) {
        Exit();
      }
      else {
        Close();
        return;
      }
    }
    else if (model==GetWindow()->GetFocusOutAction() &&
             GetWindow()->GetFocusOutAction()->IsFinished()) {
      if (GetType()==OS::Window::typePopup)  {
        if (GetWindow()->IsInEventLoop()) {
          Exit();
        }
        else {
          Close();
          return;
        }
      }
    }
    else if (model==GetWindow()->GetEventAction() &&
             GetWindow()->GetEventAction()->IsFinished()) {
      const OS::Window::EventMsg *eventMsg=dynamic_cast<const OS::Window::EventMsg*>(&msg);
      OS::MouseEvent             *mouseEvent;
      OS::KeyEvent               *keyEvent;

      /*
        We close the window if we are popup and the user clicks
        outside the window or presses escape.
      */
      if ((mouseEvent=dynamic_cast<OS::MouseEvent*>(eventMsg->event))!=NULL) {
        if (mouseEvent->type==OS::MouseEvent::down && !GetWindow()->CursorIsIn()) {
          if (GetWindow()->IsInEventLoop()) {
            Exit();
          }
          else {
            Close();
            return;
          }
        }
      }
      else if ((keyEvent=dynamic_cast<OS::KeyEvent*>(eventMsg->event))!=NULL) {
        if (keyEvent->type==OS::KeyEvent::down &&
            keyEvent->qualifier==0 &&
            keyEvent->key==OS::keyEscape) {
          if (GetWindow()->IsInEventLoop()) {
            Exit();
          }
          else {
            Close();
            return;
          }
        }
      }
    }

    Window::Resync(model,msg);
  }
}

