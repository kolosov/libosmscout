/*
  This source is part of the Illumination library
  Copyright (C) 2006  Tim Teulings

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

#include <Lum/OS/Carbon/Window.h>

#include <Lum/Private/Config.h>

#include <assert.h>

#include <iostream>

#include <Lum/OS/Carbon/Display.h>
#include <Lum/OS/Carbon/Event.h>

#include <Lum/Base/Util.h>

namespace Lum {
  namespace OS {
    namespace Carbon {

      Window::Window(::Lum::OS::DisplayPtr display)
      : display(dynamic_cast<DisplayPtr>(display)),
        grab(false),exit(true),/*oldPress(0),lastPress(0),thisPress(0),balloonId(0),*/
        window(0),tracking(0)
      {
      }

      void Window::SetTitle(const std::wstring& name)
      {
        ::Lum::OS::Base::Window::SetTitle(name);

        if (window==0) {
          return;
        }

        ::CFStringRef string;

        string=::CFStringCreateWithCString(NULL,::Lum::Base::WStringToUTF8(name).c_str(),
                                           ::kCFStringEncodingUTF8);

        ::SetWindowTitleWithCFString(window,string);

        ::CFRelease(string);
      }

      /**
        Grabs the mouse cursor and keyboard.
      */
      void Window::GrabOn()
      {
        display->StopContextHelp();
        /*
        if (XGrabPointer(display->display,window,
                         False,
                         StructureNotifyMask|ButtonPressMask|ButtonReleaseMask|ButtonMotionMask|PointerMotionMask|PointerMotionHintMask|LeaveWindowMask|EnterWindowMask|VisibilityChangeMask|FocusChangeMask,
                         GrabModeAsync,GrabModeAsync,
                         None,
                         None,
                         CurrentTime)!=GrabSuccess) {
          std::cerr << "Cannot grab pointer" << std::endl;
        }
        if (XGrabKeyboard(display->display,window,
                          False,
                          GrabModeAsync,GrabModeAsync,
                          CurrentTime)!=GrabSuccess) {
          std::cerr << "Cannot grab keyboard" << std::endl;
        }*/
      }

      /**
        Releases the grab of the mouse cursor and the keyboard.
      */
      void Window::GrabOff()
      {
        //XUngrabPointer(display->display,CurrentTime);
        //XUngrabKeyboard(display->display,CurrentTime);
        display->RestartContextHelp();
      }

      void Window::OnFocusIn()
      {
        if (!grab) {
          display->StartContextHelp();
        }
        TriggerFocusIn();
      }

      void Window::OnFocusOut()
      {
        if (!grab) {
          display->StopContextHelp();
        }
        TriggerFocusOut();
      }

      bool Window::HandleEvent(::Lum::OS::EventPtr event)
      {
        ::Rect     rect;
        ::Point    point;
        ::EventRef carbonEvent;

        GetCarbonEvent(event,carbonEvent);

        switch (::GetEventClass(carbonEvent)) {
        case ::kEventClassWindow:
          switch (::GetEventKind(carbonEvent)) {
          case ::kEventWindowClose:
            if (modalCount==0) {
              TriggerClosed();
            }

            return true;
          case ::kEventWindowDrawContent:
            std::cout << "DrawContent" << std::endl;
            TriggerRedraw(0,0,width,height);
            return true;
          case ::kEventWindowShowing:
            std::cout << "Showing" << std::endl;
            // TODO: Get Window coords
            TriggerMaped();
            if (grab) {
              GrabOn();
            }
            return true;
          case ::kEventWindowHidden:
            std::cout << "Hidden" << std::endl;
            TriggerUnmaped();
            return true;
          case ::kEventWindowActivated:
            std::cout << "Activated" << std::endl;
            //OnFocusIn();
            return true;
          case ::kEventWindowDeactivated:
            std::cout << "Deactivated" << std::endl;
            //OnFocusOut();
            return true;
          case ::kEventWindowBoundsChanged:
            std::cout << "BoundsChanged" << std::endl;
            if (::GetEventParameter(carbonEvent,
                                    ::kEventParamCurrentBounds,
                                    ::typeQDRectangle,
                                    NULL,
                                    sizeof(::Rect),
                                    NULL,
                                    &rect)==0) {
              x=rect.left;
              y=rect.top;

              if (width!=rect.right-rect.left+1 ||
                  height!=rect.bottom-rect.top+1) {
                width=rect.right-rect.left+1;
                height=rect.bottom-rect.top+1;
                TriggerResize(width,height);
              }
            }
            return true;
          case ::kEventWindowFocusAcquired:
            std::cout << "FocusAquired" << std::endl;
            OnFocusIn();
            return true;
          case ::kEventWindowFocusRelinquish:
            std::cout << "Focusrelinquish" << std::endl;
            OnFocusOut();
            return true;
          case ::kEventWindowGetMinimumSize:
            std::cout << "GetMinimumSize" << std::endl;
            point.h=width;
            point.v=height;
            if (::SetEventParameter(carbonEvent,
                                    ::kEventParamDimensions,
                                    ::typeQDPoint,
                                    sizeof(::Point),
                                    &point)==0) {
              // TODO?
            }
            return true;
          case ::kEventWindowGetMaximumSize:
            std::cout << "MaximumSize" << std::endl;
            return true;
          default:
            std::cout << "<other>" << std::endl;
            break;
          }
          break;
        case ::kEventClassMouse:
          switch (::GetEventKind(carbonEvent)) {
          case ::kEventMouseDown:
          case ::kEventMouseUp:
          case ::kEventMouseMoved:
          case ::kEventMouseDragged:
            if (modalCount!=0) {
              return true;
            }
            break;
          default:
            break;
          }
          break;
        default:
          std::cout << "<other>" << std::endl;
          break;
        }

        return ::Lum::OS::Base::Window::HandleEvent(event);
      }

