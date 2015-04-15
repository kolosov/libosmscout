#ifndef LUM_OS_FONT_H
#define LUM_OS_FONT_H

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

#include <string>

#include <Lum/Private/ImportExport.h>

#include <Lum/Base/Reference.h>

namespace Lum {
  namespace OS {

    /*
      class FontSize
      {
        public:
        int size;
      };

      class FontEncoding
      {
        public:
        ::VO::Base::Util::Text encoding;
        FontSize sizes;
        bool anySize;
        FontSize GetOrCreateSize(int size);
      };

      class FontFoundry
      {
        public:
        ::VO::Base::Util::Text name;
        FontEncoding encodings;
        FontEncoding GetOrCreateEncoding(char name[]);
      };

      class FontFamily
      {
        public:
        ::VO::Base::Util::Text name;
        FontFoundry foundries;
        FontFoundry GetOrCreateFoundry(char name[]);
      };

      class FontList
      {
        public:
        FontFamily families;
        FontFamily GetOrCreateFamily(char name[]);
      };
*/

    /**
      Class holding size and bound information for a single character or a complex string.
    */
    class LUMAPI FontExtent
    {
    public:
      int    left;    //! Origin to start of character
      int    right;   //! Space behind last character
      size_t width;   //! Width of text including left and right
      size_t height;  //! height
      int    ascent;  //! ascent
      int    descent; //! descent
    };

    /**
      Class abstracting OS specific fonts.
    */
    class LUMAPI Font : public Lum::Base::Referencable
    {
    public:
      enum Attribute {
        attrFoundry     =  (1 << 0), //! Optional font class/foundry name
        attrName        =  (1 << 1), //! This should always be set
        attrHeight      =  (1 << 2),
        attrAscent      =  (1 << 3), //! I'm not sure if these two are really  feature
        attrDescent     =  (1 << 4),
        attrPointHeight =  (1 << 5), //! This should always be set
        attrAvWidth     =  (1 << 6),
        attrSetWidth    =  (1 << 7),
        attrSpacing     =  (1 << 8),
        attrCharSet     =  (1 << 9)
      };

      enum Spacing {
        spacingMonospace,
        spacingProportional,
        spacingDefault
      };

      static const unsigned long normal      = 0;        //! Normal font
      static const unsigned long bold        = (1 << 0); //! Bold
      static const unsigned long italic      = (1 << 1); //! Italic
      static const unsigned long underlined  = (1 << 2); //! Underlined
      static const unsigned long slanted     = (1 << 3); //! Slanted (unlike italic this is a generated style by transforming normal)

      static const unsigned long maxStyle    = 3;       //! highest used style bit

    public:
      unsigned long features;
      std::string   foundry;
      std::string   name;
      std::string   charSet;
      int           ascent;      //! Space above the font baseline including inter-line space
      int           descent;     //! Space below the font baseline including inter-line space
      size_t        height;      //! Height of font including inter-line space
      double        pixelHeight;
      double        pointHeight;
      size_t        avWidth;
      size_t        setWidth;
      Spacing       spacing;

    public:
      Font();
      virtual ~Font();

      /**
        Return the width of the given string using the given style in pixel.
      */
      virtual size_t StringWidth(const std::wstring& text,
                                 unsigned long style=normal) = 0;

      /**
        Returns all sisze information regarding the dimension of the given string using
        the given style in pixel.
      */
      virtual void StringExtent(const std::wstring& text,
                                FontExtent& extent,
                                unsigned long style=normal) = 0;

      /**
        Load this font using the given font attributes as specified by the features set and
        the referenced values.
      */
      virtual Font* Load() = 0;
    };

#if defined(LUM_INSTANTIATE_TEMPLATES)
    LUM_EXPTEMPL template class LUMAPI Lum::Base::Reference<Font>;
#endif

    typedef Lum::Base::Reference<Font> FontRef;
  }
}

#endif
