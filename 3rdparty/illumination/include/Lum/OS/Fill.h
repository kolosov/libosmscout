#ifndef LUM_OS_FILL_H
#define LUM_OS_FILL_H

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

#include <Lum/Base/Reference.h>

#include <Lum/OS/Color.h>

namespace Lum {
  namespace OS {

    class DrawInfo;

    /**
      A fill is a datastructure abstracting filling of rectangluar areas. Simple fills
      draw in a single color, but more complex fills can draw by using images or
      patterns.
    */
    class LUMAPI Fill : public Lum::Base::Referencable
    {
    public:
      size_t topBorder,bottomBorder,leftBorder,rightBorder;
      bool   transparentBorder;

    public:
      Fill();
      virtual ~Fill();

      bool HasBorder() const;

      virtual void Draw(::Lum::OS::DrawInfo* draw,
                        int xOff, int yOff, size_t width, size_t height,
                        int x, int y, size_t w, size_t h) = 0;
    };

    /**
      Draw background using a plain color.
    */
    class LUMAPI PlainFill : public Fill
    {
    private:
      Color color;

    public:
      PlainFill(Color color);

      void Draw(DrawInfo* draw,
                int xOff, int yOff, size_t width, size_t height,
                int x, int y, size_t w, size_t h);
    };

    /**
      Draw background using a plain color.
    */
    class LUMAPI EmptyFill : public Fill
    {
    public:
      void Draw(DrawInfo* draw,
                int xOff, int yOff, size_t width, size_t height,
                int x, int y, size_t w, size_t h);
    };

#if defined(LUM_INSTANTIATE_TEMPLATES)
    LUM_EXPTEMPL template class LUMAPI Lum::Base::Reference<Fill>;
#endif

    typedef Lum::Base::Reference<Fill> FillRef;
  }
}

#endif


