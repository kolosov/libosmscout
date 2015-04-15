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

#include <Lum/Text.h>

namespace Lum {

  Text::Part::Part(const std::wstring& text,
                   unsigned long style,
                   OS::Font *font)
  : width(0),ascent(0),descent(0),
    style(style),text(text),font(font),hOffset(0),vOffset(0)
  {
    // no code
  }

  void Text::Part::CalcSize(bool isFirst, bool isLast)
  {
    OS::FontExtent extent;

    font->StringExtent(text,extent,style);
    width=extent.width;
    hOffset=0;
    ascent=font->ascent;
    descent=font->descent;

    if (isFirst) {
      hOffset=extent.left;
      width-=extent.left;
    }

    if (isLast) {
      width-=extent.right;
    }

    if (OS::display->GetType()!=OS::Display::typeTextual) {
      if (superscript & style) {
        vOffset=-(int)font->height/3;
        ascent+=font->height/3;

        if (descent<font->height/3) {
          descent=0;
        }
        else {
          descent-=font->height/3;
        }
      }
      else if (subscript & style) {
        vOffset=font->height/3;
        descent+=font->height/3;

        if (ascent<font->height/3) {
          ascent=0;
        }
        else {
          ascent-=font->height/3;
        }
      }
      else {
        vOffset=0;
      }

      if (smart & style) {
        ascent++;
      }
    }
    else {
      style&=~(smart|superscript|subscript);
    }
  }

  Text::Line::Line(Alignment alignment)
  : alignment(alignment),parts(),width(0),height(0),ascent(0),descent(0)
  {
    // no code
  }

  Text::Line::~Line()
  {
    std::list<Part*>::iterator part;

    part=parts.begin();
    while (part!=parts.end()) {
      delete *part;

      ++part;
    }

    parts.clear();
  }

  void Text::Line::CalcSize(const OS::FontRef& defaultFont)
  {
    std::list<Part*>::iterator iter;

    width=0;
    ascent=0;
    descent=0;

    if (parts.size()>0) {
      iter=parts.begin();
      while (iter!=parts.end()) {
        (*iter)->CalcSize(iter==parts.begin(),(*iter)==parts.back());

        width+=(*iter)->width;
        ascent=std::max(ascent,(*iter)->ascent);
        descent=std::max(descent,(*iter)->descent);

        ++iter;
      }
    }
    else {
      ascent=defaultFont->ascent;
      descent=defaultFont->descent;
    }

    height=ascent+descent;
  }

  void Text::Line::AddText(const std::wstring& text,
                           unsigned long style,
                           OS::Font *font)
  {
    parts.push_back(new Part(text,style,font));
  }

  Text::Text(const std::wstring& text,
             unsigned long style,
             Alignment alignment,
             OS::Font *font)
  : current(NULL),
    textWidth(0),
    textHeight(0),
    lineSpace(0),
    calced(false),
    currentJust(left),
    defaultStyle(OS::Font::normal),
    defaultFont(OS::display->GetFont())
  {
    SetText(text,style,alignment,font);
  }

  Text::~Text()
  {
    Clear();
  }

  bool Text::HasBaseline() const
  {
    return true;
  }

  size_t Text::GetBaseline() const
  {
    assert(inited);

    if (text.begin()!=text.end()) {
      return GetY()-GetOY()+(*text.begin())->ascent;
    }
    else {
      return 0;
    }
  }

  void Text::Clear()
  {
    for (std::list<Line*>::iterator line=text.begin(); line!=text.end(); ++line) {
      delete *line;
    }

    text.clear();
    current=NULL;
  }

  void Text::CloseLine()
  {
    if (current==NULL) {
      // Create empty line, if no text was assigned after last CloseLine.
      this->text.push_back(new Line(currentJust));
    }
    current=NULL;
  }

  void Text::AddTextToPart(const std::wstring& text, unsigned long style, OS::Font *font)
  {
    if (text.empty()) {
      return;
    }

    if (current==NULL) {
      current=new Line(currentJust);
      this->text.push_back(current);
    }

    current->AddText(text,style,font);
  }

  void Text::SetAlignment(Alignment alignment)
  {
    if (current!=NULL) {
      current->alignment=alignment;
    }
    currentJust=alignment;
  }

  void Text::SetFont(OS::Font *font)
  {
    defaultFont=font;
  }

  void Text::SetStyle(unsigned long style)
  {
    defaultStyle=style;
  }

  void Text::SetText(const std::wstring& text)
  {
    Clear();

    AddText(text);
  }

  void Text::SetText(const std::wstring& text,
                     OS::Font *font)
  {
    Clear();

    AddText(text,font);
  }

  void Text::SetText(const std::wstring& text,
                     unsigned long style)
  {
    Clear();

    AddText(text,style);
  }

  void Text::SetText(const std::wstring& text,
                     unsigned long style,
                     OS::Font *font)
  {
    Clear();

    AddText(text,style,font);
  }

  void Text::SetText(const std::wstring& text,
                     unsigned long style,
                     Alignment alignment)
  {
    Clear();

    SetAlignment(alignment);
    SetText(text,style);
  }

