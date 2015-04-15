#ifndef LUM_ARRAY_H
#define LUM_ARRAY_H

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

#include <Lum/Base/Size.h>

#include <Lum/Object.h>

namespace Lum {
  /**
    A layout group that arranges all it children in an
    two-dimensional array.
  */
  class LUMAPI Array : public List
  {
  private:
    size_t sWidth,sHeight;
    size_t horiz,vert;
    bool   horizSpace;
    bool   vertSpace;

  public:
    Array();

    void SetHorizontalCount(size_t count);
    void SetVerticalCount(size_t count);
    void SetSpace(bool horiz, bool vert);

    Array* AddEmpty();

    void CalcSize();
    void Layout();
  };
}

#endif
