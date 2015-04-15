#ifndef LUM_OS_IMAGE_H
#define LUM_OS_IMAGE_H

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

#include <Lum/Base/Reference.h>

namespace Lum {
  namespace OS {

    class DrawInfo;

    /**
      Class abstracting the drawing of predefined images. Predefined images can be
      implemented by using drawing primitive or external images.
    */
    class LUMAPI Image : public Lum::Base::Referencable
    {
    public:
      enum Capabilities
      {
        drawsDisabled  = 1 << 0,
        drawsFocused   = 1 << 1,
        drawsActivated = 1 << 2
      };

      enum Mode {
        normal    = 0,
        selected  = 1,
        activated = 2
      };

    public:
      virtual ~Image();
      virtual size_t GetWidth() const = 0;
      virtual size_t GetHeight() const = 0;
      virtual bool GetAlpha() const = 0;
      virtual unsigned long GetDrawCap() const = 0;

      virtual void Draw(OS::DrawInfo *draw, int x, int y) = 0;
      virtual void DrawScaled(OS::DrawInfo *draw, int x, int y, size_t w, size_t h) = 0;
      virtual void DrawStretched(OS::DrawInfo *draw, int x, int y, size_t w, size_t h) = 0;
    };

#if defined(LUM_INSTANTIATE_TEMPLATES)
    LUM_EXPTEMPL template class LUMAPI Lum::Base::Reference<Image>;
#endif

    typedef Lum::Base::Reference<Image> ImageRef;

    class EmptyImage : public Image
    {
    public:
      virtual size_t GetWidth() const;
      virtual size_t GetHeight() const;
      virtual bool GetAlpha() const;
      virtual unsigned long GetDrawCap() const;

      virtual void Draw(OS::DrawInfo *draw, int x, int y);
      virtual void DrawScaled(OS::DrawInfo *draw, int x, int y, size_t w, size_t h);
      virtual void DrawStretched(OS::DrawInfo *draw, int x, int y, size_t w, size_t h);
    };
  }
}

#endif
