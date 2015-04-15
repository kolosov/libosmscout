#ifndef LUM_OS_CAIRO_BITMAP_H
#define LUM_OS_CAIRO_BITMAP_H

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

#include <Lum/Features.h>

// Cairo
#if defined(__WIN32__) || defined(WIN32) || defined(__APPLE__)
  #include <cairo.h>
#else
  #include <cairo/cairo.h>
#endif

#if defined(LUM_HAVE_LIB_X)
  #include <cairo-xlib.h>
#endif

#if defined(LUM_HAVE_LIB_WIN32)
  #include <cairo-win32.h>
#endif

#include <Lum/OS/Base/Bitmap.h>

namespace Lum {
  namespace OS {
    namespace Cairo {

      class LUMAPI Bitmap : public OS::Base::Bitmap
      {
      public:
        ::cairo_surface_t *surface;

#if defined(CAIRO_HAS_XLIB_SURFACE)
        ::Display          *display;
        ::Window           drawable;
#elif defined(CAIRO_HAS_WIN32_SURFACE)
        HDC                dc;
#endif
      public:
        Bitmap(size_t width, size_t height);
        virtual ~Bitmap();
      };
    }
  }
}

#endif
