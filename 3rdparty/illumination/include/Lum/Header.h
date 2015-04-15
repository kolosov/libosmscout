#ifndef LUM_HEADER_H
#define LUM_HEADER_H

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

#include <Lum/Base/Size.h>

#include <Lum/Model/Adjustment.h>
#include <Lum/Model/Header.h>

#include <Lum/OS/Display.h>

#include <Lum/Object.h>

namespace Lum {

  /**
    A scrollable of table column labels. Only used internaly
    by the VisualOberon table implementation. Don't use it
    outside!
  */
  class LUMAPI Header : public Control
  {
  private:
    Model::HeaderRef     model;      /** The model we listen to */
    std::vector<Object*> objects;    /** Internal table of columns */
    Model::AdjustmentRef adjustment; /** The horizontal adjustment we listen to */
    int                  offset;     /** For grabbing the offset of our point in relation to the mouse */
    int                  grabStart;  /** The start of the column currently grabbed */
    size_t               grabColumn; /** The column that gets grabbed */

  public:
    Header();
    virtual ~Header();

    bool VisitChildren(Visitor &visitor, bool onlyVisible);

    void SetAdjustment(Model::Adjustment* adjustment);

    void CalcSize();

    bool SetModel(Base::Model* model);

    void Layout();
    void PreDrawChilds(OS::DrawInfo* draw);
    void PostDrawChilds(OS::DrawInfo* draw);
    void Draw(OS::DrawInfo* draw,
              int x, int y, size_t w, size_t h);

    bool HandleMouseEvent(const OS::MouseEvent& event);

    void Resync(Base::Model* model, const Base::ResyncMsg& msg);
  };
}

#endif
