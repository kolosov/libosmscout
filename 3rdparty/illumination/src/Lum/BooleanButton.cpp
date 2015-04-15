/*
  This source is part of the Illumination library
  Copyright (C) 2009 Tim Teulings

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

#include <Lum/BooleanButton.h>

#include <Lum/OS/Display.h>

#include <Lum/Text.h>

namespace Lum {

  BooleanButton::BooleanButton()
   : image(OS::display->GetImage(OS::Display::checkImageIndex)),
     label(NULL)
  {
    assert(image.Valid());

    SetBackground(OS::display->GetFill(OS::Display::buttonBackgroundFillIndex));
  }

  BooleanButton::~BooleanButton()
  {
    delete label;
  }

  bool BooleanButton::HasImplicitLabel() const
  {
    return true;
  }

  void BooleanButton::SetLabel(Object *label)
  {
    assert(label!=NULL);

    delete this->label;

    this->label=label;
    this->label->SetParent(this);
  }

  void BooleanButton::SetLabel(const std::wstring& label)
  {
    Text *text;

    text=new Text(label);
    text->SetParent(this);
    text->SetFlex(true,true);
    text->SetAlignment(Text::centered);

    SetLabel(text);
  }

  void BooleanButton::CalcSize()
  {
    minWidth=0;
    minHeight=0;
    width=0;
    height=0;

    width=image->GetWidth();
    height=image->GetHeight();

    if (label!=NULL) {
      minWidth+=OS::display->GetSpaceHorizontal(OS::Display::spaceIntraObject);
      width+=OS::display->GetSpaceHorizontal(OS::Display::spaceIntraObject);

      label->CalcSize();
      minWidth+=label->GetOMinWidth();
      width+=label->GetOWidth();

      minHeight=std::max(minHeight,label->GetOMinHeight());
      height=std::max(height,label->GetOHeight());
    }

    if (OS::display->GetTheme()->RequestFingerFriendlyControls()) {
      SetMinWidth(OS::display->GetTheme()->GetFingerFriendlyMinWidth());
      SetMinHeight(OS::display->GetTheme()->GetFingerFriendlyMinHeight());
    }

    State::CalcSize();
  }

  void BooleanButton::Layout()
  {
    if (label!=NULL) {
      label->Resize(width-
                    OS::display->GetSpaceHorizontal(OS::Display::spaceIntraObject)-
                    image->GetWidth(),
                    height);

      label->Move(this->x+
                  image->GetWidth()+
                  OS::display->GetSpaceHorizontal(OS::Display::spaceIntraObject),
                  y+(height-label->GetOHeight())/2);
    }

    State::Layout();
  }

  void BooleanButton::PrepareForBackground(OS::DrawInfo* draw)
  {
    if (!boolModel.Valid() ||
        boolModel->IsNull() ||
        !boolModel->IsEnabled()) {
      draw->disabled=true;
    }

    draw->focused=HasFocus();
    draw->activated=IsMouseActive();
  }

  void BooleanButton::DrawContent(OS::DrawInfo* draw,
                                  int x, int y, size_t w, size_t h)
  {
    image->Draw(draw,
                this->x,this->y+(height-image->GetHeight())/2);

    draw->selected=false;

    if (label!=NULL) {
      label->Draw(draw,x,y,w,h);
    }
  }

  BooleanButton* BooleanButton::Create(const std::wstring& label,
                         Base::Model* model,
                         bool horizontalFlex, bool verticalFlex)
  {
    BooleanButton *t=new BooleanButton();

    t->SetFlex(horizontalFlex,verticalFlex);
    t->SetModel(model);
    t->SetLabel(label);

    return t;
  }
}




