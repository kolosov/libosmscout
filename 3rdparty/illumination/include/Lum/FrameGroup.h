#ifndef LUM_FRAMEGROUP_H
#define LUM_FRAMEGROUP_H

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
  /**
    A layout object that draws a frame (with some optional
    space) around its only child. Usefull for simple visual
    grouping.
  */
  class LUMAPI FrameGroup : public Group
  {
  private:
    OS::FrameRef frame;
    Object*      object;
    Object*      text;
    size_t       horizSpace;
    size_t       vertSpace;
    bool         space;

  public:
    FrameGroup();
    ~FrameGroup();

    bool VisitChildren(Visitor &visitor, bool onlyVisible);

    void SetFrame(OS::Frame* frame);
    void SetObject(Object* object);
    void SetSpace(bool space);
    void SetLabel(Object* label);
    void SetLabel(const std::wstring& label);

    void CalcSize();
    void Layout();
    void Draw(OS::DrawInfo* draw,
              int x, int y, size_t w, size_t h);
  };
}

#endif
