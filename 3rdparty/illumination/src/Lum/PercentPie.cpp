/*
  This source is part of the Illumination library
  Copyright (C) 2008  Tim Teulings

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
#include <Lum/PercentPie.h>

#include <cmath>

namespace Lum {

  PercentPie::PercentPie()
  {
    // no code
  }

  bool PercentPie::SetModel(Base::Model* model)
  {
    this->model=dynamic_cast<Model::DoubleDataStream*>(model);

    Control::SetModel(this->model);

    return this->model.Valid();
  }

  void PercentPie::CalcSize()
  {
    width=Base::GetSize(Base::Size::stdCharHeight,5);
    height=width;

    minWidth=width;
    minHeight=height;

    Object::CalcSize();
  }

  void PercentPie::Draw(OS::DrawInfo* draw,
                        int x, int y, size_t w, size_t h)
  {
    Object::Draw(draw,x,y,w,h);

    if (!OIntersect(x,y,w,h)) {
      return;
    }

    /* --- */

    size_t radius=std::min(width,height);

    if (model.Valid() && !model->IsNull()) {
      double       sum;
      size_t       entries,count;
      int          last;

      sum=0;
      entries=0;
      for (size_t i=0; i<model->GetChannels(); i++) {
        sum+=model->Get(i);
        entries++;
      }

      if (sum<100.0) {
        entries++;
      }

      last=90*64;
      count=0;
      for (size_t i=0; i<model->GetChannels(); i++) {
        int angle=lround(-(model->Get(i)/sum)*360*64);

        draw->PushUniqueFill(count,entries);
        draw->FillArc(this->x+(int)(width-radius)/2,this->y+(int)(height-radius)/2,
                      (int)radius-1,(int)radius-1,
                      last,angle);
        draw->PopUniqueFill(count,entries);
        last+=angle;

        count++;
      }
    }
    draw->PushForeground(OS::Display::whiteColor);
    draw->DrawArc(this->x+(int)(width-radius)/2,this->y+(int)(height-radius)/2,
                  (int)radius-1,(int)radius-1,
                  45*64,180*64);
    draw->PopForeground();

    draw->PushForeground(OS::Display::blackColor);
    draw->DrawArc(this->x+(int)(width-radius)/2,this->y+(int)(height-radius)/2,
                  (int)radius-1,(int)radius-1,
                  225*64,180*64);
    draw->PopForeground();
  }

  void PercentPie::Resync(Base::Model* model, const Base::ResyncMsg& msg)
  {
    if (model==this->model) {
      if (visible) {
        Redraw();
      }
    }
    else {
      Control::Resync(model,msg);
    }
  }
}
