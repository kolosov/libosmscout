/*
  This source is part of the Illumination library
  Copyright (C) 2005  Tim Teulings

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

#include <Lum/OS/Curses/Window.h>

#include <assert.h>

#include <Lum/Base/String.h>

#include <Lum/OS/Curses/Display.h>

namespace Lum {
  namespace OS {
    namespace Curses {

      Window::Window(OS::Display* display)
      : display(dynamic_cast<Display*>(display)),
        //oldPress(0),lastPress(0),thisPress(0),
        window(NULL),frame(NULL),panel(NULL),framePanel(NULL)
      {
        // no code
      }

      Window::~Window()
      {
        // no code
      }

      void Window::SetTitle(const std::wstring& name)
      {
        OS::Base::Window::SetTitle(name);

        if (frame!=0) {
          std::string tmp=Lum::Base::WStringToString(GetTitle());

          tmp=tmp.substr(0,width);

          wattrset(frame,A_NORMAL+COLOR_PAIR(display->GetColorDepth()*2*
                                             display->GetColor(Display::backgroundColor)+
                                             display->GetColor(Display::textColor)));
          wmove(frame,0,1);
          whline(frame,ACS_HLINE,width);
          mvwaddstr(frame,0,1,tmp.c_str());
        }
      }

      ::WINDOW* Window::GetWINDOW() const
      {
        return window;
      }

      /*
      ::Display* Window::GetDisplay() const
      {
        return display->display;
      }*/

      void Window::RecalcWindowPosition()
      {
        /*
        int      x;
        int      y;
        ::Window tmp;

        if (XTranslateCoordinates(display->display,window,
                                  XRootWindow(display->display,display->scrNum),
                                  0,0,&x,&y,&tmp)!=0) {
          this->x=x;
          this->y=y;
        }*/
      }

      void Window::OnFocusIn()
      {
        if (!grabMouse) {
          display->StartContextHelp();
        }
        TriggerFocusIn();
      }

      void Window::OnFocusOut()
      {
        if (!grabMouse) {
          display->StopContextHelp();
        }
        TriggerFocusOut();
      }


      bool Window::HandleEvent(OS::Event* event)
      {
        KeyEvent *keyEvent;

        keyEvent=dynamic_cast<KeyEvent*>(event);
        if (keyEvent!=NULL && keyEvent->type==KeyEvent::up && keyEvent->text==L"x" &&
            (keyEvent->qualifier & (qualifierAltLeft|qualifierAltRight)) &&
            modalCount==0) {
          TriggerClosed();
          return true;
        }

        return OS::Base::Window::HandleEvent(event);
      }

      void Window::Resize(size_t width, size_t height)
      {
        OS::Base::Window::Resize(width,height);

        // TODO
      }

      void Window::TriggerDraw(int x, int y, size_t width, size_t height)
      {
        // TODO
      }

      bool Window::Open(bool activate)
      {
        bool xManPos;
        bool yManPos;

        if (!OS::Base::Window::Open(activate)) {
          return false;
        }

        if (width>display->GetWorkAreaWidth() || height>display->GetWorkAreaHeight()) {
          return false;
        }

        CalculateWindowPos(xManPos,yManPos);

        switch (type) {
        case typeMain:
        case typeDialog:
        case typeUtility:
          if (x>0 && y>0 && width+2<=display->GetWorkAreaWidth() && height+2<=display->GetWorkAreaHeight()) {
            frame=newwin(height+2,width+2,y-1,x-1);

            if (frame==NULL) {
              return false;
            }

            framePanel=new_panel(frame);
            if (framePanel==NULL) {
              delwin(frame);
              return false;
            }
            wattrset(frame,A_NORMAL+COLOR_PAIR(display->GetColorDepth()*2*
                                               display->GetColor(Display::backgroundColor)+
                                               display->GetColor(Display::textColor)));
            box(frame,0,0);
          }
          break;
        default:
          break;
        }

        window=newwin(height,width,y,x);

        if (window==NULL) {
          if (frame!=NULL) {
            del_panel(framePanel);
            delwin(frame);
          }
          return false;
        }

        nodelay(window,true);
        meta(window,true);
        keypad(window,true);
        scrollok(window,false);
        wtimeout(window,1000); // 1 second read timeout

        panel=new_panel(window);

        if (panel==NULL) {
          if (frame!=NULL) {
            del_panel(framePanel);
            delwin(frame);
          }
          delwin(window);
          window=NULL;
          return false;
        }

        SetTitle(GetTitle());

        draw=driver->CreateDrawInfo(this);

        TriggerMaped();
        //TriggerRefresh(0,0,width,height);
        OnFocusIn();
        TriggerMouseIn();
        display->AddWindow(this);

        return true;
      }

      void Window::Close()
      {
        if (IsOpen() && window!=NULL) {
          GrabMouse(false);
          GrabKeyboard(false);
        }

        TriggerMouseOut();
        OnFocusOut();
        TriggerUnmaped();
        display->RemoveWindow(this);

        if (draw!=NULL) {
          delete draw;
          draw=NULL;
        }

        if (panel!=NULL) {
          del_panel(panel);
          panel=NULL;
        }

        if (window!=NULL) {
          delwin(window);
          window=NULL;
        }

        if (framePanel!=NULL) {
          del_panel(framePanel);
          framePanel=NULL;
        }

        if (frame!=NULL) {
          delwin(frame);
          frame=NULL;
        }

        OS::Base::Window::Close();
      }

      bool Window::Show(bool activate)
      {
        assert(IsOpen() && !IsShown());

        return false;
      }

      bool Window::Hide()
      {
        assert(IsOpen() && IsShown());

        return false;
      }

      bool Window::GetMousePos(int& /*x*/, int& /*y*/) const
      {
        return false;
        /*
        ::Window     root;
        ::Window     child;
        unsigned int bmask;
        int          rx,ry;

        return XQueryPointer(display->display,
                             window,&root,&child,
                             &rx,&ry,&x,&y,&bmask)!=False;*/
      }

      void Window::Enable()
      {
        OS::Base::Window::Enable();

        if (modalCount==0) {
          //XUndefineCursor(display->display,window);
        }
      }

      void Window::Disable()
      {
        OS::Base::Window::Disable();

        if (modalCount==1) {
          //XDefineCursor(D.display(Display).display,w.window,D.display(Display).sleepCursor);
        }
      }

      bool Window::SendBalloonMessage(const std::wstring& /*text*/,
                                      unsigned long /*timeout*/)

      {
        return false;
      }
    }
  }
}
