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

#include <Lum/OS/Image.h>

namespace Lum {
  namespace OS {

    Image::~Image()
    {
      // no code
    }

    size_t EmptyImage::GetWidth() const
    {
      return 0;
    }

    size_t EmptyImage::GetHeight() const
    {
      return 0;
    }

    bool EmptyImage::GetAlpha() const
    {
      return false;
    }

    unsigned long EmptyImage::GetDrawCap() const
    {
      return drawsFocused;
    }

    void EmptyImage::Draw(OS::DrawInfo *draw, int x, int y)
    {
      // no code
    }

    void EmptyImage::DrawScaled(OS::DrawInfo *draw,
                                int x, int y,
                                size_t w, size_t h)
    {
      // no code
    }

    void EmptyImage::DrawStretched(OS::DrawInfo *draw,
                                   int x, int y,
                                   size_t w, size_t h)
    {
      // no code
    }
  }
}

