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

#include <Lum/OS/Cairo/Font.h>

#include <cmath>

#include <Lum/Base/String.h>

#include <Lum/OS/Cairo/Driver.h>

#if defined(CAIRO_HAS_WIN32_SURFACE)
  #include <cairo-win32.h>
#endif

namespace Lum {
  namespace OS {
    namespace Cairo {

      Font::Font()
      {
        for (size_t x=0; x<sizeof(styles)/sizeof(FontStyle*); x++) {
          styles[x]=NULL;
        }
      }

      Font::~Font()
      {
        for (size_t x=0; x<sizeof(styles)/sizeof(FontStyle*); x++) {
          if (styles[x]!=NULL) {
            if (styles[x]->fontFace!=NULL) {
              cairo_font_face_destroy(styles[x]->fontFace);
            }
            if (styles[x]->fontScaled!=NULL) {
              cairo_scaled_font_destroy(styles[x]->fontScaled);
            }

            delete styles[x];
          }
        }
      }

      Font::FontStyle* Font::GetStyle(unsigned long style)
      {
        FontStyle       *s;
        cairo_surface_t *surface=GetGlobalSurface();
        cairo_t         *cairo=GetGlobalCairo();

        // masking out all possible programatic styles
        style=(style & (bold|italic|slanted|underlined));

        s=styles[style];
        if (s!=NULL) {
          if (s->fontScaled!=NULL) {
            return s;
          }
          else {
            return NULL;
          }
        }

        s=new FontStyle;
        styles[style]=s;

        s->fontFace=NULL;
        s->fontScaled=NULL;

#if defined(CAIRO_HAS_WIN32_SURFACE)
        LOGFONTW info;

        info.lfHeight=0;
        info.lfWidth=0;
        info.lfEscapement=0;
        info.lfOrientation=0;
        info.lfWeight=FW_NORMAL; // TODO
        info.lfItalic=0;         // TODO
        info.lfUnderline=0;      // TODO
        info.lfStrikeOut=0;
        info.lfCharSet=DEFAULT_CHARSET;
        info.lfOutPrecision=OUT_DEFAULT_PRECIS;
        info.lfClipPrecision=CLIP_DEFAULT_PRECIS;
        info.lfQuality=ANTIALIASED_QUALITY;
        info.lfPitchAndFamily=DEFAULT_PITCH;
        info.lfFaceName[0]=L'\0';

        if (attrName & features) {
          std::wstring wname;

          wname=Lum::Base::StringToWString(name);
          wcscpy(info.lfFaceName,wname.c_str());
        }

        styles[style]->fontFace=cairo_win32_font_face_create_for_logfontw(&info);

        cairo_set_font_face(cairo,styles[style]->fontFace);
#else
        cairo_font_slant_t  slant;
        cairo_font_weight_t weight;

        if (style & italic) {
          slant=CAIRO_FONT_SLANT_ITALIC;
        }
        else if (style & slanted) {
          slant=CAIRO_FONT_SLANT_OBLIQUE;
        }
        else {
          slant=CAIRO_FONT_SLANT_NORMAL;
        }

        if (style & bold) {
          weight=CAIRO_FONT_WEIGHT_BOLD;
        }
        else {
          weight=CAIRO_FONT_WEIGHT_NORMAL;
        }

        cairo_select_font_face(cairo,name.c_str(),slant,weight);

#endif
        cairo_matrix_t       scaleMatrix;
        cairo_matrix_t       transformMatrix;
        cairo_font_options_t *options;
        cairo_font_extents_t fextents;

        cairo_matrix_init_scale(&scaleMatrix,
                                pixelHeight,
                                pixelHeight);
        cairo_get_matrix(cairo,&transformMatrix);
        options=cairo_font_options_create();
        cairo_surface_get_font_options(surface,options);
        styles[style]->fontScaled=cairo_scaled_font_create(cairo_get_font_face(cairo),
                                                           &scaleMatrix,
                                                           &transformMatrix,
                                                           options);

        cairo_font_options_destroy(options);

        if (style==normal) {
          cairo_scaled_font_extents(styles[style]->fontScaled,&fextents);

          height=size_t(ceil(fextents.height));
          ascent=int(ceil(fextents.ascent));
          descent=int(ceil(fextents.descent));

          if (height<(size_t)(ascent+descent)) {
            height=ascent+descent;
          }

          if (height>size_t(ascent+descent)) {
            descent+=(height-ascent-descent)/2;
            ascent+=(height-ascent-descent)-(height-ascent-descent)/2;
          }
        }

        return s;
      }

      size_t Font::StringWidth(const std::wstring& text, unsigned long style)
      {
        OS::FontExtent extent;

        StringExtent(text,extent,style);

        return extent.width;
      }

      void Font::StringExtent(const std::wstring& text, OS::FontExtent& extent, unsigned long style)
      {
        std::string          buffer;
        cairo_text_extents_t textents;
        FontStyle            *s;

        s=GetStyle(style);

        if (s!=NULL) {
          buffer=Lum::Base::WStringToUTF8(text);
          cairo_scaled_font_text_extents(s->fontScaled,buffer.c_str(),&textents);

          extent.left=int(round(textents.x_bearing));
          extent.right=int(round(textents.x_advance-textents.width-textents.x_bearing));
          extent.width=size_t(round(textents.x_advance));

          extent.height=size_t(round(textents.height));
          extent.ascent=int(round(-textents.y_bearing));
          extent.descent=int(round(textents.height+textents.y_bearing));
        }
      }

      OS::Font* Font::Load()
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
