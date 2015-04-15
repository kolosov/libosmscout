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

#include <Lum/Images/Loader/TIFF.h>

#include <Lum/Base/String.h>

#include <unistd.h>
#include <stdio.h>
#include <tiffio.h>

namespace Lum {
  namespace Images {

    bool LoaderTIFF::CanLoad(const std::wstring& filename) const
    {
      return filename.length()>=4 && filename.substr(filename.length()-4)==L".tif";
    }

    bool LoaderTIFF::Load(const std::wstring& filename,
                          Image* image) const
    {
      std::string name=::Lum::Base::WStringToString(filename);
      TIFF        *tif;
      uint32      width,height;
      uint32      *imageData;
      bool        alpha=false;
      Data        data=NULL;

      if ((tif=TIFFOpen(name.c_str(),"r"))==NULL) {
        return false;
      }

      TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
      TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
      if ((imageData=(uint32*)_TIFFmalloc(width*height*sizeof(uint32)))!=NULL) {

        data=new Pixel[width*height];

        if (TIFFReadRGBAImage(tif,width,height,imageData,0)) {
          unsigned long   pos;
          register uint32 *row = imageData;
          int             y = height;

          pos=width*height-1;
          while (--y>=0) {
            int x;

            for (x=width-1; x>=0 ; --x) {
              uint32 c = row[x];

              data[pos].a=(c>>24)&0x00FF;
              data[pos].r=(c    )&0x00FF;
              data[pos].g=(c>>8 )&0x00FF;
              data[pos].b=(c>>16)&0x00FF;

              if (data[pos].a!=255) {
                alpha=true;
              }

              pos--;
            }

            row+=width;
          }
        }
        _TIFFfree(imageData);
      }

      TIFFClose(tif);

      image->SetData(width,height,alpha,data);

      return true;
    }
  }
}
