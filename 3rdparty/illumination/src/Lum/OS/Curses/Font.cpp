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

#include <Lum/OS/Curses/Font.h>

namespace Lum {
  namespace OS {
    namespace Curses {

      Font::Font()
      {
        height=1;
        ascent=0;
        descent=1;
      }

      Font::~Font()
      {
      }

      size_t Font::StringWidth(const std::wstring& text, unsigned long /*style*/)
      {
        return text.length();
      }

      void Font::StringExtent(const std::wstring& text, OS::FontExtent& extent, unsigned long /*style*/)
      {
        extent.width=text.length();
        extent.left=0;
        extent.right=0;
        extent.height=1;
        extent.ascent=0;
        extent.descent=descent;
      }

      /**
        Loads the font. The OS specific font will be matched by evaluating the
        handed features. Every font loaded must be free using Free.

        RETURNS
        If the font can be loaded the method returns a new instance of font that
        must be used for future uses exspecially when calling Free.
      */
      ::Lum::OS::Font* Font::Load()
      {
        return this;
      }
    }
  }
}
