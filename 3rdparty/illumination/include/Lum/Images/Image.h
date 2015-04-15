#ifndef LUM_IMAGE_IMAGE_H
#define LUM_IMAGE_IMAGE_H

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

#include <Lum/Base/Reference.h>
#include <Lum/Base/Singleton.h>

#include <Lum/OS/DrawInfo.h>
#include <Lum/OS/Display.h>

namespace Lum {
  namespace Images {
    /**
      Holds color value information for one pixel within the
      image.
    */
    class Pixel
    {
    public:
      unsigned char r,g,b,a;
    };

    typedef Pixel* Data;

    /**
      Lowlevel representation of an image. Hold plattform
      independend data together with some OS dependend
      handles.
    */
    class LUMAPI Image : public Base::Referencable
    {
    protected:
      bool   alpha;   /** TRUE, if there is alpha channel information in the image */
      size_t oWidth;  /** The original width of the image */
      size_t oHeight; /** The original height of the image */
      size_t width;   /** The current width, may be different from oWidth because of resizing */
      size_t height;  /** The current height, may be different from oHeight because of resizing */
      Data   source;  /** The original image data */
      Data   current; /** The current data, after effects and resizing */

    private:
      Image(const Image& other);

    protected:
      virtual void FreeOSStructures() = 0;

    public:
      Image();
      virtual ~Image();

      bool HasAlpha() const;

      size_t GetOriginalWidth() const;
      size_t GetOriginalHeight() const;
      size_t GetWidth() const;
      size_t GetHeight() const;

      void SetData(size_t width, size_t height, bool alpha, Data data);
      Data GetOriginalData() const;
      Data GetData() const;

      void Resize(size_t width, size_t height);
      void InitializeEmpty();

      void Rotate90Left();
      void Rotate90Right();
      void Rotage180();
      void GreyOut(int factor);
      void ModifyRelative(int r, int g, int b, int a=0);
      void ModifyAbsolute(int r, int g, int b, int a=0);

      virtual void Draw(OS::DrawInfo* draw, int x, int y) = 0;
      /**
        Draw the sub rectangle x,y,w,h of the picture at position dx,dy.

        (This is the same as drawing the picture at dx-x,dy-y
        and clip this to the sub rectangle x,y,w,h (relative to the
        picture origin)).
      */
      virtual void DrawSub(OS::DrawInfo* draw, int x, int y, size_t w, size_t h, int dx, int dy) = 0;
      /**
        Draw the sub rectangle x,y,w,h of the picture at position dx,dy.

        (This is the same as drawing the picture at dx-x,dy-y
        and clip this to the sub rectangle x,y,w,h (relative to the
        picture origin)). You must also respect the current clipping
        rectangle!
      */
      virtual void DrawSubCliped(OS::DrawInfo* draw, int x, int y, size_t w, size_t h, int dx, int dy) = 0;
      virtual void DrawTiled(OS::DrawInfo* draw, int x, int y, size_t w, size_t h, int dx, int dy) = 0;

      Image* Clone() const;
      Image* CloneRegion(size_t x, size_t y,
                         size_t width, size_t height) const;
    };

    typedef Base::Reference<Image> ImageRef;

    class LUMAPI Factory
    {
    public:
      static Factory* factory;

    public:
      static void Set(Factory* factory);

      Factory(OS::Display* display);
      virtual ~Factory();

      virtual void Deinit(OS::Display* display) = 0;

      virtual Image* CreateImage() const = 0;
    };
  }
}

#endif
