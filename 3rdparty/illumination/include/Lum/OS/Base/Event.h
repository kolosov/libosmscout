#ifndef LUM_BASE_EVENT_H
#define LUM_BASE_EVENT_H

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

#include <Lum/OS/Event.h>

namespace Lum {
  namespace OS {
    namespace Base {

      class LUMAPI KeyEvent : public OS::KeyEvent
      {
      };

      class LUMAPI MouseEvent : public OS::MouseEvent
      {
      private:
        bool grabed;

      public:
        MouseEvent();

        virtual bool IsGrabStart() const;
        virtual bool IsGrabEnd() const;
        virtual bool IsGrabed() const;

        void SetGrabed(bool grabed);
      };
    }
  }
}

#endif
