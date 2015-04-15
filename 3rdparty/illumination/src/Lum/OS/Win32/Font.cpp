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

#include <Lum/OS/Win32/Font.h>

#include <cstdlib>

#include <Lum/OS/Win32/Display.h>

#include <iostream>

namespace Lum {
  namespace OS {
    namespace Win32 {
      Font::Font()
      {
        height=1;
        descent=1;
        ascent=0;

        for (size_t x=0; x<sizeof(styles)/sizeof(FontStyle*); x++) {
          styles[x]=NULL;
        }
      }

      Font::~Font()
      {
        for (size_t x=0; x<sizeof(styles)/sizeof(FontStyle*); x++) {
          delete styles[x];
        }
      }

      Font::FontStyle* Font::GetStyle(unsigned long style)
      {
        LOGFONT   info;
        HFONT     hFont;
        FontStyle *s;

        // masking out all possible programatic styles
        style=(style & (bold|italic|slanted|underlined));

        s=styles[style];
        if (s!=NULL) {
          if (s->hFont!=0) {
            return s;
          }
          else {
            return NULL;
          }
        }

        s=new FontStyle;
        styles[style]=s;

        s->hFont=0;

        /* Initialisation of default fields */
        info.lfHeight=0;
        info.lfWidth=0;
        info.lfEscapement=0;
        info.lfOrientation=0;
        info.lfWeight=FW_NORMAL;
        info.lfItalic=0;
        info.lfUnderline=0;
        info.lfStrikeOut=0;
        info.lfCharSet=DEFAULT_CHARSET;
        info.lfOutPrecision=OUT_DEFAULT_PRECIS;
        info.lfClipPrecision=CLIP_DEFAULT_PRECIS;
        info.lfQuality=DEFAULT_QUALITY;
        info.lfPitchAndFamily=DEFAULT_PITCH;
        info.lfFaceName[0]='\0';

        if (attrHeight & features) {
          info.lfHeight=std::abs(MulDiv(pixelHeight,GetDeviceCaps(dynamic_cast<Display*>(OS::display)->hdc,LOGPIXELSY),72));
        }
        
        if (attrName & features) {
          strcpy(info.lfFaceName,name.c_str());
        }

        if (bold & style) {
          info.lfWeight=FW_BOLD;
        }
        if ((italic & style) || (slanted & style)) {
          info.lfItalic=true;
        }
        if (underlined & style) {
          info.lfUnderline=true;
        }

        hFont=CreateFontIndirectA(&info);
        if (hFont!=0) {
          Display    *display;
          HGDIOBJ    old;

          display=dynamic_cast<Display*>(OS::display);

          old=SelectObject(display->hdc,hFont);

          if (old==NULL || old==HGDI_ERROR) {
            std::cerr << "Cannot select font: " << GetLastError() << std::endl;
          }

          if (GetTextMetricsA(display->hdc,&s->metrics)==0) {
            std::cerr << "Failed to get font metrics:" << GetLastError() << std::endl;
          }
          else {
            s->hFont=hFont;
            if (style==0) {
              height=s->metrics.tmHeight;
              ascent=s->metrics.tmAscent;
              descent=s->metrics.tmDescent;
            }
          }
          /* ignore */ SelectObject(display->hdc,old);
        }

        return s;
      }

      size_t Font::StringWidth(const std::wstring& text, unsigned long style)
      {
        SIZE       size;
        FontStyle  *s;

        s=GetStyle(style);
        if (s!=NULL) {
          Display    *display;
          HGDIOBJ    old;

          display=dynamic_cast<Display*>(OS::display);
          old=SelectObject(display->hdc,s->hFont);
          /* ignore */ GetTextExtentPoint32W(display->hdc,text.c_str(),text.length(),&size);
          old=SelectObject(display->hdc,old);

          return size.cx;
        }
        else {
          return 0;
        }
      }

      void Font::StringExtent(const std::wstring& text, FontExtent& extent, unsigned long style)
      {
        SIZE       size;
        FontStyle  *s;

        s=GetStyle(style);
        if (s!=NULL) {
          Display    *display;
          HGDIOBJ    old;

          display=dynamic_cast<Display*>(OS::display);
          old=SelectObject(display->hdc,s->hFont);
          /* ignore */ GetTextExtentPoint32W(display->hdc,text.c_str(),text.length(),&size);
          old=SelectObject(display->hdc,old);

          extent.left=0;
          extent.right=0;
          extent.width=size.cx;
          extent.height=size.cy;
          extent.ascent=s->metrics.tmAscent;
          extent.descent=s->metrics.tmDescent;
        }
      }

      Font* Font::Load()
      {
        if (GetStyle(normal)!=NULL) {
          return this;
        }
        else {
          return NULL;
        }
      }
    }
  }
}
