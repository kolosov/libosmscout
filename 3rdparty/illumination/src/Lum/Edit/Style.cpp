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

#include <Lum/Edit/Style.h>

namespace Lum {
  namespace Edit {

    ColorSheme::ColorSheme()
    {
      // no code
    }

    void ColorSheme::SetStyle(Style style,
                              OS::ColorRef& fg,
                              OS::ColorRef& bg,
                              unsigned long format)

    {
      styles[style].fg.Assign(fg);
      styles[style].bg.Assign(bg);
      styles[style].style=format;
    }

    void ColorSheme::AddColorizer(Colorizer colorizer)
    {
      this->colorizer.push_back(colorizer);
    }

    ColorShemeRef ColorSheme::GetDefaultColorSheme()
    {
      return new DefaultColorSheme();
    }

    DefaultColorSheme::DefaultColorSheme()
    {
      OS::ColorRef f,b,
                   fg(OS::display->GetColor(OS::Display::blackColor)),
                   bg(OS::display->GetColor(OS::Display::whiteColor));


      SetStyle(normalStyle,fg,bg,OS::Font::normal);

      // keyword
      //f.Allocate("red",fg);
      f.Allocate(1.0,0,0,fg);
      SetStyle(keywordStyle,f,bg,OS::Font::normal);

      // string
      if (OS::display->GetType()==OS::Display::typeTextual)  {
        f.Allocate("yellow",fg);
      }
      else {
        f.Allocate("orange",fg);
      }
      SetStyle(stringStyle,f,bg,OS::Font::normal);

      // comment
      if (OS::display->GetType()==OS::Display::typeTextual)  {
        f.Allocate("blue",fg);
        SetStyle(commentStyle,f,bg,OS::Font::normal);
      }
      else {
        f.Allocate("dim grey",fg);
        SetStyle(commentStyle,f,bg,OS::Font::slanted);
      }

      // docu & docuKeyword
      if (OS::display->GetType()==OS::Display::typeTextual)  {
        f.Allocate("blue",fg);
        SetStyle(docuStyle,f,bg,OS::Font::bold);

        f.Allocate("yellow",fg);
        SetStyle(docKeywordStyle,f,bg,OS::Font::bold);
      }
      else {
        f.Allocate("dark orange",fg);
        SetStyle(docuStyle,f,bg,OS::Font::normal);

        f.Allocate("dark orange",fg);
        SetStyle(docKeywordStyle,f,bg,OS::Font::bold);
      }

      // Special
      if (OS::display->GetType()==OS::Display::typeTextual)  {
        SetStyle(specialStyle,bg,fg,OS::Font::normal);
      }
      else {
        b.Allocate(0.95,0.95,0.95,bg);
        SetStyle(specialStyle,fg,b,OS::Font::normal);
      }

      // Cursor
      SetStyle(cursorStyle,bg,fg,OS::Font::normal);

      // mark
      f.Allocate("red",fg);
      b.Allocate("yellow",bg);
      SetStyle(markStyle,f,b,OS::Font::normal);

      // selection
      f.Allocate(OS::display->GetColor(OS::Display::fillTextColor));
      b.Allocate(OS::display->GetColor(OS::Display::fillColor));
      SetStyle(selectionStyle,f,b,OS::Font::normal);

      // bookmark
      f.Allocate(OS::display->GetColor(OS::Display::fillTextColor));
      b.Allocate(OS::display->GetColor(OS::Display::fillColor));
      SetStyle(bookmarkStyle,f,b,OS::Font::normal);

      // line
      b.Allocate("dark slate grey",OS::display->GetColor(OS::Display::backgroundColor));
      SetStyle(lineStyle,fg,b,OS::Font::normal);

      // operator
      f.Allocate("blue",fg);
      SetStyle(operatorStyle,f,bg,OS::Font::normal);

      // preprocess
      if (OS::display->GetType()==OS::Display::typeTextual)  {
        f.Allocate("magenta",fg);
        SetStyle(preprocessStyle,f,bg,OS::Font::bold);
      }
      else {
        f.Allocate("dark violet",fg);
        SetStyle(preprocessStyle,f,bg,OS::Font::normal);
      }

      // datatype
      if (OS::display->GetType()==OS::Display::typeTextual)  {
        f.Allocate("green",fg);
      }
      else {
        f.Allocate("dark green",fg);
      }
      SetStyle(datatypeStyle,f,bg,OS::Font::normal);

      // number
      if (OS::display->GetType()==OS::Display::typeTextual)  {
        f.Allocate("blue",fg);
      }
      else {
        f.Allocate("dark cyan",fg);
      }
      SetStyle(numberStyle,f,bg,OS::Font::normal);

      // function
      if (OS::display->GetType()==OS::Display::typeTextual)  {
        f.Allocate("green",fg);
      }
      else {
        f.Allocate("brown",fg);
      }
      SetStyle(functionStyle,f,bg,OS::Font::normal);

      // include
      if (OS::display->GetType()==OS::Display::typeTextual)  {
        f.Allocate("magenta",fg);
      }
      else {
        f.Allocate("purple",fg);
      }
      SetStyle(includeStyle,f,bg,OS::Font::normal);

      // quote1
      if (OS::display->GetType()==OS::Display::typeTextual)  {
        f.Allocate("cyan",fg);
      }
      else {
        f.Allocate("dark magenta",fg);
      }
      SetStyle(quote1Style,f,bg,OS::Font::normal);

      // quote2
      f.Allocate("blue",fg);
      SetStyle(quote2Style,f,bg,OS::Font::normal);

      // quote3
      f.Allocate("green",fg);
      SetStyle(quote3Style,f,bg,OS::Font::normal);

      // quote4
      f.Allocate("red",fg);
      SetStyle(quote4Style,f,bg,OS::Font::normal);

      // header
      f.Allocate("red",fg);
      SetStyle(headerStyle,f,bg,OS::Font::normal);

      // warning
      f.Allocate("orange",fg);
      SetStyle(warningStyle,f,bg,OS::Font::bold);

      // error
      f.Allocate("red",fg);
      SetStyle(errorStyle,f,bg,OS::Font::bold);

      // errorKeyword
      f.Allocate(OS::display->GetColor(OS::Display::whiteColor));
      b.Allocate("red",fg);
      SetStyle(errorKeywordStyle,f,b,OS::Font::normal);

      // Special
      if (OS::display->GetType()==OS::Display::typeTextual)  {
        SetStyle(currentLineStyle,bg,fg,OS::Font::normal);
      }
      else {
        b.Allocate(0.95,0.95,0.95,bg);
        SetStyle(currentLineStyle,fg,b,OS::Font::normal);
      }
    }
  }
}
