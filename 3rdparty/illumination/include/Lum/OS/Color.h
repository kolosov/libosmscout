#ifndef LUM_OS_COLOR_H
#define LUM_OS_COLOR_H

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

#include <string>

#include <Lum/Private/ImportExport.h>

namespace Lum {
  namespace OS {

    typedef long unsigned Color; //! a OS color

    class LUMAPI ColorValue
    {
    private:
      double r;
      double g;
      double b;

    public:
      ColorValue();

      void SetRed(double red);
      void SetGreen(double green);
      void SetBlue(double blue);
      void SetRGB(double r, double g, double b);

      double GetRed() const;
      double GetGreen() const;
      double GetBlue() const;

      friend LUMAPI bool operator==(const ColorValue& a, const ColorValue& b);
      friend LUMAPI bool operator!=(const ColorValue& a, const ColorValue& b);
    };

    extern LUMAPI bool operator==(const ColorValue& a, const ColorValue& b);
    extern LUMAPI bool operator!=(const ColorValue& a, const ColorValue& b);

    class LUMAPI ColorRef
    {
    private:
      Color color;
      bool  allocated;

    public:
      ColorRef();
      ColorRef(const ColorRef& colorRef);
      ColorRef(const Color& color);
      ColorRef(double r, double g, double b, const Color& defaultValue);
      ColorRef(const std::string& colorName, const Color& defaultValue);

      void Allocate(const Color& color);
      void Allocate(double r, double g, double b, const Color& defaultValue);
      void Allocate(const std::string& colorName, const Color& defaultValue);

      void Assign(ColorRef& colorRef);

      bool IsDefault() const;

      ~ColorRef();

      void operator=(const ColorRef& colorRef);
      operator const Color&() const;
    };
  }
}

#endif
