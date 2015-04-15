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

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>
namespace Lum {
  namespace Images {

    Factory* Factory::factory=NULL;

    Image::Image()
    : alpha(false),width(0),height(0),source(NULL),current(NULL)
    {
      // no code
    }

    Image::Image(const Image& other)
    {
      assert(false);
    }

    Image::~Image()
    {
      delete [] current;
      delete [] source;
    }

    bool Image::HasAlpha() const
    {
      return alpha;
    }

    size_t Image::GetOriginalWidth() const
    {
      return oWidth;
    }

    size_t Image::GetOriginalHeight() const
    {
      return oHeight;
    }

    size_t Image::GetWidth() const
    {
      return width;
    }

    size_t Image::GetHeight() const
    {
      return height;
    }

    Data Image::GetOriginalData() const
    {
      return source;
    }

    Data Image::GetData() const
    {
      return current;
    }

    void Image::SetData(size_t width, size_t height,
                        bool alpha,
                        Data data)
    {
      assert(data!=NULL);

      FreeOSStructures();
      delete [] source;
      delete [] current;

      oWidth=width;
      oHeight=height;
      this->width=width;
      this->height=height;

      this->alpha=alpha;

      source=data;
      current=new Pixel[width*height];
      memcpy(current,source,sizeof(Pixel)*width*height);
    }

    void Image::Resize(size_t width, size_t height)
    {
      size_t j;
      size_t xs,ys,fx,fy,xm,ym,xmi,ymi,cx,cy,p1,p2,p3,p4;

      if (source==NULL || width==0 || height==0) {
        return;
      }

      if (this->width==width && this->height==height) {
        // nothing to resize
        return;
      }

      if (this->oWidth==width && this->oHeight==height) {
        // no image scaling necessary
        delete [] current;
        current=new Pixel[width*height];
        memcpy(current,source,sizeof(Pixel)*width*height);
        return;
      }

      delete [] current;
      current=new Pixel[width*height];

      /* bilinear scaling */

      xs=(oWidth*100) / width;
      ys=(oHeight*100) / height;

      j=0;
      for (size_t y=0; y<height; y++) {
        for (size_t x=0; x<width; x++) {
          fx=(x*xs) / 100;
          fy=(y*ys) / 100;

          cx=fx+1;
          if (cx>=oWidth) {
            cx=fx;
          }

          cy=fy+1;
          if (cy>=oHeight) {
            cy=fy;
          }

          xm=x*xs-fx*100;
          xmi=100-xm;
          ym=y*ys-fy*100;
          ymi=100-ym;

          p1=fy*oWidth+fx;
          p2=fy*oWidth+cx;
          p3=cy*oWidth+fx;
          p4=cy*oWidth+cx;

          current[j].r=(ymi*(xmi*source[p1].r+xm*source[p2].r)+
                        ym*(xmi*source[p3].r+xm*source[p4].r)) / (100*100);

          current[j].g=(ymi*(xmi*source[p1].g+xm*source[p2].g)+
                        ym*(xmi*source[p3].g+xm*source[p4].g)) / (100*100);

          current[j].b=(ymi*(xmi*source[p1].b+xm*source[p2].b)+
                        ym*(xmi*source[p3].b+xm*source[p4].b)) / (100*100);

          current[j].a=(ymi*(xmi*source[p1].a+xm*source[p2].a)+
                        ym*(xmi*source[p3].a+xm*source[p4].a)) / (100*100);

          ++j;
        }
      }

      this->width=width;
      this->height=height;

      FreeOSStructures();
    }

    void Image::InitializeEmpty()
    {
      FreeOSStructures();

      delete [] current;
      delete [] source;

      oWidth=10;
      oHeight=10;
      width=10;
      height=10;
      alpha=false;

      source=new Pixel[width*height];
      current=new Pixel[width*height];

      for (size_t p=0; p<width*height; p++) {
        source[p].a=255;
        source[p].r=255;
        source[p].g=source[p].r;
        source[p].b=source[p].r;

        current[p]=source[p];
      }
    }

