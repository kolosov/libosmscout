#ifndef LUM_OS_WIN32_WINDOW_H
#define LUM_OS_WIN32_WINDOW_H

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

#include <Lum/OS/Base/Window.h>

#include <Lum/OS/Win32/Display.h>
#include <Lum/OS/Win32/Event.h>
#include <Lum/OS/Win32/EventLoop.h>

namespace Lum {
  namespace OS {
    namespace Win32 {

      class LUMAPI Window : public OS::Base::Window
      {
        friend class Display;

      private:
        Display      *display;

        long         balloonId;


        int          wOff,hOff,
                     wDelta,hDelta; /* border of window */
        bool         drawn;         /* window has been drawn */
        bool         queueFocus;    /* focus event is queued to be sent following redraw. */
        
        OS::DrawInfo *paintDraw;

      public:
        HWND         window;

      public:
        Window(OS::Display* display);
        ~Window();

        // setter
        void SetTitle(const std::wstring& name);

        // action
        void Grab(bool grab);

        bool HandleDrag(OS::MouseEvent* event);
        bool HandleDragDrop(OS::Event* event);

        void RecalcWindowPosition();

        void MouseGrabOn();
        void MouseGrabOff();

        void Init();

        void Resize(size_t width, size_t height);

        bool Open(bool activate);
        void Close();
        bool Show(bool activate);
        bool Hide();

        bool GetMousePos(int& x, int& y) const;

        void Enable();
        void Disable();

        void TriggerDraw(int x, int y, size_t width, size_t height);
        void OnPaint();
        
        bool SendBalloonMessage(const std::wstring& text,unsigned long timeout);

        LRESULT MessageHandler(HWND wnd,UINT msg, WPARAM wParam, LPARAM lParam);
      };
    }
  }
}

#endif


