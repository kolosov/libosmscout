#ifndef LUM_OS_FRAME_H
#define LUM_OS_FRAME_H

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
      Clas abstracting the drawing of frames.
    */
    class LUMAPI Frame : public Lum::Base::Referencable
    {
    public:
      size_t topBorder,bottomBorder,leftBorder,rightBorder;
      size_t gx,gw,gh;
      size_t minWidth,minHeight;
      bool   alpha;

    public:
      Frame();
      virtual ~Frame();

      virtual void SetGap(size_t x, size_t width, size_t height);
      virtual bool HasGap() const;
      virtual void Draw(OS::DrawInfo* draw, int x, int y, size_t w, size_t h) = 0;
    };

#if defined(LUM_INSTANTIATE_TEMPLATES)
    LUM_EXPTEMPL template class LUMAPI Lum::Base::Reference<Frame>;
#endif

    typedef Lum::Base::Reference<Frame> FrameRef;

    class LUMAPI EmptyFrame : public Frame
    {
    public:
      void Draw(OS::DrawInfo* draw, int x, int y, size_t w, size_t h);
    };
  }
}

#endif
