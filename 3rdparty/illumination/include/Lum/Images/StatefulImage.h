#ifndef LUM_IMAGE_STATEFULIMAGE_H
#define LUM_IMAGE_STATEFULIMAGE_H

/*
  This source is part of the Illumination library
  Copyright (C) 2009  Tim Teulings

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

#include <Lum/Images/Image.h>

namespace Lum {
  namespace Images {

    class LUMAPI StatefulImage : public Lum::Base::Referencable
    {
    private:
      ImageRef normal;
      ImageRef normalSelected;
      ImageRef disabled;
      ImageRef disabledSelected;
      ImageRef activated;

    public:
      StatefulImage();
      virtual ~StatefulImage();

      bool IsValid() const;

      size_t GetWidth() const;
      size_t GetHeight() const;

      void SetNormalImage(Image* image);
      Image* GetNormalImage() const;

      void SetNormalSelectedImage(Image* image);
      Image* GetNormalSelectedImage() const;

      void SetDisabledImage(Image* image);
      Image* GetDisabledImage() const;

      void SetDisabledSelectedImage(Image* image);
      Image* GetDisabledSelectedImage() const;

      void SetActivatedImage(Image* image);
      Image* GetActivatedImage() const;
    };

    typedef Base::Reference<StatefulImage> StatefulImageRef;
  }
}

#endif
