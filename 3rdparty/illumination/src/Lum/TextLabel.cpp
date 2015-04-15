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

#include <Lum/TextLabel.h>

#include <Lum/Base/Util.h>

#include <Lum/OS/Font.h>

namespace Lum {

  TextLabel::TextLabel(const std::wstring& text)
  : text(text),
    font(OS::display->GetFont()),
    alignment(left),
    style(OS::Font::normal)
  {
    SetBackground(OS::display->GetFill(OS::Display::labelBackgroundFillIndex));
  }

  TextLabel::~TextLabel()
  {
    // no code
  }

  bool TextLabel::HasBaseline() const
  {
    return true;
  }

  size_t TextLabel::GetBaseline() const
  {
    return GetTopBorder()+(height-font->height)/2+font->ascent;
  }

  void TextLabel::SetText(const std::wstring& text)
  {
    this->text=text;

    if (visible) {
      Redraw();
    }
  }

  void TextLabel::SetFont(OS::Font *font)
  {
    this->font=font;

    if (visible) {
      Redraw();
    }
  }

  void TextLabel::SetStyle(unsigned long style)
  {
    this->style=style;

    if (visible) {
      Redraw();
    }
  }

  void TextLabel::SetAlignment(Alignment alignment)
  {
    this->alignment=alignment;

    if (visible) {
      Redraw();
    }
  }

  void TextLabel::CalcSize()
  {
    Lum::OS::FontExtent extent;

    font->StringExtent(text,extent,style);

    minWidth=extent.width;
    minHeight=font->height;

    width=minWidth;
    height=minHeight;

    Object::CalcSize();
  }

  void TextLabel::PrepareForBackground(OS::DrawInfo* draw)
  {
    draw->focused=HasFocus() || ShowFocus();
  }

  void TextLabel::Draw(OS::DrawInfo* draw,
                       int x, int y, size_t w, size_t h)
  {
    Object::Draw(draw,x,y,w,h);

    if (!OIntersect(x,y,w,h)) {
      return;
    }

    /* --- */

    if (text.length()>0) {
      Lum::OS::FontExtent extent;

      font->StringExtent(text,extent,style);

      if (extent.width-extent.left-extent.right>width) {
        draw->PushClip(this->x,this->y,this->width,this->height);
      }

      draw->PushForeground(OS::Display::textColor);
      draw->PushFont(font,style);
      switch (alignment) {
      case left:
        draw->DrawString(this->x-extent.left,
                         this->y+(height-font->height)/2+font->ascent,
                         text);
        break;
      case centered:
        draw->DrawString(this->x-extent.left+(width-extent.width)/2,
                         this->y+(height-font->height)/2+font->ascent,
                         text);
        break;
      case right:
        draw->DrawString(this->x-extent.left+width-extent.width,
                         this->y+(height-font->height)/2+font->ascent,
                         text);
        break;
      }
      draw->PopFont();
      draw->PopForeground();

      if (extent.width-extent.left-extent.right>width) {
        draw->PopClip();
      }
    }
  }
}
