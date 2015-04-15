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

#include <Lum/Scroller.h>

#include <Lum/OS/Theme.h>

#include <Lum/Button.h>
#include <Lum/Image.h>

namespace Lum {

  Scroller::Scroller(bool vert)
  : panel(NULL),knob(new Knob),vert(vert)
  {
    SetCanFocus(true);

    knob->SetFlex(true,true);
    knob->Set(vert);

    if (vert) {
      SetBackground(OS::display->GetFill(OS::Display::vscrollBackgroundFillIndex));
    }
    else {
      SetBackground(OS::display->GetFill(OS::Display::hscrollBackgroundFillIndex));
    }
  }

  Scroller::~Scroller()
  {
    delete panel;
  }

  bool Scroller::VisitChildren(Visitor &visitor, bool /*onlyVisible*/)
  {
    return visitor.Visit(panel);
  }

  void Scroller::SetOffset(int offset)
  {
    knob->SetOffset(offset);
  }

  bool Scroller::SetModel(Base::Model* model)
  {
    bool res;

    res=knob->SetModel(model);
    Control::SetModel(knob->GetModel());

    return res;
  }

  Button* Scroller::CreateButton(bool vert, bool dec)
  {
    Button *button;
    Image  *image;

    image=new Image();
    image->SetFlex(true,true);
    if (dec) {
      if (vert) {
        image->SetImage(OS::display->GetImage(OS::Display::scrollUpImageIndex));
      }
      else {
        image->SetImage(OS::display->GetImage(OS::Display::scrollLeftImageIndex));
      }
    }
    else {
      if (vert) {
        image->SetImage(OS::display->GetImage(OS::Display::scrollDownImageIndex));
      }
      else {
        image->SetImage(OS::display->GetImage(OS::Display::scrollRightImageIndex));
      }
    }

    button=Button::Create(image,dec ? knob->GetDecAction() : knob->GetIncAction());
    button->SetType(Button::typeScroll);
    if (vert) {
      button->SetFlex(true,false);
    }
    else {
      button->SetFlex(false,true);
    }
    button->SetPulse(true);

    return button;
  }

  void Scroller::CalcSize()
  {
    if (panel==NULL) {
      if (vert) {
        panel=new VPanel();
        panel->SetFlex(false,true);
      }
      else {
        panel=new HPanel();
        panel->SetFlex(true,false);
      }
      panel->SetParent(this);

      switch (OS::display->GetTheme()->GetScrollKnobMode()) {
      case OS::Theme::scrollKnobModeSingle:
        panel->Add(CreateButton(vert,true));
        break;
      case OS::Theme::scrollKnobModeDouble:
        panel->Add(CreateButton(vert,true));
        panel->Add(CreateButton(vert,false));
        break;
      case OS::Theme::scrollKnobModeNone:
        break;
      }

      panel->Add(knob);

      switch (OS::display->GetTheme()->GetScrollKnobMode()) {
      case OS::Theme::scrollKnobModeSingle:
        panel->Add(CreateButton(vert,false));
        break;
      case OS::Theme::scrollKnobModeDouble:
        panel->Add(CreateButton(vert,true));
        panel->Add(CreateButton(vert,false));
        break;
      case OS::Theme::scrollKnobModeNone:
        break;
      }
    }

    panel->CalcSize();

    minWidth=panel->GetOMinWidth();
    minHeight=panel->GetOMinHeight();

    width=panel->GetOWidth();
    height=panel->GetOHeight();

    Control::CalcSize();
  }

  void Scroller::Layout()
  {
    panel->MoveResize(x,y,width,height);

    Control::Layout();
  }

  void Scroller::Draw(OS::DrawInfo* draw,
                      int x, int y, size_t w, size_t h)
  {
    Control::Draw(draw,x,y,w,h);

    if (!OIntersect(x,y,w,h)) {
      return;
    }

    /* --- */

    panel->Draw(draw,x,y,w,h);
  }

  void Scroller::Hide()
  {
    if (visible) {
      panel->Hide();
      Control::Hide();
    }
  }
}
