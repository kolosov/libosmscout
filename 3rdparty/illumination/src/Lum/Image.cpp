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

#include <Lum/Image.h>

namespace Lum {

  Image::Image()
  {
    // no code
  }

  Image::~Image()
  {
    // delete bitmap;
  }

  void Image::SetImage(Images::Image *image)
  {
    osImage=NULL;
    extImage=image;
    extStatefulImage=NULL;
  }

  void Image::SetImage(Images::StatefulImage *image)
  {
    osImage=NULL;
    extImage=NULL;
    extStatefulImage=image;
  }

  void Image::SetImage(OS::Image *image)
  {
    osImage=image;
    extImage=NULL;
    extStatefulImage=NULL;
  }

  void Image::CalcSize()
  {
    if (osImage.Valid()) {
      width=osImage->GetWidth();
      height=osImage->GetHeight();
    }
    else if (extImage.Valid()) {
      width=extImage->GetWidth();
      height=extImage->GetHeight();
    }
    else if (extStatefulImage.Valid() &&
             extStatefulImage->IsValid()) {
      width=extStatefulImage->GetWidth();
      height=extStatefulImage->GetHeight();
    }
    else if (OS::display->GetType()==OS::Display::typeTextual) {
      width=1;
      height=1;
    }
    else {
      width=20;
      height=20;
    }

    minWidth=width;
    minHeight=height;

    Object::CalcSize();
  }

  void Image::Draw(OS::DrawInfo* draw,
                   int x, int y, size_t w, size_t h)
  {
    Object::Draw(draw,x,y,w,h);

    if (!OIntersect(x,y,w,h)) {
      return;
    }

    /* --- */

    if (width==0 || height==0 || w==0 || h==0) {
      return;
    }

    if (osImage.Valid()) {
      osImage->DrawStretched(draw,
                             this->x,this->y,
                             HorizontalFlex() ? this->width : osImage->GetWidth(),
                             VerticalFlex() ? this->height : osImage->GetHeight());
    }
    else if (extImage.Valid()) {
      extImage->DrawSub(draw,
                        0,0,
                        extImage->GetWidth(),extImage->GetHeight(),
                        this->x+(width-extImage->GetWidth())/2,
                        this->y+(height-extImage->GetHeight())/2);
    }
    else if (extStatefulImage.Valid() &&
             extStatefulImage->IsValid()) {
      Images::ImageRef image=extStatefulImage->GetNormalImage();

      if (!draw->disabled && draw->selected && extStatefulImage->GetNormalSelectedImage()!=NULL) {
        image=extStatefulImage->GetNormalSelectedImage();
      }
      else if (draw->disabled && !draw->selected && extStatefulImage->GetDisabledImage()!=NULL) {
        image=extStatefulImage->GetDisabledImage();
      }
      else if (draw->disabled && draw->selected && extStatefulImage->GetDisabledSelectedImage()!=NULL) {
        image=extStatefulImage->GetDisabledSelectedImage();
      }
      else if (draw->activated && extStatefulImage->GetActivatedImage()!=NULL) {
        image=extStatefulImage->GetActivatedImage();
      }

      image->DrawSub(draw,
                     0,0,
                     image->GetWidth(),image->GetHeight(),
                     this->x+(width-image->GetWidth())/2,
                     this->y+(height-image->GetHeight())/2);
    }
  }

  Image* Image::Create(Images::Image *image)
  {
    Image *i;

    i=new Image();
    i->SetImage(image);

    return i;
  }

  Image* Image::Create(Images::StatefulImage *image)
  {
    Image *i;

    i=new Image();
    i->SetImage(image);

    return i;
  }

  Image* Image::Create(OS::Image *image)
  {
    Image *i;

    i=new Image();
    i->SetImage(image);

    return i;
  }

}
