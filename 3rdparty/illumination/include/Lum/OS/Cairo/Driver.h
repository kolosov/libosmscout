#ifndef LUM_OS_CAIRO_DRIVER_H
#define LUM_OS_CAIRO_DRIVER_H

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

#include <Lum/Private/Config.h>

// Cairo
#if defined(__WIN32__) || defined(WIN32) || defined(__APPLE__)
#include <cairo.h>
#else
#include <cairo/cairo.h>
#endif

#include <Lum/OS/Driver.h>
#include <Lum/OS/Base/Display.h>

namespace Lum {
  namespace OS {
    namespace Cairo {

      extern LUMAPI ::cairo_t* GetGlobalCairo();
      extern void FreeGlobalCairo();

      extern LUMAPI ::cairo_surface_t* GetGlobalSurface();
      extern void FreeGlobalSurface();

#if defined(CAIRO_HAS_WIN32_SURFACE)
      class CairoWin32Driver : public Base::Driver
      {
      public:
        CairoWin32Driver();

        OS::Display* CreateDisplay() const;
        OS::Window* CreateWindow() const;
        OS::Bitmap* CreateBitmap(size_t width, size_t height) const;
        OS::DrawInfo* CreateDrawInfo(OS::Window* window);
        OS::DrawInfo* CreateDrawInfo(OS::Bitmap* bitmap);
        OS::Font* CreateFont() const;
        OS::Tray* CreateTray() const;
        Images::Factory* CreateImageFactory(OS::Display* display) const;
      };

      extern CairoWin32Driver cairoWin32Driver;
#endif

#if defined(CAIRO_HAS_XLIB_SURFACE)
      class CairoX11Driver : public Base::Driver
      {
      public:
        CairoX11Driver();

        OS::Display* CreateDisplay() const;
        OS::Window* CreateWindow() const;
        OS::Bitmap* CreateBitmap(size_t width, size_t height) const;
        OS::DrawInfo* CreateDrawInfo(OS::Window* window);
        OS::DrawInfo* CreateDrawInfo(OS::Bitmap* bitmap);
        OS::Font* CreateFont() const;
        OS::Tray* CreateTray() const;
        Images::Factory* CreateImageFactory(OS::Display* display) const;
      };

      extern CairoX11Driver cairoX11Driver;
#endif

#if defined(CAIRO_HAS_QUARTZ_SURFACE)
      class CairoQuartzDriver : public Base::Driver
      {
      public:
        CairoQuartzDriver();

        OS::Display* CreateDisplay() const;
        OS::Window* CreateWindow() const;
        OS::Bitmap* CreateBitmap(size_t width, size_t height) const;
        OS::DrawInfo* CreateDrawInfo(OS::Window* window);
        OS::DrawInfo* CreateDrawInfo(OS::Bitmap* bitmap);
        OS::Font* CreateFont() const;
        OS::Tray* CreateTray() const;
        Images::Factory* CreateImageFactory(OS::Display* display) const;
      };

      extern CairoQuartzDriver cairoQuartzDriver;
#endif
    }
  }
}

#endif
