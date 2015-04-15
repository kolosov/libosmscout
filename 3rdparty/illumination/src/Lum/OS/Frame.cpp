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

#include <Lum/OS/Frame.h>

namespace Lum {
  namespace OS {

    Frame::Frame()
    : topBorder(0),bottomBorder(0),leftBorder(0),rightBorder(0),
      gx(0),gw(0),gh(0),
      minWidth(0),minHeight(0),alpha(false)
    {
      // no code
    }

    Frame::~Frame()
    {
      // no code
    }

    void Frame::SetGap(size_t x, size_t width, size_t height)
    {
      gx=x;
      gw=width;
      gh=height;

      topBorder=std::max(height,topBorder);
    }

    bool Frame::HasGap() const
    {
      return gw>0 || gh>0;
    }

    void EmptyFrame::Draw(OS::DrawInfo* draw, int x, int y, size_t w, size_t h)
    {
      // No code
    }
  }
}


