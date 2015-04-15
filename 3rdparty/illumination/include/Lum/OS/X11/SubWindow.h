#ifndef LUM_OS_X11_SUBWINDOW_H
#define LUM_OS_X11_SUBWINDOW_H

/*
  This source is part of the Illumination library
  Copyright (C) 2007  Tim Teulings

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

#include <Lum/Object.h>

// X11
#include <X11/Xlib.h>

namespace Lum {
  namespace OS {
    namespace X11 {

      /**
        Abstract base class for (external) controls that require an underlying X11
        child window (in relation to the top level dialog window) to act on.
      */
      class LUMAPI SubWindow : public Object
      {
      private:
        bool done;
        bool raised;
        bool destroyOnCleanup;

      protected:
        ::Window     window;
        OS::DrawInfo *drawInfo;

      public:
        SubWindow();
        ~SubWindow();

        void SetDestroyOnCleanup(bool destroyOnCleanup);

        void Layout();
        void Draw(OS::DrawInfo* draw,
                  int x, int y, size_t w, size_t h);
        void Hide();
      };
    }
  }
}

#endif
