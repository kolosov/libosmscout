#ifndef LUM_KNOB_H
#define LUM_KNOB_H

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

#include <Lum/Model/Action.h>
#include <Lum/Model/Adjustment.h>

#include <Lum/OS/Frame.h>
#include <Lum/OS/Image.h>

#include <Lum/Object.h>
#include <Lum/Image.h>

namespace Lum {

  /**
    Implements the knob part of a scrollbar. You can use this object
    directly if you know what you do (need such beast as part
    of a new control), but if you want to have a simple scrollbar,
    simple use @code{VO:Scroller}.

    It epects an instance of @otype{A.Adjustment} as an model.
  */
  class LUMAPI Knob : public Control
  {
  private:
    Image                *knob;
    int                  offset; //! offset of the mouse relative to the knob start during mouse selection
    int                  corr;   //! corrective offset to fix model values not starting with 0
    Model::AdjustmentRef adjustment;
    Model::ActionRef     incAction;
    Model::ActionRef     decAction;
    bool                 vert;
    bool                 selected;

  private:
    void HandleMouseMovement(int x, int y);

  public:
    Knob();
    ~Knob();

    void Set(bool vert);
    void SetOffset(int offset);

    bool SetModel(Base::Model* model);

    void CalcSize();

    bool HandleMouseEvent(const OS::MouseEvent& event);
    bool HandleKeyEvent(const OS::KeyEvent& event);

    void Draw(OS::DrawInfo* draw,
              int x, int y, size_t w, size_t h);
    void Hide();

    void Resync(Base::Model* model, const Base::ResyncMsg& msg);

    Model::Action* GetIncAction() const;
    Model::Action* GetDecAction() const;
  };
}

#endif

