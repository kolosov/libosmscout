#ifndef LUM_WIN32_EVENT_H
#define LUM_WIN32_EVENT_H

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

#include <Lum/OS/Base/Event.h>

#include <Lum/OS/Win32/OSAPI.h>

namespace Lum {
  namespace OS {
    namespace Win32 {

      class Window;
      
      class LUMAPI Event : public OS::Event
      {
      public:
        MSG event;
        
      public:
        Event(const MSG& msg);
      };

      class LUMAPI KeyEvent : public OS::Base::KeyEvent
      {
      public:
        MSG event;
      
      public:    
        KeyEvent(const MSG& msg);
        void GetName(std::wstring& buffer) const;
      };

      class LUMAPI MouseEvent : public OS::Base::MouseEvent
      {
      public:
        MSG event;
        
      public:  
        MouseEvent(Window* window, const MSG& msg);
      };
  
      OS::Event* GetEvent(Window* window, const MSG& msg);
    }
  }
}

#endif
