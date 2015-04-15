#ifndef LUM_OS_BASE_BITMAP_H
#define LUM_OS_BASE_BITMAP_H

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

#include <Lum/OS/Bitmap.h>

namespace Lum {
  namespace OS {
    namespace Base {

      class Bitmap : public ::Lum::OS::Bitmap
      {
      protected:
        size_t   width;
        size_t   height;
        DrawInfo *draw;

      public:
        Bitmap(size_t width, size_t height);

        size_t GetWidth() const;
        size_t GetHeight() const;
        OS::DrawInfo* GetDrawInfo() const;
      };
    }
  }
}

#endif
