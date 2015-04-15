#ifndef LUM_OS_CURSES_EVENT_H
#define LUM_OS_CURSES_EVENT_H

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

#include <string>

#include <curses.h>

#include <Lum/OS/Base/Event.h>

#include <Lum/OS/Curses/Window.h>

namespace Lum {
  namespace OS {
    namespace Curses {

      class Event : public OS::Event
      {
      public:
        int event;
      };

      class KeyEvent : public OS::Base::KeyEvent
      {
      public:
        int event;

      public:
        void GetName(std::wstring& buffer) const;

        bool SetCursesEvent(Window* window, int event, bool meta);
      };

      class MouseEvent : public OS::Base::MouseEvent
      {
      public:
        int event;

      public:
        bool SetCursesEvent(Window* window, int event);
      };

      OS::Event* GetEvent(Window* window, int event, bool meta);
    }
  }
}

#endif