  /**
    Parses text and builds up TextClass.textList
  */
  void Text::SetText(const std::wstring& text,
                     unsigned long style,
                     Alignment alignment,
                     OS::Font *font)
  {
    Clear();

    SetAlignment(alignment);
    AddText(text,style,font);
  }

  void Text::AddText(const std::wstring& text,
                     unsigned long style,
                     OS::Font *font)
  {
    size_t start,pos;

    if (font==NULL) {
      font=defaultFont;
    }

    start=0;
    pos=0;
    while (pos<text.length()) {
      if (text[pos]==L'\n') {
        AddTextToPart(text.substr(start,pos-start),style,font);
        CloseLine();
        start=pos+1;
      }
      pos++;
    }
    if (start<text.length()) {
      AddTextToPart(text.substr(start,pos-start),style,font);
    }

    if (calced) {
      CalcTextSize();

      if (visible) {
        Redraw();
      }
    }
  }

  void Text::AddText(const std::wstring& text,OS::Font *font)
  {
    AddText(text,defaultStyle,font);
  }

  void Text::AddText(const std::wstring& text)
  {
    AddText(text,defaultStyle,defaultFont);
  }

  void Text::AddText(const std::wstring& text,unsigned long style)
  {
    AddText(text,style,defaultFont);
  }

  void Text::AddLF()
  {
    AddText(L"\n",defaultStyle,defaultFont);
  }

  /**
    Calculates the size of TextClass.textList in respect to given font and rastPort.
  */
  void Text::CalcTextSize()
  {
    std::list<Line*>::iterator line;

    textHeight=0;
    textWidth=0;

    line=text.begin();
    while (line!=text.end()) {
      (*line)->CalcSize(defaultFont);

      textWidth=std::max(textWidth,(*line)->width);
      textHeight+=(*line)->height;

      ++line;
    }

    if (text.size()>0) {
      textHeight+=(text.size()-1)*lineSpace;
    }
    else {
      textHeight=defaultFont->height;
      textWidth=0;
    }
  }

  void Text::CalcSize()
  {
    CalcTextSize();

    calced=true;
    width=textWidth;
    height=textHeight;
    minWidth=textWidth;
    minHeight=textHeight;

    Object::CalcSize();
  }

  void Text::Draw(OS::DrawInfo* draw,
                  int x, int y, size_t w, size_t h)
  {
    Object::Draw(draw,x,y,w,h);

    if (!OIntersect(x,y,w,h)) {
      return;
    }

    /* --- */

    std::list<Line*>::iterator line;
    int                        xPos,yPos,y2;

    if ((draw->focused || ShowFocus()) &&
        OS::display->GetType()==OS::Display::typeTextual) {
      draw->PushForeground(OS::Display::fillTextColor);
    }
    else {
      draw->PushForeground(GetTextColor(draw,this));
    }

    draw->PushClip(x,y,w,h);

    yPos=this->y;

    if (height>textHeight) {
      yPos+=(height-textHeight) / 2;
    }

    line=text.begin();
    while (line!=text.end()) {

      xPos=this->x; // To make compiler happy
      switch ((*line)->alignment) {
      case left:
        xPos=this->x;
        break;
      case right:
        if (textWidth>width) {
          xPos=this->x+textWidth-(*line)->width;
        }
        else {
          xPos=this->x+width-(*line)->width;
        }
        break;
      case centered:
        xPos=this->x+(width-(*line)->width) / 2;
        break;
      }

      std::list<Part*>::iterator iter;

      iter=(*line)->parts.begin();
      while (iter!=(*line)->parts.end()) {
        Part *part=(*iter);

        draw->PushFont(part->font,part->style);

        y2=yPos+(*line)->ascent;

        if (draw->disabled) {
          draw->PushForeground(OS::Display::textDisabledColor);
          draw->DrawString(xPos-part->hOffset,y2+part->vOffset,part->text);
          draw->PopForeground();
        }
        else if ((part->style & smart) &&
                 OS::display->GetType()!=OS::Display::typeTextual) {
          draw->PushForeground(OS::Display::textSmartAColor);
          draw->DrawString(xPos+1-part->hOffset,y2+part->vOffset,part->text);
          draw->PopForeground();
          draw->PushForeground(OS::Display::textSmartBColor);
          draw->DrawString(xPos-part->hOffset,y2-1+part->vOffset,part->text);
          draw->PopForeground();
        }
        else {
          draw->DrawString(xPos-part->hOffset,y2+part->vOffset,part->text);
        }

        xPos+=part->width;

        draw->PopFont();

        ++iter;
      }

      yPos+=(*line)->height+lineSpace;
      ++line;
    }

    draw->PopClip();
    draw->PopForeground();
  }

  Text* Lum::Text::Create(const std::wstring& text,
                          bool horizontalFlex, bool verticalFlex)
  {
    Text *t=new Lum::Text();

    t->SetFlex(horizontalFlex,verticalFlex);
    t->SetText(text);

    return t;
  }

  Text* Lum::Text::Create(const std::wstring& text,
                          Text::Alignment alignment,
                          bool horizontalFlex, bool verticalFlex)
  {
    Text *t=new Lum::Text();

    t->SetFlex(horizontalFlex,verticalFlex);
    t->SetText(text);
    t->SetAlignment(alignment);

    return t;
  }
}
