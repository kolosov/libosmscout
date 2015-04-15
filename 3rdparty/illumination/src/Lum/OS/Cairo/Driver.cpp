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

#include <Lum/OS/Cairo/Driver.h>

#include <cassert>
#include <iostream>
#include <limits>

#include <Lum/OS/Cairo/Bitmap.h>
#include <Lum/OS/Cairo/DrawInfo.h>
#include <Lum/OS/Cairo/Font.h>
#include <Lum/OS/Cairo/Image.h>

#if defined(CAIRO_HAS_WIN32_SURFACE)
 #include <Lum/OS/Win32/Display.h>
 #include <Lum/OS/Win32/Window.h>
#endif

#if defined(CAIRO_HAS_QUARTZ_SURFACE)
  #include <Lum/OS/Carbon/Display.h>
  #include <Lum/OS/Carbon/Window.h>
#endif

#if defined(CAIRO_HAS_XLIB_SURFACE)
  #include <Lum/OS/X11/Display.h>
  #include <Lum/OS/X11/Window.h>
#endif

namespace Lum {
  namespace OS {
    namespace Cairo {

      static ::cairo_t         *globalCairo=NULL;
      static ::cairo_surface_t *globalSurface=NULL;

      ::cairo_t* GetGlobalCairo()
      {
        if (globalCairo==NULL) {
          cairo_surface_t *surface=GetGlobalSurface();

          globalCairo=cairo_create(surface);
          cairo_set_line_width(globalCairo,1);
          cairo_set_line_join(globalCairo,CAIRO_LINE_JOIN_MITER);
        }

        return globalCairo;
      }

      void FreeGlobalCairo()
      {
        if (globalCairo!=NULL) {
          cairo_destroy(globalCairo);
          globalCairo=NULL;
        }
      }

      ::cairo_surface_t* GetGlobalSurface()
      {
        if (globalSurface==NULL) {
#if defined(CAIRO_HAS_WIN32_SURFACE)
          globalSurface=cairo_win32_surface_create(GetDC(dynamic_cast< OS::Win32::Display*>(OS::display)->appWindow));
#elif defined(CAIRO_HAS_QUARTZ_SURFACE)
          globalSurface=cairo_quartz_surface_create(dynamic_cast< OS::Carbon::Display*>(OS::display)->globalContext,10,10,false);
#elif defined(CAIRO_HAS_XLIB_SURFACE)
          globalSurface=cairo_xlib_surface_create(dynamic_cast< OS::X11::Display*>(OS::display)->display,
                                                  dynamic_cast< OS::X11::Display*>(OS::display)->appWindow,
                                                  dynamic_cast< OS::X11::Display*>(OS::display)->visual,
                                                  10,10);
#endif
        }

        return globalSurface;
      }

      void FreeGlobalSurface()
      {
        if (globalSurface!=NULL) {
          cairo_surface_destroy(globalSurface);
          globalSurface=NULL;
        }
      }

#if defined(CAIRO_HAS_QUARTZ_SURFACE)
      CairoQuartzDriver::CairoQuartzDriver()
      : Base::Driver(L"CairoQuartz",10)
      {
        // no code
      }

      OS::Display* CairoQuartzDriver::CreateDisplay() const
      {
        return new OS::Carbon::Display();
      }

      OS::Window* CairoQuartzDriver::CreateWindow() const
      {
        return new OS::Carbon::Window(display);
      }

      OS::Bitmap* CairoQuartzDriver::CreateBitmap(size_t width, size_t height) const
      {
        assert(width>0);
        assert(height>0);

        return new Bitmap(width,height);
      }

      OS::DrawInfo* CairoQuartzDriver::CreateDrawInfo(OS::Window* window)
      {
        return new DrawInfo(bitmap);
      }

      OS::DrawInfo* CairoQuartzDriver::CreateDrawInfo(OS::Bitmap* bitmap)
      {
        return new DrawInfo(bitmap);
      }

      OS::Font* CairoQuartzDriver::CreateFont() const
      {
        return new Font();
      }

      OS::Tray* CairoQuartzDriver::CreateTray() const
      {
        return NULL;
      }

      Lum::Images::Factory* CairoQuartzDriver::CreateImageFactory(OS::Display* display) const
      {
        return new ImageFactory(display);
      }

      CairoQuartzDriver cairoQuartzDriver;
#endif

#if defined(CAIRO_HAS_WIN32_SURFACE)
      CairoWin32Driver::CairoWin32Driver()
      : Base::Driver(L"CairoWin32",10)
      {
        // no code
      }

      OS::Display* CairoWin32Driver::CreateDisplay() const
      {
        return new OS::Win32::Display();
      }

      OS::Window* CairoWin32Driver::CreateWindow() const
      {
        return new OS::Win32::Window(display);
      }

      OS::Bitmap* CairoWin32Driver::CreateBitmap(size_t width, size_t height) const
      {
        assert(width>0 && height>0);

        return new Bitmap(width,height);
      }

      OS::DrawInfo* CairoWin32Driver::CreateDrawInfo(OS::Window* window)
      {
        return new Win32DrawInfo(window);
      }

      OS::DrawInfo* CairoWin32Driver::CreateDrawInfo(OS::Bitmap* bitmap)
      {
        return new Win32DrawInfo(bitmap);
      }

      OS::Font* CairoWin32Driver::CreateFont() const
      {
        return new Font();
      }

      OS::Tray* CairoWin32Driver::CreateTray() const
      {
        return NULL;
      }

      Lum::Images::Factory* CairoWin32Driver::CreateImageFactory(OS::Display* display) const
      {
        return new ImageFactory(display);
      }

      CairoWin32Driver cairoWin32Driver;
#endif

#if defined(CAIRO_HAS_XLIB_SURFACE)
      CairoX11Driver::CairoX11Driver()
      : Base::Driver(L"CairoX11",10)
      {
        // no code
      }

      OS::Display* CairoX11Driver::CreateDisplay() const
      {
        return new OS::X11::Display();
      }

      OS::Window* CairoX11Driver::CreateWindow() const
      {
        return new OS::X11::Window(display);
      }

      OS::Bitmap* CairoX11Driver::CreateBitmap(size_t width, size_t height) const
      {
        assert(width>0);
        assert(height>0);

        return new Bitmap(width,height);
      }

      OS::DrawInfo* CairoX11Driver::CreateDrawInfo(OS::Window* window)
      {
        return new X11DrawInfo(window);
      }

      OS::DrawInfo* CairoX11Driver::CreateDrawInfo(OS::Bitmap* bitmap)
      {
        return new X11DrawInfo(bitmap);
      }

      OS::Font* CairoX11Driver::CreateFont() const
      {
        return new Font();
      }

      OS::Tray* CairoX11Driver::CreateTray() const
      {
        return NULL;
      }

      Lum::Images::Factory* CairoX11Driver::CreateImageFactory(OS::Display* display) const
      {
        return new ImageFactory(display);
      }

      CairoX11Driver cairoX11Driver;
#endif
    }
  }
}
