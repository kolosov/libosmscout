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

#include <Lum/OS/Fill.h>

#include <Lum/OS/DrawInfo.h>

namespace Lum {
  namespace OS {

    Fill::Fill()
    : topBorder(0),
      bottomBorder(0),
      leftBorder(0),
      rightBorder(0),
      transparentBorder(false)
    {
      // no code
    }

    Fill::~Fill()
    {
      // no code
    }

    bool Fill::HasBorder() const
    {
      return topBorder>0 || bottomBorder>0 || leftBorder>0 || rightBorder>0;
    }

    PlainFill::PlainFill(Color color)
     : color(color)
    {
      // no code
    }

    /**
      Fill the rectangle at \p x, \p y with the dimension \p w and \p h. Befor
      resize the underlying pattern to \p width and \p height and use \p xOff
      yOff as offsets into the pattern space.
    */
    void PlainFill::Draw(OS::DrawInfo* draw,
                         int /*xOff*/, int /*yOff*/, size_t /*width*/, size_t /*height */,
                         int x, int y, size_t w, size_t h)
    {
      draw->PushForeground(color);
      draw->FillRectangle(x,y,w,h);
      draw->PopForeground();
    }

    /**
      Does draw anything.
    */
    void EmptyFill::Draw(OS::DrawInfo* /*draw*/,
                         int /*xOff*/, int /*yOff*/, size_t /*width*/, size_t /*height */,
                         int /*x*/, int /*y*/, size_t /*w*/, size_t /*h*/)
    {
      // no code
    }
  }
}


