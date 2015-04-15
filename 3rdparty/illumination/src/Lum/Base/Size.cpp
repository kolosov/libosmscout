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

#include <Lum/Base/Size.h>

#include <math.h>

#include <cassert>
#include <vector>

#include <Lum/OS/Display.h>

namespace Lum {
  namespace Base {

    static size_t valueUnit=(size_t)-1;  //! Base value in pixel, calculated from the screen width, is always >0
    static size_t valueSUnit=(size_t)-1; //! Base value in pixel, calculated from the screen width, may be 0

    /**
      Initialy calculates all relevant base values, which are then later used to calculate
      values for all given modes and values.
    */
    void CalculateUnits()
    {
      valueUnit=OS::display->GetScreenWidth() / 300;
      valueSUnit=valueUnit;
      if (valueUnit==0) {
        valueUnit=1;
      }
    }

    /**
      Returns the calculated size in pixel for the given \p mode, \p value and \p font.
    */
    int GetSize(Size::Mode mode,
                size_t value,
                const OS::FontRef& font)
    {
      size_t             help;
      OS::FontRef f(font);

      if (!f.Valid()) {
        f=OS::display->GetFont();
      }

      if (valueUnit==(size_t)-1) {
        CalculateUnits();
      }

      switch (mode) {
      case Size::pixel:
        return value;
      case Size::mm:
        // TODO: To be approved!
        return (int)round((value*OS::display->GetDPI())/25.4);
      case Size::workHRel:
        return (value*OS::display->GetWorkAreaWidth()) / 100;
      case Size::workVRel:
        return (value*OS::display->GetWorkAreaHeight()) / 100;
      case Size::unit:
        return (value*valueUnit);
      case Size::unitP:
        help=(value*valueUnit) / 100;
        if (help==0) {
          return 1;
        }
        else {
          return help;
        }
      case Size::softUnit:
        return (value*valueSUnit);
      case Size::softUnitP:
        return (value*valueSUnit) / 100;
      case Size::stdCharWidth:
        return value*(f->StringWidth(L"M",0)+f->StringWidth(L"x",0)+f->StringWidth(L"E",0)) / 3;
      case Size::stdCharHeight:
        return value*f->height;
      }

      return 0;
    }

    Size::Size(Size::Mode mode,
               size_t size,
               const OS::FontRef& font)
    : value((size_t)-1),mode(mode),size(size),font(font)
    {
      // no code
    }

    /**
      Set the abstract size using one of the predefined modes, a value based on definition of the
      mode and - for some modes - the index of one of the internal predefined fonts.
    */
    void Size::SetSize(Size::Mode mode,
                       size_t size,
                       const OS::FontRef& font)
    {
      this->mode=mode;
      this->size=size;
      this->font=font;
      this->value=(size_t)-1;
    }

    /**
      Returns the calculated size in pixel for the earlier assigned mode, value and font.
    */
    size_t Size::GetSize() const
    {
      if (value==(size_t)-1) {
        value=Base::GetSize(mode,size,font);
      }

      return value;
    }
  }
}