    /**
     Rotates the image 90 degrees to the left (clockwise).

     The operation is in place, that means that the original
     image data is lost and all further transformation are based on this
     transformation result (however since there is no loss in the
     transformation the original data can be recreated by applying
     the opposite transformation).
    */
    void Image::Rotate90Left()
    {
      Data   data;
      size_t tmp;

      FreeOSStructures();

      data=new Pixel[oHeight*oWidth];

      for (size_t y=0; y<oWidth; y++) {
        for (size_t x=0; x<oHeight; x++) {
          data[x+y*oHeight]=source[(oHeight-x-1)*oWidth+y];
        }
      }

      delete [] source;
      source=data;

      tmp=oWidth;
      oWidth=oHeight;
      oHeight=tmp;
      width=oWidth;
      height=oHeight;

      delete [] current;
      current=new Pixel[oWidth*oHeight];
      memcpy(current,source,sizeof(Pixel)*oWidth*oHeight);
    }

    /**
     Rotates the image 90 degrees to the right (anti-clockwise).

     The operation is in place, that means that the original
     image data is lost and all further transformation are based on this
     transformation result (however since there is no loss in the
     transformation the original data can be recreated by applying
     the opposite transformation).
    */
    void Image::Rotate90Right()
    {
      Data   data;
      size_t tmp;

      FreeOSStructures();

      data=new Pixel[oHeight*oWidth];

      for (size_t y=0; y<oWidth; y++) {
        for (size_t x=0; x<oHeight; x++) {
          data[x+y*oHeight]=source[x*oWidth+(oWidth-y-1)];
        }
      }

      delete [] source;
      source=data;

      tmp=oWidth;
      oWidth=oHeight;
      oHeight=tmp;
      width=oWidth;
      height=oHeight;

      delete [] current;
      current=new Pixel[oWidth*oHeight];
      memcpy(current,source,sizeof(Pixel)*oWidth*oHeight);
    }

    /**
     Rotates the image 180 degrees to the left/right.

     The operation is in place, that means that the original
     image data is lost and all further transformation are based on this
     transformation result (however since there is no loss in the
     transformation the original data can be recreated by applying
     the opposite transformation).
    */
    void Image::Rotage180()
    {
      Data data;

      FreeOSStructures();

      data=new Pixel[oHeight*oWidth];

      for (size_t y=0; y<oWidth; y++) {
        for (size_t x=0; x<oHeight; x++) {
          data[y*oWidth+x]=source[oHeight*oWidth-1-(y*oWidth+x)];
        }
      }

      delete [] source;
      source=data;

      delete [] current;
      current=new Pixel[oWidth*oHeight];
      memcpy(current,source,sizeof(Pixel)*oWidth*oHeight);
    }

    /**
      Converts all color values to their average grey value and than applies the given
      factor as absolute value to brighten (or darken) the image.
    */
    void Image::GreyOut(int factor)
    {
      FreeOSStructures();

      for (size_t p=0; p<oWidth*oHeight; p++) {
        if (!alpha || source[p].a!=0) {
          int color=(source[p].r+source[p].g+source[p].b)/3;

          if (255-factor>=color && -factor<=color) {
            color+=factor;
          }
          else if (factor<0) {
            color=0;
          }
          else if (factor>0) {
            color=255;
          }

          source[p].r=color;
          source[p].g=color;
          source[p].b=color;
        }
      }

      this->width=oWidth;
      this->height=oHeight;

      delete [] current;
      current=new Pixel[oWidth*oHeight];
      memcpy(current,source,sizeof(Pixel)*oWidth*oHeight);
    }


