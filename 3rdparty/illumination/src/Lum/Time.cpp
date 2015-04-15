/*
  This source is part of the Illumination library
  Copyright (C) 2005  Tim Teulings

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

#include <Lum/Time.h>

#include <cassert>

#include <Lum/Base/L10N.h>

#include <Lum/Label.h>
#include <Lum/Slider.h>
#include <Lum/TimeView.h>

namespace Lum {

  Time::Time()
  : format(Base::timeFormatHM),
    hour(new Model::ULong(0,23)),
    minute(new Model::ULong(0,59)),
    second(new Model::ULong(0,59))
  {
    Observe(hour);
    Observe(minute);
    Observe(second);
  }

  Time::~Time()
  {
    // no code
  }

  void Time::SetFormat(Base::TimeFormat format)
  {
    assert(!inited);

    this->format=format;
  }

  bool Time::SetModel(Base::Model* model)
  {
    this->model=dynamic_cast<Model::Time*>(model);

    ControlComponent::SetModel(this->model);

    return this->model.Valid();
  }

  void Time::CalcSize()
  {
    if (container==NULL) {
      Label    *label;
      Slider   *slider;
      TimeView *view;

      label=new Label();

      view=new Lum::TimeView();
      view->SetFormat(format);
      view->SetModel(this->model);
      label->AddLabel(_l(L"TIME_VALUE",L"Value:"),view);

      slider=new HSlider();
      slider->SetFlex(true,false);
      slider->SetScale(false);
      slider->SetModel(hour);
      label->AddLabel(_l(L"TIME_HOUR",L"Hour:"),slider);

      slider=new HSlider();
      slider->SetFlex(true,false);
      slider->SetScale(false);
      slider->SetModel(minute);
      label->AddLabel(_l(L"TIME_MINUTE",L"Minute:"),slider);

      if (format==Base::timeFormatHMS) {
        slider=new HSlider();
        slider->SetFlex(true,false);
        slider->SetScale(false);
        slider->SetModel(second);
        label->AddLabel(_l(L"TIME_SECOND",L"Second:"),slider);
      }

      container=label;
    }

    ControlComponent::CalcSize();
  }

  void Time::Resync(Base::Model* model, const Base::ResyncMsg& msg)
  {
    if (model==this->model) {
      hour->Set(this->model->Get().GetHour());
      minute->Set(this->model->Get().GetMinute());
      second->Set(this->model->Get().GetSecond());
    }
    else if (model==hour && this->model.Valid()) {
      Base::Time time=this->model->Get();

      time.SetHour((unsigned char)hour->GetUnsignedLong());
      this->model->Set(time);
    }
    else if (model==minute && this->model.Valid()) {
      Base::Time time=this->model->Get();

      time.SetMinute((unsigned char)minute->GetUnsignedLong());
      this->model->Set(time);
    }
    else if (model==second && this->model.Valid()) {
      Base::Time time=this->model->Get();

      time.SetSecond((unsigned char)second->GetUnsignedLong());
      this->model->Set(time);
    }

    Control::Resync(model,msg);
  }
}

