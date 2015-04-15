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

#include <Lum/FuelGauge.h>

#include <cmath>

#include <Lum/Base/String.h>

#include <Lum/OS/Theme.h>

namespace Lum {

  FuelGauge::FuelGauge()
  : font(OS::display->GetFont())
  {
    SetBackground(OS::display->GetFill(OS::Display::hprogressBackgroundFillIndex));
  }

  bool FuelGauge::SetModel(Base::Model* model)
  {
    this->model=dynamic_cast<Model::Number*>(model);

    Control::SetModel(this->model);

    return this->model.Valid();
  }

  void FuelGauge::CalcSize()
  {
    minWidth=2*OS::display->GetSpaceHorizontal(OS::Display::spaceObjectBorder)+
             3*font->StringWidth(L"m",OS::Font::normal);
    minHeight=font->pixelHeight+2*OS::display->GetSpaceVertical(OS::Display::spaceObjectBorder);

    minWidth=std::max(minWidth,OS::display->GetTheme()->GetVerticalFuelBarMinWidth());
    minHeight=std::max(minHeight,OS::display->GetTheme()->GetHorizontalFuelBarMinHeight());

    width=minWidth;
    height=minHeight;
    SetBackground(OS::display->GetFill(OS::Display::hprogressBackgroundFillIndex));

    Control::CalcSize();
  }

  void FuelGauge::Layout()
  {
    if (width>=height) {
      SetBackground(OS::display->GetFill(OS::Display::hprogressBackgroundFillIndex));
    }
    else {
      SetBackground(OS::display->GetFill(OS::Display::vprogressBackgroundFillIndex));
    }

    Control::Layout();
  }

  void FuelGauge::Draw(OS::DrawInfo* draw,
                       int x, int y, size_t w, size_t h)
  {
    Control::Draw(draw,x,y,w,h);

    if (!OIntersect(x,y,w,h)) {
      return;
    }

    /* --- */

    if (!model.Valid() || model->IsNull()) {
      return;
    }

    double percent=(model->GetDouble()-model->GetMinAsDouble())/
                   (model->GetMaxAsDouble()-model->GetMinAsDouble());

    if (width>=height) {
      Lum::OS::FontExtent extent;
      std::wstring        label(Base::NumberToWString(lround(percent*100))+L"%");
      OS::FillRef         fill=OS::display->GetFill(OS::Display::hprogressFillIndex);

      fill->Draw(draw,
                 this->x,this->y,
                 width,height,
                 this->x,this->y,
                 (size_t)(width*percent),height);


      font->StringExtent(label,extent);

      draw->PushForeground(OS::Display::textColor);
      draw->PushFont(font,0);
      draw->DrawString(this->x+(width-extent.width+extent.left+extent.right)/2,
                       this->y+(height-font->height)/2+font->ascent,
                       label);
      draw->PopFont();
      draw->PopForeground();

    }
    else {
      OS::FillRef fill=OS::display->GetFill(OS::Display::vprogressFillIndex);

      fill->Draw(draw,
                 this->x,this->y,
                 width,height,
                 this->x,this->y+height-(size_t)(height*percent),
                 width,(size_t)(height*percent));
    }
  }

  void FuelGauge::Resync(Base::Model* model, const Base::ResyncMsg& msg)
  {
    if (model==this->model) {
      Redraw();
    }
    Control::Resync(model,msg);
  }

  FuelGauge* FuelGauge::Create(bool horizontalFlex, bool verticalFlex)
  {
    FuelGauge *f;

    f=new FuelGauge();
    f->SetFlex(horizontalFlex,verticalFlex);

    return f;
  }

  FuelGauge* FuelGauge::Create(Base::Model* model, bool horizontalFlex, bool verticalFlex)
  {
    FuelGauge *f;

    f=new FuelGauge();
    f->SetFlex(horizontalFlex,verticalFlex);
    f->SetModel(model);

    return f;
  }
}
