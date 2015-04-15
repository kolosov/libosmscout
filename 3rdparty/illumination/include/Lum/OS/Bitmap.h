#ifndef LUM_OS_BITMAP_H
#define LUM_OS_BITMAP_H

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

#if !defined(__WIN32__) && !defined(WIN32)
  #include <unistd.h>
#endif

#include <Lum/Private/ImportExport.h>

#include <Lum/OS/DrawInfo.h>

namespace Lum {
  namespace OS {

    /**
      A of screen bitmap.
    */
    class LUMAPI Bitmap
    {
    public:
      Bitmap(size_t width, size_t height);
      virtual ~Bitmap();

      virtual size_t GetWidth() const = 0;
      virtual size_t GetHeight() const = 0;

      virtual DrawInfo* GetDrawInfo() const = 0;
    };
  }
}

#endif
