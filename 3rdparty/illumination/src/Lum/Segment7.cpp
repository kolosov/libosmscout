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

#include <Lum/Segment7.h>

namespace Lum {
  static int d[12]={
    1+2+4+8+16+32,
      2+4,
    1+2  +8+16   +64,
    1+2+4+8      +64,
      2+4     +32+64,
    1  +4+8   +32+64,
    1  +4+8+16+32+64,
    1+2+4,
    1+2+4+8+16+32+64,
    1+2+4+8   +32+64,
                  64,
    0};

  static int kx[7][6]={
    {1,2,8,9,8,2},
    {9,10,10,9,8,8},
    {9,10,10,9,8,8},
    {1,2,8,9,8,2},
    {1,2,2,1,0,0},
    {1,2,2,1,0,0},
    {1,2,8,9,8,2}
  };

  static int ky[7][6]={
    {1,0,0,1,2,2},
    {1,2,8,9,8,2},
    {9,10,16,17,16,10},
    {17,16,16,17,18,18},
    {9,10,16,17,16,0},
    {1,2,8,9,9,2},
    {9,8,8,9,10,10}
  };

  Segment7::Segment7()
  : onColor(OS::display->GetColor(OS::Display::blackColor)),
    offColor(OS::display->GetColor(OS::Display::backgroundColor)),
    value(line)
  {
    // no code
  }

  void Segment7::SetOnColor(OS::Color color)
  {
    onColor=color;

    if (visible) {
      Redraw();
    }
  }

  void Segment7::SetOffColor(OS::Color color)
  {
    offColor=color;

    if (visible) {
      Redraw();
    }
  }

  void Segment7::SetValue(Value value)
  {
    this->value=value;

    if (visible) {
      Redraw();
    }
  }

  void Segment7::CalcSize()
  {
    width=10;
    height=18;

    minWidth=width;
    minHeight=height;

    Object::CalcSize();
  }

  void Segment7::Draw(OS::DrawInfo* draw,
                      int x, int y, size_t w, size_t h)
  {
    Object::Draw(draw,x,y,w,h);

    if (!OIntersect(x,y,w,h)) {
      return;
    }

    /* --- */

    int                 dx,dy,xp,yp;
    OS::DrawInfo::Point points[7];

    if (value<images) {
      dx=width / 10;
      dy=height / 18;

      xp=this->x+(width-dx*10) / 2;
      yp=this->y+(height-dy*18) / 2;

      for (size_t i=0; i<=6; i++) {
        if (((1 << i) & d[value])!=0) {
          draw->PushForeground(onColor);
        }
        else {
          draw->PushForeground(offColor);
        }
        points[0].x=xp+dx*kx[i][0];
        points[0].y=yp+dy*ky[i][0];

        for (size_t j=1; j<=5; j++) {
          points[j].x=xp+dx*kx[i][j];
          points[j].y=yp+dy*ky[i][j];
        }
        points[6].x=xp+dx*kx[i][0];
        points[6].y=yp+dy*ky[i][0];
        draw->FillPolygon(points,7);
        draw->PopForeground();
      }
    }
    else {
      switch (value) {
      case doublePoint:
        draw->PushForeground(onColor);
        draw->FillRectangle(this->x+width / 3,this->y+height / 5,
                            width / 3, height / 5);
        draw->FillRectangle(this->x+width / 3,this->y+height-1-2*(height / 5),
                           width / 3, height / 5);
        draw->PopForeground();
        break;
      default:
        break;
      }
    }
  }
}
