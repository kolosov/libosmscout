#ifndef LUM_OS_WIN32_DRIVER_H
#define LUM_OS_WIN32_DRIVER_H

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

#include <Lum/Base/Singleton.h>

#include <Lum/OS/Base/Display.h>

namespace Lum {
  namespace OS {
    namespace Win32 {

      class Win32Driver : public OS::Base::Driver
      {
      public:
        Win32Driver();

        OS::Display* CreateDisplay() const;
        OS::Window* CreateWindow() const;
        OS::Bitmap* CreateBitmap(size_t width, size_t height) const;
        OS::DrawInfo* CreateDrawInfo(OS::Window* window);
        OS::DrawInfo* CreateDrawInfo(OS::Bitmap* bitmap);
        OS::Font* CreateFont() const;
        OS::Tray* CreateTray() const;
        Images::Factory* CreateImageFactory(OS::Display* display) const;
      };

      extern Win32Driver win32Driver;
    }
  }
}

#endif
