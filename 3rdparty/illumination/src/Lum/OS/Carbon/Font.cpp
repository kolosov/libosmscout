/*
  This source is part of the Illumination library
  Copyright (C) 2006  Tim Teulings

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

#include <Lum/OS/Carbon/Font.h>

#include <unistd.h>

#include <iostream>

#include <Lum/Base/Util.h>

#include <Lum/OS/Carbon/Display.h>

namespace Lum {
  namespace OS {
    namespace Carbon {

      Font::Font()
      : loaded(false),handle(0),atsu(0)
      {
        for (size_t x=0; x<maxStyleNum ; x++) {
          styles[x]=0;
        }
      }

      Font::~Font()
      {
        /*
        for (size_t x=0; x<maxStyleNum ; x++) {
          if (fonts[x]!=NULL) {
            XFreeFont(dynamic_cast<Display*>(::Lum::OS::display)->display,fonts[x]);
          }
        }*/
      }

      size_t Font::StringWidth(const std::wstring& text, unsigned long style)
      {
        ::ATSUTextMeasurement before,after,asc,desc;
        size_t                length;
        char                  *tmp=::Lum::Base::WStringToChar16BE(text,length);

        ::SetGWorld(dynamic_cast<Display*>(::Lum::OS::display)->fontScratch,NULL);

        style=(style&(maxStyleNum-1));

        if (::ATSUSetTextPointerLocation(layout,
                                         (const UniChar*)tmp,
                                         ::kATSUFromTextBeginning,
                                         ::kATSUToTextEnd,
                                         length)!=::noErr) {
          std::cerr << "Cannot set text pointer" << std::endl;
        }

        if (::ATSUSetRunStyle(layout,
                              styles[style],
                              0,length)!=::noErr) {
          std::cerr << "Cannot set run style" << std::endl;
        }


        if (ATSUGetUnjustifiedBounds(layout,0,length,&before,&after,&asc,&desc)!=::noErr) {
          std::cerr << "Cannot calculate text bounds" << std::endl;
        }

        delete [] tmp;

        return Fix2Long(after)-Fix2Long(before);
      }

      void Font::StringExtent(const std::wstring& text, OS::FontExtent& extent, unsigned long style)
      {
        ::ATSUTextMeasurement before,after,asc,desc;
        size_t                length;
        char                  *tmp=::Lum::Base::WStringToChar16BE(text,length);

        ::SetGWorld(dynamic_cast<Display*>(::Lum::OS::display)->fontScratch,NULL);

        style=(style&(maxStyleNum-1));

        if (::ATSUSetTextPointerLocation(layout,
                                         (const UniChar*)tmp,
                                         ::kATSUFromTextBeginning,
                                         ::kATSUToTextEnd,
                                         length)!=::noErr) {
          std::cerr << "Cannot set text pointer" << std::endl;
        }

        if (::ATSUSetRunStyle(layout,
                              styles[style],
                              0,length)!=::noErr) {
          std::cerr << "Cannot set run style" << std::endl;
        }


        if (ATSUGetUnjustifiedBounds(layout,0,length,&before,&after,&asc,&desc)!=::noErr) {
          std::cerr << "Cannot calculate text bounds" << std::endl;
        }

        delete [] tmp;

        if (text.length()==0) {
          extent.left=0;
          extent.right=0;
          extent.width=0;
        }
        else {
          extent.left=0;
          extent.right=0;
          extent.width=Fix2Long(after-before);
        }
        extent.ascent=::Fix2Long(asc);
        extent.descent=::Fix2Long(desc);
        extent.height=extent.ascent+extent.descent;
      }

      /**
        Loads the font. The OS specific font will be matched by evaluating the
        handed features. Every font loaded must be free using Free.

        RETURNS
        If the font can be loaded the method returns a new instance of font that
        must be used for future uses exspecially when calling Free.
      */
      ::Lum::OS::FontPtr Font::Load()
      {
        ::FontInfo              metric;
        ::ATSUAttributeTag      tags[6];
        ::ByteCount             sizes[6];
        ::ATSUAttributeValuePtr values[6];

        ::Fixed                 size;
        ::Boolean               caret;
        ::Boolean               bold,italic,underline;

        handle=::FMGetFontFamilyFromName(name.c_str());

        SetFont(dynamic_cast<Display*>(::Lum::OS::display)->fontScratch,0);

        GetFontInfo(&metric);

        //font.left=metric.leading;
        height=metric.ascent+metric.descent;
        descent=metric.descent;
        ascent=metric.ascent;

        /* ATSU font handling */

        if (::ATSUFindFontFromName(name.c_str(),
                                   name.length(),
                                   ::kFontFullName,/*Carbon.kFontNoNameCode*/
                                   ::kFontNoPlatformCode,
                                   ::kFontNoScriptCode,
                                   ::kFontNoLanguageCode,
                                   &atsu)!=::noErr) {
          std::cerr << "Cannot load font!" << std::endl;
          return NULL;
        }

        tags[0]=::kATSUSizeTag;
        tags[1]=::kATSUFontTag;
        tags[2]=::kATSUNoCaretAngleTag;
        tags[3]=::kATSUQDBoldfaceTag;
        tags[4]=::kATSUQDItalicTag;
        tags[5]=::kATSUQDUnderlineTag;

        sizes[0]=sizeof(::Fixed);
        sizes[1]=sizeof(::ATSUFontID);
        sizes[2]=sizeof(::Boolean);
        sizes[3]=sizeof(::Boolean);
        sizes[4]=sizeof(::Boolean);
        sizes[5]=sizeof(::Boolean);

        size=::Long2Fix(pixelHeight);
        caret=1;

        values[0]=&size;
        values[1]=&atsu;
        values[2]=&caret;
        values[3]=&bold;
        values[4]=&italic;
        values[5]=&underline;

        for (unsigned long style=0; style<maxStyleNum; ++style) {
          bold=(style & bold);
          italic=((italic & style) || (slanted & style));
          underline=(underlined & style);

          if (::ATSUCreateStyle(&styles[style])!=::noErr) {
            std::cerr << "Cannot create style" << std::endl;
          }

          if (::ATSUSetAttributes(styles[style],
                                  6,
                                  tags,
                                  sizes,
                                  values)!=::noErr) {
            std::cerr << "Cannot set style attributes" << std::endl;
          }
        }

        if (::ATSUCreateTextLayout(&layout)!=::noErr) {
          std::cerr << "Cannot create layout" << std::endl;
        }

        return this;
      }

      /**
        Assign the given font settings to the internal scratch world
        for further evaluation.
      */
      void Font::SetFont(::CGrafPtr port, unsigned long style)
      {
        ::StyleParameter face;

        ::SetGWorld(port,NULL);

        face=0;
        if (bold & style) {
          face|=::bold;
        }

        if ((italic & style) | (slanted & style)) {
          face|=::italic;
        }

        if (underlined & style) {
          face|=::underline;
        }

        ::TextFont(handle);
        ::TextSize(pixelHeight);
        ::TextFace(face);
      }

    }
  }
}
