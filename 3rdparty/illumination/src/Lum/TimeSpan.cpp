/*
  This source is part of the Illumination library
  Copyright (C) 2009  Tim Teulings

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

#include <Lum/TimeSpan.h>

#include <Lum/Base/String.h>

#include <Lum/Grid.h>

namespace Lum {
  static int pow(int x, int y)
  {
    if (y==0) {
      return 1;
    }

    for (int i=2; i<=y; i++) {
      x=x*x;
    }

    return x;
  }

  static Button* GetButtonUp(Model::Action* up)
  {
    Button *button;

    button=Button::Create(L"+",up);
    if (OS::display->GetTheme()->RequestFingerFriendlyControls()) {
      button->SetMinWidth(OS::display->GetTheme()->GetFingerFriendlyMinWidth());
      button->SetMinHeight(OS::display->GetTheme()->GetFingerFriendlyMinHeight());
    }

    return button;
  }

  static Button* GetButtonDown(Model::Action* down)
  {
    Button *button;

    button=Button::Create(L"-",down);

    if (OS::display->GetTheme()->RequestFingerFriendlyControls()) {
      button->SetMinWidth(OS::display->GetTheme()->GetFingerFriendlyMinWidth());
      button->SetMinHeight(OS::display->GetTheme()->GetFingerFriendlyMinHeight());
    }

    return button;
  }

  static Text* GetText(const std::wstring& text)
  {
    Text        *t;
    OS::FontRef font=OS::display->GetFont(OS::Display::fontTypeFixed,
                                                    500);

    t=new Text(text,
                    OS::Font::normal,
                    Text::centered,
                    font);

    t->SetFlex(true,true);

    return t;
  }

  TimeSpan::TimeSpan()
  {
    OS::FontRef font=OS::display->GetFont(OS::Display::fontTypeFixed,
                                          500);

    for (size_t i=0; i<5; i++) {
      ups[i]=new Model::Action();
      downs[i]=new Model::Action();
      texts[i]=new Text(Base::NumberToWString(0),
                        OS::Font::normal,
                        Text::centered,
                        font);
      texts[i]->SetFlex(true,true);

      digits[i]=new Model::ULong();
    }

    for (size_t i=0; i<5; i++) {
      Observe(ups[i]);
      Observe(downs[i]);
      Observe(digits[i]);
    }
  }

  bool TimeSpan::SetModel(Base::Model* model)
  {
    this->model=dynamic_cast<Model::Number*>(model);

    ControlComponent::SetModel(this->model);

    return this->model.Valid();
  }

  void TimeSpan::CalcSize()
  {
    if (container==NULL) {
      Grid *grid;

      grid=new Grid();
      grid->SetFlex(true,true);
      grid->SetSpace(true,true);
      grid->SetSize(6,3);

      grid->SetObject(0,0,GetButtonUp(ups[0]));
      grid->SetObject(0,1,texts[0]);
      grid->SetObject(0,2,GetButtonDown(downs[0]));

      grid->SetObject(1,0,GetButtonUp(ups[1]));
      grid->SetObject(1,1,texts[1]);
      grid->SetObject(1,2,GetButtonDown(downs[1]));

      grid->SetObject(2,0,GetButtonUp(ups[2]));
      grid->SetObject(2,1,texts[2]);
      grid->SetObject(2,2,GetButtonDown(downs[2]));

      grid->SetObject(3,1,GetText(L":"));

      grid->SetObject(4,0,GetButtonUp(ups[3]));
      grid->SetObject(4,1,texts[3]);
      grid->SetObject(4,2,GetButtonDown(downs[3]));

      grid->SetObject(5,0,GetButtonUp(ups[4]));
      grid->SetObject(5,1,texts[4]);
      grid->SetObject(5,2,GetButtonDown(downs[4]));

      container=grid;
    }

    ControlComponent::CalcSize();
  }

  void TimeSpan::Resync(Base::Model *model, const Base::ResyncMsg& msg)
  {
    for (size_t i=0; i<5; i++) {
      if (model==ups[i] &&
          ups[i]->IsFinished() &&
          ups[i]->IsEnabled()) {
        if (i==3) {
          digits[i]->Set((digits[i]->GetUnsignedLong()+1)%6);
        }
        else {
          digits[i]->Set((digits[i]->GetUnsignedLong()+1)%10);
        }
      }
      else if (model==downs[i] &&
               downs[i]->IsFinished() &&
               downs[i]->IsEnabled()) {
        if (i==3) {
          digits[i]->Set((digits[i]->GetUnsignedLong()+5)%6);
        }
        else {
          digits[i]->Set((digits[i]->GetUnsignedLong()+9)%10);
        }
      }
      else if (model==digits[i]) {
        OS::FontRef font=OS::display->GetFont(OS::Display::fontTypeFixed,
                                              500);

        if (digits[i]->IsNull()) {
          ups[i]->Disable();
          downs[i]->Disable();
          texts[i]->SetText(L"-",
                            OS::Font::normal,
                            Text::centered,
                            font);
        }
        else {
          ups[i]->Enable();
          downs[i]->Enable();
          texts[i]->SetText(Base::NumberToWString(digits[i]->GetUnsignedLong()),
                            OS::Font::normal,
                            Text::centered,
                            font);
        }

        if (this->model.Valid() &&
            !this->model->IsNull() &&
            !digits[0]->IsNull() &&
            !digits[1]->IsNull() &&
            !digits[2]->IsNull() &&
            !digits[3]->IsNull() &&
            !digits[4]->IsNull()) {
          this->model->Set(digits[0]->GetUnsignedLong()*100*60+
                           digits[1]->GetUnsignedLong()*10*60+
                           digits[2]->GetUnsignedLong()*60+
                           digits[3]->GetUnsignedLong()*10+
                           digits[4]->GetUnsignedLong());
        }
      }
      else if (model==this->model) {
        if (this->model->IsNull()) {
          for (size_t i=0; i<5; i++) {
            digits[i]->SetNull();
          }
        }
        else {
          for (size_t i=0; i<5; i++) {
            size_t value=0;

            if (this->model.Valid()) {
              value=this->model->GetUnsignedLong();

              if (i<3) {
                value=(value/(pow(10,2-i)*60))%10;
              }
              else {
                value=((value%60)/pow(10,5-i-1))%10;
              }

              digits[i]->Set(value);
            }
          }
        }
      }
    }

    ControlComponent::Resync(model,msg);
  }

  TimeSpan* TimeSpan::Create(Model::Number* model,
                             bool horizontalFlex, bool verticalFlex)
  {
    TimeSpan *timeSpan= new TimeSpan();

    timeSpan->SetFlex(horizontalFlex,verticalFlex);
    timeSpan->SetModel(model);

    return timeSpan;
  }
}

