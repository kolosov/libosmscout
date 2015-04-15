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

#include <Lum/TextValue.h>

#include <Lum/Base/Util.h>

#include <Lum/OS/Font.h>

namespace Lum {

  TextValue::TextValue()
  : font(OS::display->GetFont()),
    alignment(left),
    style(OS::Font::normal)
  {
    // no code
  }

  TextValue::~TextValue()
  {
    // no code
  }

  bool TextValue::HasBaseline() const
  {
    return true;
  }

  size_t TextValue::GetBaseline() const
  {
    return GetTopBorder()+(height-font->height)/2+font->ascent;
  }

  bool TextValue::SetModel(Base::Model* model)
  {
    this->model=dynamic_cast<Model::String*>(model);

    Control::SetModel(this->model);

    return this->model.Valid();
  }

  void TextValue::SetFont(OS::Font *font)
  {
    this->font=font;

    if (visible) {
      Redraw();
    }
  }

  void TextValue::SetStyle(unsigned long style)
  {
    this->style=style;

    if (visible) {
      Redraw();
    }
  }

  void TextValue::SetAlignment(Alignment alignment)
  {
    this->alignment=alignment;

    if (visible) {
      Redraw();
    }
  }

  void TextValue::CalcSize()
  {
    width=OS::display->GetSpaceHorizontal(OS::Display::spaceInterGroup);
    height=font->height;

    minWidth=width;
    minHeight=height;

    Control::CalcSize();
  }

  void TextValue::Draw(OS::DrawInfo* draw,
                       int x, int y, size_t w, size_t h)
  {
    Control::Draw(draw,x,y,w,h);

    if (!OIntersect(x,y,w,h)) {
      return;
    }

    /* --- */

    if (model.Valid() && !model->IsNull()) {
      Lum::OS::FontExtent extent;

      font->StringExtent(model->Get(),extent,style);

      if (extent.width-extent.left-extent.right>width) {
        draw->PushClip(this->x,this->y,this->width,this->height);
      }

      draw->PushForeground(OS::Display::textColor);
      draw->PushFont(font,style);

      switch (alignment) {
      case left:
        draw->DrawString(this->x-extent.left,
                         this->y+(height-font->height)/2+font->ascent,
                         model->Get());
        break;
      case centered:
        draw->DrawString(this->x-extent.left+(width-extent.width)/2,
                         this->y+(height-font->height)/2+font->ascent,
                         model->Get());
        break;
      case right:
        draw->DrawString(this->x-extent.left+width-extent.width,
                         this->y+(height-font->height)/2+font->ascent,
                         model->Get());
        break;
      }

      draw->PopFont();
      draw->PopForeground();

      if (extent.width-extent.left-extent.right>width) {
        draw->PopClip();
      }
    }
  }

  void TextValue::Resync(Base::Model* model, const Base::ResyncMsg& msg)
  {
    if (model==this->model) {
      Redraw();
    }
    else {
      Control::Resync(model,msg);
    }
  }

  TextValue* TextValue::Create(bool horizontalFlex, bool verticalFlex)
  {
    TextValue* t;

    t=new TextValue();
    t->SetFlex(horizontalFlex,verticalFlex);

    return t;
  }

  TextValue* TextValue::Create(Base::Model* model,
                               bool horizontalFlex, bool verticalFlex)
  {
    TextValue* t;

    t=new TextValue();
    t->SetFlex(horizontalFlex,verticalFlex);
    t->SetModel(model);

    return t;
  }

  TextValue* TextValue::Create(Base::Model* model,
                               Alignment alignment,
                               bool horizontalFlex, bool verticalFlex)
  {
    TextValue* t;

    t=new TextValue();
    t->SetFlex(horizontalFlex,verticalFlex);
    t->SetAlignment(alignment);
    t->SetModel(model);

    return t;
  }

}
