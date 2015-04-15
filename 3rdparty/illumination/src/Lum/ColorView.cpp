/*
  This source is part of the Illumination library
  Copyright (C) 2007  Tim Teulings

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

#include <Lum/ColorView.h>

namespace Lum {

  ColorView::ColorView()
  : colorAllocated(false)
  {
    SetBackground(OS::display->GetFill(OS::Display::plateBackgroundFillIndex));
  }

  ColorView::~ColorView()
  {
    if (colorAllocated) {
      OS::display->FreeColor(color);
    }
  }

  bool ColorView::SetModel(Base::Model* model)
  {
    this->model=dynamic_cast<Model::Color*>(model);

    Control::SetModel(this->model);

    return this->model.Valid();
  }

  void ColorView::CalcSize()
  {
    minWidth=OS::display->GetSpaceHorizontal(OS::Display::spaceObjectDetail);
    minHeight=OS::display->GetSpaceVertical(OS::Display::spaceObjectDetail);

    width=minWidth;
    height=minHeight;

    Control::CalcSize();
  }

  void ColorView::Draw(OS::DrawInfo* draw,
                       int x, int y, size_t w, size_t h)
  {
    Control::Draw(draw,x,y,w,h);

    if (!OIntersect(x,y,w,h)) {
      return;
    }

    /* --- */

    if (model.Valid() && !model->IsNull()) {
      OS::ColorValue value=model->Get();

      if (colorAllocated) {
        OS::display->FreeColor(color);
      }

      colorAllocated=OS::display->AllocateColor(value.GetRed(),value.GetGreen(),value.GetBlue(),
                                                color);

      if (colorAllocated) {
        draw->PushForeground(color);
        draw->FillRectangle(x,y,w,h);
        draw->PopForeground();
      }
    }
  }

  void ColorView::Resync(Base::Model* model, const Base::ResyncMsg& msg)
  {
    if (model==this->model) {
      Redraw();
    }
    else {
      Control::Resync(model,msg);
    }
  }
}