#if 0
          bool Window::HandleEvent(::Lum::OS::EventPtr event)
          {
            XEvent x11Event;

            ::Lum::OS::X11::GetX11Event(event,x11Event);

            switch (x11Event.type) {
            case EnterNotify:
              TriggerMouseIn();
              return true;
            case LeaveNotify:
              TriggerMouseOut();
              return true;
            case VisibilityNotify:
              if (x11Event.xvisibility.state>0) {
                TriggerHidden();
              }
              return true;
            case ButtonPress:
              oldButton=lastButton;
              oldPress=lastPress;
              lastButton=thisButton;
              lastPress=thisPress;
              thisButton=*dynamic_cast<MouseEventPtr>(event);
              thisPress=x11Event.xbutton.time;

              if (modalCount!=0) {
                return true;
              }
              break;
            case ButtonRelease:
              if (modalCount!=0) {
                return true;
              }
              break;
            case MotionNotify:
              // skip all pending motion events and evaluate only the last available
              while (XCheckMaskEvent(display->display,ButtonMotionMask,&x11Event)) {
              }
              ::Lum::OS::X11::GetX11Event(event,x11Event);
              if (modalCount!=0) {
                return true;
              }
              break;
            default:
              break;
            }
            return ::Lum::OS::Base::Window::HandleEvent(event);
      }
