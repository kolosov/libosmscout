#ifndef LUM_OS_X11_EVENT_H
#define LUM_OS_X11_EVENT_H

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

#include <string>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <Lum/OS/Base/Event.h>

namespace Lum {
  namespace OS {
    namespace X11 {
      class Event : public OS::Event
      {
      public:
        XEvent event;

      public:
        Event(const XEvent& event);
      };

      class KeyEvent : public OS::Base::KeyEvent
      {
      public:
        XEvent event;
        KeySym xKey;
        bool   valid;

      public:
        KeyEvent();
        KeyEvent(XKeyEvent& event, XIC xic);

        bool IsValid() const;

        void GetName(std::wstring& buffer) const;
      };

      class MouseEvent : public OS::Base::MouseEvent
      {
      public:
        XEvent event;

      public:
        MouseEvent(XButtonEvent& event);
        MouseEvent(XMotionEvent& event);
      };

      OS::Event* GetEvent(XEvent event, XIC xic);
      void GetX11Event(OS::Event* event, XEvent& x11Event);
    }
  }
}

#endif
