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

#include <Lum/OS/Win32/Driver.h>

#include <Lum/OS/Win32/Bitmap.h>
#include <Lum/OS/Win32/Display.h>
#include <Lum/OS/Win32/DrawInfo.h>
#include <Lum/OS/Win32/Font.h>
#include <Lum/OS/Win32/Image.h>
#include <Lum/OS/Win32/Tray.h>
#include <Lum/OS/Win32/Window.h>

namespace Lum {
  namespace OS {
    namespace Win32 {

      Win32Driver::Win32Driver()
       : Driver(L"Win32",10)
      {
        // no code
      }

      OS::Display* Win32Driver::CreateDisplay() const
      {
        return new Display();
      }

      OS::Window* Win32Driver::CreateWindow() const
      {
        return new Window(display);
      }

      OS::Bitmap* Win32Driver::CreateBitmap(size_t width, size_t height) const
      {
        assert(width>0 && height>0);

        return new Bitmap(width,height);
      }

      OS::DrawInfo* Win32Driver::CreateDrawInfo(OS::Window* window)
      {
        return new DrawInfo(window);
      }

      OS::DrawInfo* Win32Driver::CreateDrawInfo(OS::Bitmap* bitmap)
      {
        return new DrawInfo(bitmap);
      }

      OS::Font* Win32Driver::CreateFont() const
      {
        return new Font();
      }

      OS::Tray* Win32Driver::CreateTray() const
      {
        return new Tray();
      }

      Images::Factory* Win32Driver::CreateImageFactory(OS::Display* display) const
      {
        return new ImageFactory(display);
      }

      Win32Driver win32Driver;
    }
  }
}