#endif

      void Window::Resize(size_t width, size_t height)
      {
        ::Lum::OS::Base::Window::Resize(width,height);

        ::SizeWindow(window,width,height,true);
      }

      void Window::Grab(bool grab)
      {
        if (this->grab==grab) {
          return;
        }
        this->grab=grab;
        if (IsOpen()) {
          if (this->grab) {
            GrabOn();
          }
          else {
            GrabOff();
          }
        }
      }

      ::OSStatus EventHandler(::EventHandlerCallRef nextHandler,
                              ::EventRef carbonEvent,
                              ::LogicalAddress data)
      {
        Window*             window=static_cast<Window*>(data);
        ::Lum::OS::EventPtr event=::Lum::OS::Carbon::GetEvent(carbonEvent);

        if (window->HandleEvent(event)) {
          return ::eventNotHandledErr;
        }

        /* ignore */ ::CallNextEventHandler(nextHandler,carbonEvent);

        return noErr;
      }

      bool Window::Open(bool activate)
      {
        ::Rect            rect;
        int               res;
        ::EventHandlerUPP handler;
        ::EventTargetRef  targetRef;
        ::EventTypeSpec   spec[20];
        bool              xManPos,yManPos;

        if (!::Lum::OS::Base::Window::Open(activate)) {
          return false;
        }

        CalculateWindowPos(xManPos,yManPos);

        rect.top=y;
        rect.left=x;
        rect.bottom=y+height-1;
        rect.right=x+width-1;

        switch (type) {
        case typeMenu:
        case typePopup:
        case typeSplash:
          res=::CreateNewWindow(::kHelpWindowClass,
                                ::kWindowStandardHandlerAttribute,
                                &rect,
                                &window);
          break;
        default:
          res=::CreateNewWindow(::kDocumentWindowClass,
                                ::kWindowStandardDocumentAttributes|
                                ::kWindowStandardHandlerAttribute|
                                ::kWindowLiveResizeAttribute,
                                &rect,
                                &window);
        }

        if (res!=0) {
          std::cerr << "Cannot create window: " << res << std::endl;
          return false;
        }

        if (CreateCGContextForPort(GetWindowPort(window),&context)!=::noErr) {
          std::cerr << "Cannot create context for font scratch!" << std::endl;
          return false;
        }

        targetRef=::GetWindowEventTarget(window);
        if (::InstallStandardEventHandler(targetRef)!=0) {
          std::cerr << "Cannot install standard event handler" << std::endl;
        }

        spec[ 0].eventClass=::kEventClassWindow;
        spec[ 0].eventKind=::kEventWindowDrawContent;

        spec[ 1].eventClass=::kEventClassWindow;
        spec[ 1].eventKind=::kEventWindowShowing;

        spec[ 2].eventClass=::kEventClassWindow;
        spec[ 2].eventKind=::kEventWindowHidden;

        spec[ 3].eventClass=::kEventClassWindow;
        spec[ 3].eventKind=::kEventWindowActivated;

        spec[ 4].eventClass=::kEventClassWindow;
        spec[ 4].eventKind=::kEventWindowDeactivated;

        spec[ 5].eventClass=::kEventClassWindow;
        spec[ 5].eventKind=::kEventWindowClose;

        spec[ 6].eventClass=::kEventClassWindow;
        spec[ 6].eventKind=::kEventWindowBoundsChanged;

        spec[ 7].eventClass=::kEventClassWindow;
        spec[ 7].eventKind=::kEventWindowFocusAcquired;

        spec[ 8].eventClass=::kEventClassWindow;
        spec[ 8].eventKind=::kEventWindowFocusRelinquish;

        spec[ 9].eventClass=::kEventClassWindow;
        spec[ 9].eventKind=::kEventWindowGetMinimumSize;

        spec[10].eventClass=::kEventClassWindow;
        spec[10].eventKind=::kEventWindowGetMaximumSize;

        spec[11].eventClass=::kEventClassMouse;
        spec[11].eventKind=::kEventMouseUp;

        spec[12].eventClass=::kEventClassMouse;
        spec[12].eventKind=::kEventMouseDown;

        spec[13].eventClass=::kEventClassMouse;
        spec[13].eventKind=::kEventMouseMoved;

        spec[14].eventClass=::kEventClassMouse;
        spec[14].eventKind=::kEventMouseDragged;

        spec[15].eventClass=::kEventClassMouse;
        spec[15].eventKind=::kEventMouseEntered;

        spec[16].eventClass=::kEventClassMouse;
        spec[16].eventKind=::kEventMouseExited;

        spec[17].eventClass=::kEventClassKeyboard;
        spec[17].eventKind=::kEventRawKeyDown;

        spec[18].eventClass=::kEventClassKeyboard;
        spec[18].eventKind=::kEventRawKeyRepeat;

        spec[19].eventClass=::kEventClassKeyboard;
        spec[19].eventKind=::kEventRawKeyUp;

        handler=::NewEventHandlerUPP(EventHandler);

        if (::InstallEventHandler(targetRef,
                                  handler,
                                  sizeof(spec)/sizeof(EventTypeSpec),
                                  spec,
                                  this,
                                  &this->handler)) {
          std::cerr << "Cannot install event handler" << std::endl;
        }

        SetTitle(GetTitle());

        draw=::Lum::OS::Factory::factory->CreateDrawInfo(this);

        display->AddWindow(this);

        ::ShowWindow(window);

        switch (type) {
        case typePopup:
        case typeSplash:
          if (::SetUserFocusWindow(window)!=noErr) {
            std::cerr << "Cannot set keboard focus!" << std::endl;
          }
          break;
        default:
          break;
        }

        return true;
      }

      void Window::Close()
      {
        if (IsOpen() && window!=0) {
          if (grab) {
            GrabOff();
          }
        }

        if (window!=0) {
          ::HideWindow(window);
          ::RemoveEventHandler(handler);

          if (draw!=NULL) {
            delete draw;
            draw=NULL;
          }

          ::DisposeWindow(window);
        }

        display->RemoveWindow(this);

        ::Lum::OS::Base::Window::Close();
      }

      bool Window::Show(bool activate)
      {
        assert(IsOpen() && !IsShown());

        ::ShowWindow(window);

        return true;
      }

      bool Window::Hide()
      {
        assert(IsOpen() && IsShown());

        ::HideWindow(window);
      }

      bool Window::GetMousePos(int& x, int& y) const
      {
        ::Point point;

        ::SetGWorld(::GetWindowPort(window),NULL);
        ::GetMouse(&point);

        x=point.h;
        y=point.v;

        return true;
      }

      void Window::EventLoop()
      {
        assert(exit);

        exit=false;

        DisableParents();

        /* ignore */ ::RunAppModalLoopForWindow(window);

        EnableParents();
      }

      void Window::Enable()
      {
        ::Lum::OS::Base::Window::Enable();

        if (modalCount==0) {
          //XUndefineCursor(display->display,window);
        }
      }

      void Window::Disable()
      {
        ::Lum::OS::Base::Window::Disable();

        if (modalCount==1) {
          //XDefineCursor(D.display(Display).display,w.window,D.display(Display).sleepCursor);
        }
      }

      bool Window::SendBalloonMessage(const std::wstring& text,
                                      unsigned long timeout)

      {
        return false;
      }

      bool Window::IsInEventLoop() const
      {

        return !exit;
      }

      bool Window::IsDoubleClicked() const
      {
        /*
        return (thisPress-lastPress<=display->multiClickTime)
          && (lastButton.type==thisButton.type)
          && (lastButton.qualifier==thisButton.qualifier)
          && (abs(lastButton.x-thisButton.x)<5)
          && (abs(lastButton.y-thisButton.y)<5);*/
        return false;
      }

      bool Window::IsTrippleClicked() const
      {
        return false;
        //return (w.oldButton.type==w.lastButton.type) && (w.oldButton.qualifier==w.lastButton.qualifier) && (ABS(w.oldButton.x-w.lastButton.x)<5) && (ABS(w.oldButton.y-w.lastButton.y)<5) && (w.lastPress-w.oldPress<=D.display(Display).multiClickTime) && (w.lastButton.type==w.thisButton.type) && (w.lastButton.qualifier==w.thisButton.qualifier) && (ABS(w.lastButton.x-w.thisButton.x)<5) && (ABS(w.lastButton.y-w.thisButton.y)<5) && (w.thisPress-w.lastPress<=D.display(Display).multiClickTime);
      }

      void Window::Exit()
      {
        assert(!exit);
        exit=true;

        /* ignore */ ::QuitAppModalLoopForWindow(window);
      }
    }
  }
}
