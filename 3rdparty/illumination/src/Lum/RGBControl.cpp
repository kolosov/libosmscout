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

#include <Lum/RGBControl.h>

#include <Lum/Base/L10N.h>

#include <Lum/Label.h>
#include <Lum/Slider.h>

namespace Lum {

  RGBControl::RGBControl()
  : r(new Model::ULong(0,255)),
    g(new Model::ULong(0,255)),
    b(new Model::ULong(0,255))
  {
    Observe(r);
    Observe(g);
    Observe(b);
  }

  RGBControl::~RGBControl()
  {
    // no code
  }

  bool RGBControl::SetModel(Base::Model* model)
  {
    this->model=dynamic_cast<Model::Color*>(model);

    ControlComponent::SetModel(this->model);

    return this->model.Valid();
  }

  void RGBControl::CalcSize()
  {
    Label  *label;
    Slider *slider;

    label=new Label();

    slider=new HSlider();
    slider->SetFlex(true,false);
    slider->SetModel(r);
    label->AddLabel(_l(L"RGBCONTROL_RED",L"Red:"),slider);

    slider=new HSlider();
    slider->SetFlex(true,false);
    slider->SetModel(g);
    label->AddLabel(_l(L"RGBCONTROL_GREEN",L"Green:"),slider);

    slider=new HSlider();
    slider->SetFlex(true,false);
    slider->SetModel(b);
    label->AddLabel(_l(L"RGBCONTROL_BLUE",L"Blue:"),slider);

    container=label;

    ControlComponent::CalcSize();
  }

  void RGBControl::Resync(Base::Model* model, const Base::ResyncMsg& msg)
  {
    if (model==this->model) {
      r->Set((int)(this->model->Get().GetRed()*255.0+0.5));
      g->Set((int)(this->model->Get().GetGreen()*255.0+0.5));
      b->Set((int)(this->model->Get().GetBlue()*255.0+0.5));
    }
    else if (model==r && this->model.Valid()) {
      OS::ColorValue color=this->model->Get();

      color.SetRed(r->GetDouble()/255.0);
      this->model->Set(color);
    }
    else if (model==g && this->model.Valid()) {
      OS::ColorValue color=this->model->Get();

      color.SetGreen(g->GetDouble()/255.0);
      this->model->Set(color);
    }
    else if (model==b && this->model.Valid()) {
      OS::ColorValue color=this->model->Get();

      color.SetBlue(b->GetDouble()/255.0);
      this->model->Set(color);
    }

    ControlComponent::Resync(model,msg);
  }
}

