/*
  This source is part of the Illumination library
  Copyright (C) 2007 Tim Teulings

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

#include <Lum/Toggle.h>

#include <Lum/OS/Display.h>

#include <Lum/Text.h>

namespace Lum {

  Toggle::Toggle()
   : label(NULL)
  {
    SetBackground(OS::display->GetFill(OS::Display::buttonBackgroundFillIndex));
  }

  Toggle::~Toggle()
  {
    delete label;
  }

  void Toggle::SetLabel(Object *label)
  {
    assert(label!=NULL);

    delete this->label;

    this->label=label;
    this->label->SetParent(this);
  }

  void Toggle::SetLabel(const std::wstring& label)
  {
    Text *text;

    text=new Text(label);
    text->SetParent(this);
    text->SetFlex(true,true);
    text->SetAlignment(Text::centered);

    SetLabel(text);
  }

  void Toggle::CalcSize()
  {
    minWidth=0;
    minHeight=0;
    width=0;
    height=0;

    if (label!=NULL) {
      label->CalcSize();

      minWidth=label->GetOMinWidth();
      minHeight=label->GetOMinHeight();

      width=label->GetOWidth();
      height=label->GetOHeight();
    }

    if (OS::display->GetTheme()->RequestFingerFriendlyControls()) {
      SetMinWidth(OS::display->GetTheme()->GetFingerFriendlyMinWidth());
      SetMinHeight(OS::display->GetTheme()->GetFingerFriendlyMinHeight());
    }

    State::CalcSize();
  }

  void Toggle::Layout()
  {
    if (label!=NULL) {
      label->Resize(width,height);

      label->Move(x+(width-label->GetOWidth())/2,
                  y+(height-label->GetOHeight())/2);
    }

    State::Layout();
  }

  void Toggle::DrawContent(OS::DrawInfo* draw,
                           int x, int y, size_t w, size_t h)
  {
    if (label!=NULL) {
      label->Draw(draw,x,y,w,h);
    }
  }

  Toggle* Toggle::Create(const std::wstring& label,
                         Base::Model* model,
                         bool horizontalFlex, bool verticalFlex)
  {
    Toggle *t=new Toggle();

    t->SetFlex(horizontalFlex,verticalFlex);
    t->SetModel(model);
    t->SetLabel(label);

    return t;
  }

  Toggle* Toggle::Create(Object* label,
                         Base::Model* model,
                         bool horizontalFlex, bool verticalFlex)
  {
    Toggle *t=new Toggle();

    t->SetFlex(horizontalFlex,verticalFlex);
    t->SetModel(model);
    t->SetLabel(label);

    return t;
  }

  Toggle* Toggle::Create(const std::wstring& label,
                         Base::Model* model,
                         unsigned long index,
                         bool horizontalFlex, bool verticalFlex)
  {
    Toggle *t=new Toggle();

    t->SetFlex(horizontalFlex,verticalFlex);
    t->SetModel(model);
    t->SetIndex(index);
    t->SetLabel(label);

    return t;
  }
}




