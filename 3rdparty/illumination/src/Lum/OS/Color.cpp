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

#include <Lum/OS/Color.h>

#include <cassert>

#include <Lum/OS/Display.h>

namespace Lum {
  namespace OS {

    ColorValue::ColorValue()
    : r(0),g(0),b(0)
    {
      // no code
    }

    void ColorValue::SetRed(double red)
    {
      assert(red>=0 && red<=1);

      r=red;
    }

    void ColorValue::SetGreen(double green)
    {
      assert(green>=0 && green<=1);

      g=green;
    }

    void ColorValue::SetBlue(double blue)
    {
      assert(blue>=0 && blue<=1);

      b=blue;
    }

    void ColorValue::SetRGB(double r, double g, double b)
    {
      assert(r>=0 && r<=1);
      assert(g>=0 && g<=1);
      assert(b>=0 && b<=1);

      this->r=r;
      this->g=g;
      this->b=b;
    }

    double ColorValue::GetRed() const
    {
      return r;
    }

    double ColorValue::GetGreen() const
    {
      return g;
    }

    double ColorValue::GetBlue() const
    {
      return b;
    }

    bool operator==(const ColorValue& a, const ColorValue& b)
    {
      return a.GetRed()==b.GetRed() && a.GetGreen()==b.GetGreen() && a.GetBlue()==b.GetBlue();
    }

    bool operator!=(const ColorValue& a, const ColorValue& b)
    {
      return a.GetRed()!=b.GetRed() || a.GetGreen()!=b.GetGreen() || a.GetBlue()!=b.GetBlue();
    }

    ColorRef::ColorRef()
    {
      allocated=false;
      color=display->GetColor(OS::Display::backgroundColor);
    }

    ColorRef::ColorRef(const ColorRef& colorRef)
    {
      allocated=false;
      color=colorRef.color;
    }

    ColorRef::ColorRef(const Color& color)
    {
      allocated=false;
      this->color=color;
    }

    ColorRef::ColorRef(double r, double g, double b, const Color& defaultValue)
    {
      if (display->AllocateColor(r,g,b,color)) {
        allocated=true;
      }
      else {
        allocated=false;
        color=defaultValue;
      }
    }

    ColorRef::ColorRef(const std::string& colorName, const Color& defaultValue)
    {
      if (display->AllocateNamedColor(colorName,color)) {
        allocated=true;
      }
      else {
        allocated=false;
        color=defaultValue;
      }
    }

    ColorRef::~ColorRef()
    {
      if (allocated) {
        display->FreeColor(color);
      }
    }

    void ColorRef::Allocate(const Color& color)
    {
      if (allocated) {
        display->FreeColor(this->color);
      }

      allocated=false;
      this->color=color;
    }

    void ColorRef::Allocate(double r, double g, double b, const Color& defaultValue)
    {
      if (allocated) {
        display->FreeColor(color);
      }

      if (display->AllocateColor(r,g,b,color)) {
        allocated=true;
      }
      else {
        allocated=false;
        color=defaultValue;
      }
    }

    void ColorRef::Allocate(const std::string& colorName, const Color& defaultValue)
    {
      if (allocated) {
        display->FreeColor(color);
      }

      if (display->AllocateNamedColor(colorName,color)) {
        allocated=true;
      }
      else {
        allocated=false;
        color=defaultValue;
      }
    }

    void ColorRef::Assign(ColorRef& colorRef)
    {
       if (allocated) {
        display->FreeColor(color);
      }

      color=colorRef.color;
      allocated=colorRef.allocated;

      if (colorRef.allocated) {
        colorRef.allocated=false;
        colorRef.color=display->GetColor(OS::Display::backgroundColor);
      }
    }

    bool ColorRef::IsDefault() const
    {
      return !allocated;
    }

    void ColorRef::operator=(const ColorRef& colorRef)
    {
      if (allocated) {
        display->FreeColor(color);
      }

      allocated=false;
      color=colorRef.color;
    }

    ColorRef::operator const Color&() const
    {
      return color;
    }
  }
}
