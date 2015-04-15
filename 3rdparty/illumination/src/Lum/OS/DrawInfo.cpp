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

#include <Lum/OS/DrawInfo.h>

#include <Lum/OS/Display.h>
#include <Lum/OS/Theme.h>

namespace Lum {
  namespace OS {

    DrawInfo::DrawInfo(Lum::OS::Window* window)
    : selected(false),activated(false),disabled(false),focused(false)
    {
      // no code
    }
    DrawInfo::DrawInfo(Lum::OS::Bitmap* bitmap)
    : selected(false),activated(false),disabled(false),focused(false)
    {
      // no code
    }

    DrawInfo::~DrawInfo()
    {
      // no code
    }

    /**
      This methods tries to create a unique fill pattern on the stack.
      Illumination tries to select the optimal fill pattern for the given
      color mode. Illumination cannot offer an unlimited number of different
      fill patterns. After a not specified amount of patterns (at least three)
      Illumination will reuse the patterns. E.g. when Illumination offers three
      patterns the fourth one will be equal to the first one. Illumination will
      also garantee that the the last pattern will not be equal to the first
      pattern. Garanteeing this, you need not take special care when filling
      f.e. a pie chart.

      PARAMETER
      - pos is the running number of the pattern.
      - maximum is the maximum number of patterns you want.
      Only when this value is correctly set Illumination will garantee the above
      fact.
      If you don't want Illumination to take special care just hand -1.
    */
    void DrawInfo::PushUniqueFill(size_t index, size_t maximum)
    {
      display->GetTheme()->PushUniqueFill(this,index,maximum);
    }

    /**
      Pop the pushed pattern from the stack.
    */
    void DrawInfo::PopUniqueFill(size_t index, size_t maximum)
    {
      display->GetTheme()->PopUniqueFill(this,index,maximum);
    }

    /**
     Draws a point at the given position in the given color.
    */
    void DrawInfo::DrawPointWithColor(int x, int y, Color color)
    {
      PushForeground(color);
      DrawPoint(x,y);
      PopForeground();
    }

    /**
      Fill the given rectangle with the background color defined in Display.
      This is a high level function. You should use it whenever you want to clear
      a area and give it the background color.

      The base class implements this method by pushing the backgroundColor,
      drawing a rectangle using DrawInfo.FillRectangle and then poping
      the color back.
    */
    void DrawInfo::FillBackground(int x, int y, int width, int height)
    {
      PushForeground(Display::backgroundColor);
      FillRectangle(x,y,width,height);
      PopForeground();
    }
  }
}
