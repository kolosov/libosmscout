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

#include <Lum/DateView.h>

#include <Lum/Base/String.h>

#include <Lum/OS/Font.h>

namespace Lum {

  DateView::DateView()
  {
    SetBackground(OS::display->GetFill(OS::Display::entryBackgroundFillIndex));
  }

  DateView::~DateView()
  {
    // no code
  }

  bool DateView::HasBaseline() const
  {
    return true;
  }

  size_t DateView::GetBaseline() const
  {
    OS::FontRef font=OS::display->GetFont();

    return GetTopBorder()+(height-font->height)/2+font->ascent;
  }

  bool DateView::SetModel(Base::Model* model)
  {
    this->model=dynamic_cast<Model::Calendar*>(model);

    Control::SetModel(this->model);

    return this->model.Valid();
  }

  void DateView::CalcSize()
  {
    OS::FontRef    font;
    Base::Calendar now;
    std::wstring   exmpl;

    font=OS::display->GetFont();

    exmpl=now.GetLocaleDate();

    width=exmpl.length()*Base::GetSize(Base::Size::stdCharWidth,1,font)+2*OS::display->GetSpaceHorizontal(OS::Display::spaceObjectBorder);
    height=font->height;

    minWidth=width;
    minHeight=height;

    Object::CalcSize();
  }

  void DateView::PrepareForBackground(OS::DrawInfo* draw)
  {
    draw->disabled=true;
  }

  void DateView::Draw(OS::DrawInfo* draw,
                      int x, int y, size_t w, size_t h)
  {
    Object::Draw(draw,x,y,w,h);

    if (!OIntersect(x,y,w,h)) {
      return;
    }

    /* --- */

    std::wstring   buffer;
    OS::FontRef    font=OS::display->GetFont();
    OS::FontExtent extent;

    if (model.Valid() && !model->IsNull()) {
      buffer=model->Get().GetLocaleDate();
    }

    font->StringExtent(buffer,extent);

    if (!model.Valid() || !model->IsEnabled()) {
      draw->PushForeground(OS::Display::textDisabledColor);
    }
    else {
      draw->PushForeground(OS::Display::textColor);
    }

    draw->PushFont(font,0);
    draw->DrawString(this->x+(width-extent.width)-OS::display->GetSpaceHorizontal(OS::Display::spaceObjectBorder),
                     this->y+(height-font->height)/2+font->ascent,
                     buffer);
    draw->PopFont();
    draw->PopForeground();
  }

  void DateView::Resync(Base::Model* model, const Base::ResyncMsg& msg)
  {
    if (model==this->model) {
      Redraw();
    }

    Control::Resync(model,msg);
  }

  DateView* DateView::Create(bool horizontalFlex, bool verticalFlex)
  {
    DateView *d=new DateView();

    d->SetFlex(horizontalFlex,verticalFlex);

    return d;
  }

  DateView* DateView::Create(Base::Model* model, bool horizontalFlex, bool verticalFlex)
  {
    DateView *d=new DateView();

    d->SetFlex(horizontalFlex,verticalFlex);
    d->SetModel(model);

    return d;
  }

}
