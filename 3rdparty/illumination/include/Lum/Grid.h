#ifndef LUM_GRID_H
#define LUM_GRID_H

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

#include <vector>

#include <Lum/Object.h>

namespace Lum {
  /**
    A grid is an layouting object that devides the available space
    into a table, where each cell in a row has the same height and
    cell in a column the same width. You can assign an object to a
    subset of cells.
  */
  class LUMAPI Grid : public Group
  {
  private:
    struct Info
    {
      size_t minSize;
      size_t size;
      size_t maxSize;
      size_t pos;
    };

  private:
    std::vector<std::vector<Object*> > data;

    std::vector<Info>                  rowHeight;
    std::vector<Info>                  columnWidth;

    bool                               hSpace,vSpace;
    bool                               equalWidth,equalHeight;

  private:
    void CalculatePos();

  public:
    Grid();
    ~Grid();

    bool VisitChildren(Visitor &visitor, bool onlyVisible);

    void SetSpace(bool horiz, bool vert);
    void SetSize(size_t horiz, size_t vert);
    void SetEqualDimensions(bool equalWidth, bool equalHeight);
    void SetObject(size_t x, size_t y, Object* object);

    void CalcSize();
    void Layout();
  };
}

#endif
