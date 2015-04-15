#ifndef LUM_OS_CARBON_FONT_H
#define LUM_OS_CARBON_FONT_H

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

#include <Lum/Private/Config.h>

// std
#include <string>

// Carbon
#include <Carbon/Carbon.h>

#include <Lum/OS/Base/Font.h>

namespace Lum {
  namespace OS {
    namespace Carbon {

      class Font : public ::Lum::OS::Base::Font
      {
      public:
        std::string    fullName;
        bool           loaded;

        int            lbearing;
        FMFontFamily   handle;
        ATSUFontID     atsu;
        ATSUTextLayout layout;
        ATSUStyle      styles[maxStyleNum];

      public:
        Font();
        virtual ~Font();

        size_t StringWidth(const std::wstring& text, unsigned long style=normal);
        void StringExtent(const std::wstring& text, OS::FontExtent& extent, unsigned long style=normal);

        ::Lum::OS::FontPtr Load();

        void SetFont(::CGrafPtr port, unsigned long style);

        /*
        void Complete();
        unsigned long GetFontPos(unsigned long style) const;*/
      };
    }
  }
}

#endif
