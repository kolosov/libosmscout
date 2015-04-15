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

#include <Lum/Scale.h>

#include <Lum/Base/String.h>

namespace Lum {

  static int minScaleInterval=3;

  /**
    Calculates log10 number.
  */
  static int Exp(int number)
  {
    int exp=0;

    while (number/10>0) {
      number=number / 10;
      exp++;
    }

    return exp;
  }

  /**
    Calculates 10^exp
   */
  static int Pow10(int exp)
  {
    int x=1;

    while (exp>0) {
      x=x*10;
      exp--;
    }

    return x;
  }

  Scale::Scale()
  : from(0),to(100),
    font(OS::display->GetFont(OS::Display::fontTypeProportional,
                              OS::Display::fontScaleFootnote))
  {
    // no code
  }

  void Scale::SetInterval(int from, int to)
  {
    this->from=from;
    this->to=to;
  }

  void HScale::CalcSize()
  {
    if (OS::display->GetType()==OS::Display::typeTextual) {
      width=Base::GetSize(Base::Size::unit,1);
      height=Base::GetSize(Base::Size::unit,1);
    }
    else {
      width=Base::GetSize(Base::Size::unit,3);
      height=Base::GetSize(Base::Size::unit,3);
    }

    fromText=Base::NumberToWString(long(from));
    toText=Base::NumberToWString(long(to));

    font->StringExtent(fromText,fromExt);
    font->StringExtent(toText,toExt);

    height+=std::max(fromExt.height,toExt.height)+OS::display->GetSpaceVertical(OS::Display::spaceObjectBorder);

    minWidth=width;
    minHeight=height;

    Object::CalcSize();
  }

  void HScale::Draw(OS::DrawInfo* draw,
                    int x, int y, size_t w, size_t h)
  {
    Object::Draw(draw,x,y,w,h);

    if (!OIntersect(x,y,w,h)) {
      return;
    }

    /* --- */

    int interval,exp,max,pos,mark,off,width,height,intv;

    interval=to-from;

    exp=Exp(interval);
    max=exp;

    draw->PushForeground(OS::Display::textColor);

    off=std::max(fromExt.height,toExt.height)+OS::display->GetSpaceVertical(OS::Display::spaceObjectBorder);
    width=this->width-1;
    height=this->height-off;

    while (exp>=0 && (width*Pow10(exp)) / interval>minScaleInterval) {
      intv=Pow10(exp);
      mark=(from / intv)*intv;

      while (mark<=to) {
        if (mark>=from && (exp==max || mark % Pow10(exp+1)!=0)) {
          pos=width*(mark-from) / interval;
          draw->PushForeground(OS::Display::textColor);
          draw->DrawLine(this->x+pos,this->y+off+height-(height / (max-exp+1)),
                         this->x+pos,this->y+off+height-1);
          draw->PopForeground();
        }
        mark+=intv;
      }
      exp--;
    }

    draw->PushFont(font,OS::Font::normal);
    draw->DrawString(this->x,this->y+font->ascent,fromText);
    draw->DrawString(this->x+this->width-toExt.width,this->y+font->ascent,toText);
    draw->PopFont();

    draw->PopForeground();
  }

  void VScale::CalcSize()
  {
    if (OS::display->GetType()==OS::Display::typeTextual) {
      width=Base::GetSize(Base::Size::unit,1);
      height=Base::GetSize(Base::Size::unit,1);
    }
    else {
      width=Base::GetSize(Base::Size::unit,3);
      height=Base::GetSize(Base::Size::unit,3);
    }

    fromText=Base::NumberToWString(long(from));
    toText=Base::NumberToWString(long(to));

    font->StringExtent(fromText,fromExt);
    font->StringExtent(toText,toExt);

    width+=std::max(fromExt.width,toExt.width)+OS::display->GetSpaceHorizontal(OS::Display::spaceObjectBorder);

    minWidth=width;
    minHeight=height;

    Object::CalcSize();
  }

  void VScale::Draw(OS::DrawInfo* draw,
                    int x, int y, size_t w, size_t h)
  {
    Object::Draw(draw,x,y,w,h);

    if (!OIntersect(x,y,w,h)) {
      return;
    }

    int interval,exp,max,pos,mark,off,width,height,intv;

    interval=to-from;

    exp=Exp(interval);
    max=exp;

    draw->PushForeground(OS::Display::textColor);

    off=std::max(fromExt.width,toExt.width)+OS::display->GetSpaceHorizontal(OS::Display::spaceObjectBorder);
    height=this->height-1;
    width=this->width-off;


    while (exp>=0 && (height*Pow10(exp)) / interval>minScaleInterval) {
      intv=Pow10(exp);
      mark=(from / intv)*intv;

      while (mark<=to) {
        if (mark>=from && (exp==max || mark % Pow10(exp+1)!=0)) {
          pos=height*(mark-from) / interval;
          draw->PushForeground(OS::Display::textColor);
          draw->DrawLine(this->x+off+width-(width / (max-exp+1)),this->y+pos,
                         this->x+off+width-1,this->y+pos);
          draw->PopForeground();
        }
        mark+=intv;
      }
      exp--;
    }

    draw->PushFont(font,OS::Font::normal);
    draw->DrawString(this->x,this->y+this->height-1,fromText);
    draw->DrawString(this->x,this->y+font->ascent,toText);
    draw->PopFont();

    draw->PopForeground();
  }
}
