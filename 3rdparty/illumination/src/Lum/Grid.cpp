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

#include <Lum/Grid.h>

#include <cassert>

#include <Lum/Base/Util.h>

namespace Lum {

  Grid::Grid()
  : hSpace(false),vSpace(false),
    equalWidth(false),equalHeight(false)
  {
    // no code
  }

  Grid::~Grid()
  {
    for (size_t r=0; r<data.size(); r++) {
      for (size_t c=0; c<data[r].size(); c++) {
        delete data[r][c];
      }
    }
  }

  bool Grid::VisitChildren(Visitor &visitor, bool /*onlyVisible*/)
  {
    for (size_t r=0; r<data.size(); r++) {
      for (size_t c=0; c<data[r].size(); c++) {
        if (data[r][c]!=NULL) {
          if (!visitor.Visit(data[r][c])) {
            return false;
          }
        }
      }
    }

    return true;
  }

  void Grid::SetSpace(bool horiz, bool vert)
  {
    hSpace=horiz;
    vSpace=vert;
  }

  void Grid::SetSize(size_t horiz, size_t vert)
  {
    if (vert>=data.size()) {
      data.resize(vert);
    }

    for (size_t y=0; y<data.size(); y++) {
      if (horiz>=data[y].size()) {
        data[y].resize(horiz);
      }
    }
  }

  void Grid::SetEqualDimensions(bool equalWidth, bool equalHeight)
  {
    this->equalWidth=equalWidth;
     this->equalHeight=equalHeight;
 }

  void Grid::SetObject(size_t x, size_t y, Object* object)
  {
    assert(!inited);

    SetSize(x+1,y+1);

    if (data[y][x]!=NULL) {
      delete data[y][x];
    }

    if (object!=NULL) {
      object->SetParent(this);
    }

    data[y][x]=object;
  }

  void Grid::CalcSize()
  {
    size_t eWidth=0;
    size_t eHeight=0;

    rowHeight.resize(data.size());
    for (size_t r=0; r<rowHeight.size(); r++) {
      rowHeight[r].minSize=0;
      rowHeight[r].size=0;
      rowHeight[r].maxSize=32000; // TODO
    }
    columnWidth.resize(data[0].size());
    for (size_t c=0; c<columnWidth.size(); c++) {
      columnWidth[c].minSize=0;
      columnWidth[c].size=0;
      columnWidth[c].maxSize=32000; // TODO
    }

    for (size_t r=0; r<data.size(); r++) {
      for (size_t c=0; c<data[r].size(); c++) {
        if (data[r][c]!=NULL) {
          data[r][c]->CalcSize();

          rowHeight[r].minSize=std::max(rowHeight[r].minSize,data[r][c]->GetOMinHeight());
          rowHeight[r].size=std::max(rowHeight[r].size,data[r][c]->GetOHeight());
          rowHeight[r].maxSize=std::min(rowHeight[r].maxSize,data[r][c]->GetOMaxHeight());

          columnWidth[c].minSize=std::max(columnWidth[c].minSize,data[r][c]->GetOMinWidth());
          columnWidth[c].size=std::max(columnWidth[c].size,data[r][c]->GetOWidth());
          columnWidth[c].maxSize=std::min(columnWidth[c].maxSize,data[r][c]->GetOMaxWidth());

          eWidth=std::max(eWidth,data[r][c]->GetOWidth());
          eHeight=std::max(eHeight,data[r][c]->GetOHeight());
        }
      }
    }

    if (equalWidth) {
      for (size_t c=0; c<columnWidth.size(); c++) {
        columnWidth[c].minSize=eWidth;
        columnWidth[c].size=eWidth;
      }
    }

    if (equalHeight) {
      for (size_t r=0; r<rowHeight.size(); r++) {
        rowHeight[r].minSize=eHeight;
        rowHeight[r].size=eHeight;
      }
    }

    minWidth=0;
    width=0;

    /*    g.maxWidth:=0; MAX(LONGINT)-4;*/
    for (size_t c=0; c<columnWidth.size(); c++) {
      minWidth+=columnWidth[c].minSize;
      width+=columnWidth[c].size;
      /*      INC(g.maxWidth,g.columnWidth[x].maxSize);*/
    }

    minHeight=0;
    height=0;
      /*    g.maxHeight:=0; MAX(LONGINT)-4;*/
    for (size_t r=0; r<rowHeight.size(); r++) {
      minHeight+=rowHeight[r].minSize;
      height+=rowHeight[r].size;
      /*      INC(g.maxHeight,g.rowHeight[y].maxSize);*/
    }

    if (hSpace && columnWidth.size()>0) {
      minWidth+=(columnWidth.size()-1)*OS::display->GetSpaceHorizontal(OS::Display::spaceInterObject);
      width+=(columnWidth.size()-1)*OS::display->GetSpaceHorizontal(OS::Display::spaceInterObject);
    }

    if (vSpace && rowHeight.size()>0) {
      minHeight+=(rowHeight.size()-1)*OS::display->GetSpaceVertical(OS::Display::spaceInterObject);
      height+=(rowHeight.size()-1)*OS::display->GetSpaceVertical(OS::Display::spaceInterObject);
    }

    Group::CalcSize();
  }

