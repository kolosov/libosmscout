#ifndef LUM_OS_CARBON_WINDOW_H
#define LUM_OS_CARBON_WINDOW_H

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

// std
#include <string>

// Carbon
#include <Carbon/Carbon.h>

// Lum
#include <Lum/OS/Base/Display.h>
#include <Lum/OS/Base/DrawInfo.h>
#include <Lum/OS/Base/Window.h>

#include <Lum/OS/Event.h>

//#include <Lum/OS/X11/Event.h>

namespace Lum {
  namespace OS {
    namespace Carbon {

      class Display;
      typedef Display *DisplayPtr;

      class LUMAPI Window : public ::Lum::OS::Base::Window
      {
        friend class Display;

      private:
        DisplayPtr        display;
        bool              grab;
        bool              exit;
        /*MouseEvent      oldButton;
        MouseEvent        lastButton;
        MouseEvent        thisButton;
        Time              oldPress;
        Time              lastPress;
        Time              thisPress;*/

        ::EventHandlerRef handler;
      public:
        ::WindowPtr       window;
        ::CGContextRef    context;

      private:
        MouseTrackingRef tracking;

      public:
        Window(::Lum::OS::DisplayPtr display);

        // setter
        void SetTitle(const std::wstring& name);

        // action
        void Grab(bool grab);

        bool HandleDrag(::Lum::OS::MouseEventPtr event);
        bool HandleDragDrop(::Lum::OS::EventPtr event);

        //void RecalcWindowPosition();

        void GrabOn();
        void GrabOff();

        void OnFocusIn();
        void OnFocusOut();

        bool HandleEvent(::Lum::OS::EventPtr event);

        void Init();

        void Resize(size_t width, size_t height);

        bool Open(bool activate);
        void Close();
        bool Show(bool activate);
        bool Hide();

        bool GetMousePos(int& x, int& y) const;

        void EventLoop();

        void Enable();
        void Disable();

        bool SendBalloonMessage(const std::wstring& text,unsigned long timeout);

        bool IsInEventLoop() const;
        bool IsDoubleClicked() const;
        bool IsTrippleClicked() const;

        void Exit();


      };
    }
  }
}

#endif
