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

#include <Lum/OS/Curses/Driver.h>

#include <Lum/OS/Curses/Bitmap.h>
#include <Lum/OS/Curses/Display.h>
#include <Lum/OS/Curses/DrawInfo.h>
#include <Lum/OS/Curses/Font.h>
//#include <Lum/OS/Curses/Image.h>
#include <Lum/OS/Curses/Tray.h>
#include <Lum/OS/Curses/Window.h>

namespace Lum {
  namespace OS {
    namespace Curses {

      CursesDriver::CursesDriver()
      : Base::Driver(L"Curses",0)
      {
        // no code
      }

      OS::Display* CursesDriver::CreateDisplay() const
      {
        return new Display();
      }

      OS::Window* CursesDriver::CreateWindow() const
      {
        return new Window(display);
      }

      OS::Bitmap* CursesDriver::CreateBitmap(size_t width, size_t height) const
      {
        assert(width>0 && height>0);

        return NULL;
        //return new Bitmap(width,height);
      }

      OS::DrawInfo* CursesDriver::CreateDrawInfo(OS::Window* window)
      {
        return new DrawInfo(window);
      }
      OS::DrawInfo* CursesDriver::CreateDrawInfo(OS::Bitmap* bitmap)
      {
        return new DrawInfo(bitmap);
      }

      OS::Font* CursesDriver::CreateFont() const
      {
        return new Font();
      }

      OS::Tray* CursesDriver::CreateTray() const
      {
        return new Tray();
      }

      Images::Factory* CursesDriver::CreateImageFactory(OS::Display* /*display*/) const
      {
        return NULL;
        //return new ImageFactory(display);
      }

      CursesDriver cursesDriver;
    }
  }
}
