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

#include <Lum/OS/Cairo/Bitmap.h>

#include <Lum/OS/Cairo/Driver.h>

#if defined(__WIN32__) || defined(WIN32)
  #include <cairo-win32.h>
#endif 

namespace Lum {
  namespace OS {
    namespace Cairo {

      Bitmap::Bitmap(size_t width, size_t height)
      : OS::Base::Bitmap(width,height)
      {
        assert(width>0 && height>0);

        surface=cairo_surface_create_similar(GetGlobalSurface(),CAIRO_CONTENT_COLOR,width,height);

#if defined(CAIRO_HAS_XLIB_SURFACE)
        display=cairo_xlib_surface_get_display(surface);
        drawable=cairo_xlib_surface_get_drawable(surface);
#elif defined(CAIRO_HAS_WIN32_SURFACE)
        dc=cairo_win32_surface_get_dc(surface);
#endif

        draw=driver->CreateDrawInfo(this);
      }

      Bitmap::~Bitmap()
      {
        if (draw!=NULL) {
          delete draw;
        }

        cairo_surface_destroy(surface);
      }
    }
  }
}

