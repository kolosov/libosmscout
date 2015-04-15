#ifndef LUM_OS_DRIVER_H
#define LUM_OS_DRIVER_H

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

#include <Lum/OS/Bitmap.h>
#include <Lum/OS/Display.h>
#include <Lum/OS/DrawInfo.h>
#include <Lum/OS/Event.h>
#include <Lum/OS/Font.h>
#include <Lum/OS/Tray.h>
#include <Lum/OS/Window.h>

namespace Lum {
  namespace Images {
    class Factory;
  }

  namespace OS {

    class LUMAPI Driver
    {
    protected:
      std::wstring    driverName;
      int             priority;

    protected:
      Driver(const std::wstring name, int priority);

    public:
      virtual ~Driver();

      std::wstring GetDriverName() const;
      int GetPriority() const;

      virtual Display* CreateDisplay() const = 0;
      virtual Window* CreateWindow() const = 0;
      virtual Bitmap* CreateBitmap(size_t width, size_t height) const = 0;
      //::Lum::OS::Display::BitmapPtr CreateBitmapPattern(char pattern[], int width, int height);
      virtual DrawInfo* CreateDrawInfo(Window* window) = 0;
      virtual DrawInfo* CreateDrawInfo(Bitmap* bitmap) = 0;
      virtual Font* CreateFont() const = 0;
      virtual Tray* CreateTray() const = 0;
      virtual Images::Factory* CreateImageFactory(Display* display) const = 0;
    };

    extern LUMAPI Driver* driver;
  }
}

#endif
