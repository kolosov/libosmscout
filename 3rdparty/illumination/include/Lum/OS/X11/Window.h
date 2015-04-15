#ifndef LUM_OS_X11_WINDOW_H
#define LUM_OS_X11_WINDOW_H

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

// std
#include <string>

// X11
#include <X11/Xlib.h>
#include <X11/Xutil.h>

// Lum
#include <Lum/Private/Config.h>

#include <Lum/OS/Base/Display.h>
#include <Lum/OS/Base/DrawInfo.h>
#include <Lum/OS/Base/Window.h>

#include <Lum/OS/Event.h>

#include <Lum/OS/X11/Event.h>

namespace Lum {
  namespace OS {
    namespace X11 {

      class Display;

      struct Rectangle
      {
        int    x;
        int    y;
        size_t width;
        size_t height;
      };

      class LUMAPI Window : public OS::Base::Window
      {
        friend class Display;

      private:
        Display                *display;
        long                   balloonId;
        ::Window               window;
        bool                   fullScreen;
        Atom                   customWindowType;
        Model::ActionRef       checkRefreshAction;
        std::vector<Rectangle> redrawRects;

      public:
        XIC         xic;

      private:
        bool IsFullScreenInternal() const;

      public:
        Window(OS::Display* display);
        ~Window();

        void Resync(Lum::Base::Model* model,
                    const Lum::Base::ResyncMsg& msg);
        // setter
        void SetTitle(const std::wstring& name);

        // getter
        ::Drawable GetDrawable() const;
        ::Display* GetDisplay() const;

        void MouseGrabOn();
        void MouseGrabOff();
        void KeyboardGrabOn();
        void KeyboardGrabOff();

        bool HandleDrag(OS::MouseEvent* event);
        bool HandleDragDrop(OS::Event* event);

        void RecalcWindowPosition();

        void OnFocusIn();
        void OnFocusOut();

        bool HandleEvent(OS::Event* event);
        bool CheckRefresh();

        void Init();

        void Resize(size_t width, size_t height);

        void SetSize(size_t width, size_t height);
        void SetMinMaxSize(size_t minWidth,
                           size_t minHeight,
                           size_t maxWidth,
                           size_t maxHeight);

        bool Open(bool activate);
        void Close();
        bool Show(bool activate);
        bool Hide();

        bool GetMousePos(int& x, int& y) const;

        void Enable();
        void Disable();

        bool SendBalloonMessage(const std::wstring& text,unsigned long timeout);

        void TriggerDraw(int x, int y, size_t width, size_t height);

        bool SetFullScreen();
        bool LeaveFullScreen();
        bool IsFullScreen() const;

        bool SetOpacity(double opacity);

        void SetCustomWindowType(Atom type);
      };
    }
  }
}

#endif