  void Grid::CalculatePos()
  {
    columnWidth[0].pos=0;
    for (size_t c=1; c<columnWidth.size(); c++) {
      columnWidth[c].pos=columnWidth[c-1].pos+columnWidth[c-1].size;

      if (hSpace) {
        columnWidth[c].pos+=OS::display->GetSpaceHorizontal(OS::Display::spaceInterObject);
      }
    }

    rowHeight[0].pos=0;
    for (size_t r=1; r<rowHeight.size(); r++) {
      rowHeight[r].pos=rowHeight[r-1].pos+rowHeight[r-1].size;

      if (vSpace) {
        rowHeight[r].pos+=OS::display->GetSpaceVertical(OS::Display::spaceInterObject);
      }
    }
  }

  void Grid::Layout()
  {
    size_t oldSize,newSize;

    // Width

    oldSize=0;
    for (size_t c=0; c<columnWidth.size(); c++) {
      oldSize+=columnWidth[c].size;
    }

    newSize=width;
    if (hSpace && columnWidth.size()>0) {
      newSize-=(columnWidth.size()-1)*OS::display->GetSpaceHorizontal(OS::Display::spaceInterObject);
    }

    /* resize columns */
    while (true) {
      size_t count=0;
      /* count number of resizable objects */
      for (size_t c=0; c<columnWidth.size(); c++) {
        if (((oldSize<newSize) && (columnWidth[c].size<columnWidth[c].maxSize)) ||
            ((newSize<oldSize) && (columnWidth[c].size>columnWidth[c].minSize))) {
          count++;
        }
      }

      /* we leave the loop if there are no objects (anymore) that can be resized */
      if (count==0) {
        break;
      }

      for (size_t c=0; c<columnWidth.size(); c++) {
        if (((oldSize<newSize) && (columnWidth[c].size<columnWidth[c].maxSize)) ||
            ((newSize<oldSize) && (columnWidth[c].size>columnWidth[c].minSize))) {
          size_t old;

          old=columnWidth[c].size;
          columnWidth[c].size+=Base::UpDiv(newSize-oldSize,count);
          columnWidth[c].size=Base::RoundRange(columnWidth[c].size,
                                               columnWidth[c].minSize,
                                               columnWidth[c].maxSize);
          oldSize+=columnWidth[c].size-old;
         count--;
        }
      }

      /* no space left to delegate */
      if (oldSize==newSize) {
        break;
      }
    }

    // Height

    oldSize=0;
    for (size_t r=0; r<rowHeight.size(); r++) {
      oldSize+=rowHeight[r].size;
    }

    newSize=height;
    if (vSpace && rowHeight.size()>0) {
      newSize-=(rowHeight.size()-1)*OS::display->GetSpaceVertical(OS::Display::spaceInterObject);
    }

    /* resize rows */
    while (true) {
      size_t count=0;
      /* count number of resizable objects */
      for (size_t r=0; r<rowHeight.size(); r++) {
        if (((oldSize<newSize) && (rowHeight[r].size<rowHeight[r].maxSize)) ||
            ((newSize<oldSize) && (rowHeight[r].size>rowHeight[r].minSize))) {
          count++;
        }
      }

      /* we leave the loop if there are no objects (anymore) that can be resized */
      if (count==0) {
        break;
      }

      for (size_t r=0; r<rowHeight.size(); r++) {
        if (((oldSize<newSize) && (rowHeight[r].size<rowHeight[r].maxSize)) ||
            ((newSize<oldSize) && (rowHeight[r].size>rowHeight[r].minSize))) {
          size_t old;

          old=rowHeight[r].size;
          rowHeight[r].size+=Base::UpDiv(newSize-oldSize,count);
          rowHeight[r].size=Base::RoundRange(rowHeight[r].size,
                                             rowHeight[r].minSize,
                                             rowHeight[r].maxSize);
          oldSize+=rowHeight[r].size-old;
          count--;
        }
      }

      /* no space left to delegate */
      if (oldSize==newSize) {
        break;
      }
    }

    CalculatePos();

    for (size_t r=0; r<data.size(); r++) {
      for (size_t c=0; c<data[r].size(); c++) {
        if (data[r][c]!=NULL) {
          Object *object;

          object=data[r][c];
          object->MoveResize(this->x+columnWidth[c].pos,this->y+rowHeight[r].pos,
                             columnWidth[c].size,rowHeight[r].size);
        }
      }
    }

    Group::Layout();
  }
}
