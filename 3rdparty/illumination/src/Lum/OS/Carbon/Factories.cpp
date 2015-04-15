#include <Lum/OS/Carbon/Factories.h>

/*
  This source is part of the Illumination library
  Copyright (C) 2006  Tim Teulings

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

#include <Lum/OS/Base/Bitmap.h>
#include <Lum/OS/Carbon/Display.h>
#include <Lum/OS/Carbon/DrawInfo.h>
#include <Lum/OS/Carbon/Font.h>
//#include <Lum/OS/Base/Tray.h>
#include <Lum/OS/Carbon/Window.h>

namespace Lum {
  namespace OS {
    namespace Carbon {

      CarbonFactory::CarbonFactory()
      {
        this->driverName=L"Carbon";
        //SetPriority(10);
      }

      ::Lum::OS::DisplayPtr CarbonFactory::CreateDisplay() const
      {
        return new Display();
      }

      ::Lum::OS::WindowPtr CarbonFactory::CreateWindow() const
      {
        return new Window(display);
      }

      ::Lum::OS::BitmapPtr CarbonFactory::CreateBitmap(size_t width, size_t height) const
      {
        assert(width>0 && height>0);

        return NULL;//new Bitmap(width,height);
      }

      ::Lum::OS::DrawInfoPtr CarbonFactory::CreateDrawInfo(::Lum::OS::WindowPtr window)
      {
        return new DrawInfo(window);
      }
      ::Lum::OS::DrawInfoPtr CarbonFactory::CreateDrawInfo(::Lum::OS::BitmapPtr bitmap)
      {
        return new DrawInfo(bitmap);
      }

      ::Lum::OS::FontPtr CarbonFactory::CreateFont() const
      {
        return new Font();
      }

      ::Lum::OS::TrayPtr CarbonFactory::CreateTray() const
      {
        return NULL;//new Tray();
      }

      ::Lum::Images::Factory* CarbonFactory::CreateImageFactory(::Lum::OS::DisplayPtr display) const
      {
        return NULL;//new ImageFactory(display);
      }
    }
  }
}
