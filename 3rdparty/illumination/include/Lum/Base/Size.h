#ifndef LUM_BASE_SIZE_H
#define LUM_BASE_SIZE_H

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

#include <Lum/Private/ImportExport.h>

#include <Lum/Config/Config.h>

#include <Lum/OS/Display.h>

namespace Lum {
  namespace Base {

    /**
      An object that hold more or less abstract size definitions and supplies
      their conrect value in display pixels.
    */
    class LUMAPI Size
    {
      public:
      enum Mode {
        pixel,        //! The size is given in pixel, you should avoid this for all costs
        mm,           //! The size in milimeter (calculated by evaluating DPI for display)
        workHRel,     //! The size is given in procent of work area size (1..100)
        workVRel,     //! The size is given in procent of work area size (1..100)
        unit,         //! The size is given in relativ units calculated evaluating the screen size
        unitP,        //! The size is given like sizeDefaultRel but in percent
        softUnit,     //! The size is given in relativ units calculated evaluating the screen size - can be zero!
        softUnitP,    //! The size is given like sizeDefaultRel but in percent - can be zero!
        stdCharWidth, //! Average character width of the given font in pixel
        stdCharHeight //! Average character height of the given font in pixel
      };

    private:
      mutable size_t value; //! the resulting size in pixel

    public:
      Mode        mode; //! the type of size given
      size_t      size; //! the amount of size in units given in mode
      OS::FontRef font; //! the font to be used for font relative calculations

    public:
      Size(Mode mode=pixel,
           size_t size=0,
           const OS::FontRef& font=OS::FontRef());
      void SetSize(Mode mode,
                   size_t size,
                   const OS::FontRef& font=OS::FontRef());
      size_t GetSize() const;
    };

    extern LUMAPI int GetSize(Size::Mode mode,
                              size_t value,
                              const OS::FontRef& font=::Lum::OS::FontRef());
  }
}

#endif