    void Image::ModifyRelative(int r, int g, int b, int a)
    {
      FreeOSStructures();

      alpha=false;
      for (size_t p=0; p<oWidth*oHeight; p++) {
        int tmp;

        if (r!=0) {
          tmp=(source[p].r*r)/100;
          if (255-tmp>=source[p].r && -tmp<=source[p].r) {
            source[p].r+=tmp;
          }
          else if (tmp<0) {
            source[p].r=0;
          }
          else if (tmp>0) {
            source[p].r=255;
          }
        }

        if (g!=0) {
          tmp=(source[p].g*g)/100;
          if (255-tmp>=source[p].g && -tmp<=source[p].g) {
            source[p].g+=tmp;
          }
          else if (tmp<0) {
            source[p].g=0;
          }
          else if (tmp>0) {
            source[p].g=255;
          }
        }

        if (b!=0) {
          tmp=(source[p].b*b)/100;
          if (255-tmp>=source[p].b && -tmp<=source[p].b) {
            source[p].b+=tmp;
          }
          else if (tmp<0) {
            source[p].b=0;
          }
          else if (tmp>0) {
            source[p].b=255;
          }
        }

        if (a!=0) {
          tmp=(source[p].a*a)/100;
          if (255-tmp>=source[p].a && -tmp<=source[p].a) {
            source[p].a+=tmp;
          }
          else if (tmp<0) {
            source[p].a=0;
          }
          else if (tmp>0) {
            source[p].a=255;
          }
        }

        if (source[p].a!=255) {
          alpha=true;
        }
      }

      this->width=oWidth;
      this->height=oHeight;

      delete [] current;
      current=new Pixel[oWidth*oHeight];
      memcpy(current,source,sizeof(Pixel)*oWidth*oHeight);
    }

    void Image::ModifyAbsolute(int r, int g, int b, int a)
    {
      FreeOSStructures();

      alpha=false;
      for (size_t p=0; p<oWidth*oHeight; p++) {
        if (r>0) {
          if (255-r>=source[p].r) {
            source[p].r+=r;
          }
          else {
            source[p].r=255;
          }
        }
        else if (r<0) {
          if (-r<=source[p].r) {
            source[p].r+=r;
          }
          else {
            source[p].r=0;
          }
        }

        if (g>0) {
          if (255-g>=source[p].g) {
            source[p].g+=g;
          }
          else {
            source[p].g=255;
          }
        }
        else if (g<0) {
          if (-g<=source[p].g) {
            source[p].g+=g;
          }
          else {
            source[p].g=0;
          }
        }

        if (b>0) {
          if (255-b>=source[p].b) {
            source[p].b+=b;
          }
          else {
            source[p].b=255;
          }
        }
        else if (b<0) {
          if (-b<=source[p].b) {
            source[p].b+=b;
          }
          else {
            source[p].b=0;
          }
        }

        if (a>0) {
          if (255-a>=source[p].a) {
            source[p].a+=a;
          }
          else {
            source[p].a=255;
          }
        }
        else if (a<0) {
          if (-a<=source[p].a) {
            source[p].a+=a;
          }
          else {
            source[p].a=0;
          }
        }

        if (source[p].a!=255) {
          alpha=true;
        }
      }

      this->width=oWidth;
      this->height=oHeight;

      delete [] current;
      current=new Pixel[oWidth*oHeight];
      memcpy(current,source,sizeof(Pixel)*oWidth*oHeight);
    }

    /**
      Create a deep copy of the image. "Deep" means, that the original
      image and its copy do not share any resources. Changing the copy
      does not change the original.
    */
    Image* Image::Clone() const
    {
      Image* image;
      Data   data;

      image=Factory::factory->CreateImage();
      data=new Pixel[oWidth*oHeight];
      memcpy(data,source,sizeof(Pixel)*oWidth*oHeight);
      image->SetData(oWidth,oHeight,alpha,data);

      return image;
    }

    Image* Image::CloneRegion(size_t x, size_t y,
                              size_t width, size_t height) const
    {
      Image* image;
      Data   data;
      size_t j;
      bool   alpha;

      assert((x+width<=oWidth) && (y+height<=oHeight));

      image=Factory::factory->CreateImage();
      data=new Pixel[width*height];

      j=0;
      alpha=false;
      for (size_t b=y; b<y+height; b++) {
        for (size_t a=x; a<x+width; a++) {
          data[j]=source[b*oWidth+a];
          if (data[j].a!=255) {
            alpha=true;
          }
          ++j;
        }
      }

      image->SetData(width,height,alpha,data);

      return image;
    }

    Factory::Factory(::Lum::OS::Display* /*display*/)
    {
      // no code
    }

    Factory::~Factory()
    {
      // no code
    }

    void Factory::Set(Factory* factory)
    {
      Factory::factory=factory;
    }

  }
}
