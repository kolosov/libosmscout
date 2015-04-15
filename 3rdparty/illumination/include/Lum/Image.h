#ifndef LUM_IMAGE_H
#define LUM_IMAGE_H

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

#include <Lum/Images/Image.h>
#include <Lum/Images/StatefulImage.h>

#include <Lum/OS/Image.h>

#include <Lum/Object.h>

namespace Lum {

  class LUMAPI Image : public Object
  {
  private:
    Images::ImageRef         extImage;
    Images::StatefulImageRef extStatefulImage;
    OS::ImageRef             osImage;

  public:
    Image();
    ~Image();

    void SetImage(Images::Image *image);
    void SetImage(Images::StatefulImage *image);
    void SetImage(OS::Image *image);

    void CalcSize();
    void Draw(OS::DrawInfo* draw,
              int x, int y, size_t w, size_t h);

    static Image* Create(Images::Image *image);
    static Image* Create(Images::StatefulImage *image);
    static Image* Create(OS::Image *image);
  };
}

#endif
