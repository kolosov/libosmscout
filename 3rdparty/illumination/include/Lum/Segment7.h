#ifndef LUM_SEGMENT7_H
#define LUM_SEGMENT7_H

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

#include <Lum/Object.h>

namespace Lum {
  class LUMAPI Segment7 : public Object
  {
  public:
    enum Value {
      zero  =  0,
      one   =  1,
      two   =  2,
      three =  3,
      four  =  4,
      five  =  5,
      six   =  6,
      seven =  7,
      eight =  8,
      nine  =  9,
      line  = 10,
      none  = 11,

      doublePoint,
      images=doublePoint // start of images
    };

  private:
    OS::Color onColor;
    OS::Color offColor;
    Value     value;

  public:
    Segment7();

    void SetOnColor(OS::Color color);
    void SetOffColor(OS::Color color);

    void SetValue(Value value);

    void CalcSize();
    void Draw(OS::DrawInfo* draw,
              int x, int y, size_t w, size_t h);
  };
}

#endif
