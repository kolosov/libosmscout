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

#include <Lum/Images/Loader/GIF.h>

#include <Lum/Base/String.h>

#include <unistd.h>
#include <stdio.h>

extern "C" {
  #include <gif_lib.h>
}

namespace Lum {
  namespace Images {

    bool LoaderGIF::CanLoad(const std::wstring& filename) const
    {
      return filename.length()>=4 && filename.substr(filename.length()-4)==L".gif";
    }

    bool LoaderGIF::Load(const std::wstring& filename,
                          Image* image) const
    {
      std::string    name=::Lum::Base::WStringToString(filename);
      GifFileType    *gif;
      SavedImage*    sp;
      ColorMapObject *cmap;
      GifRowType     row_pointer;
      char           transparent = (char)-1;
      unsigned int    width=0, height=0;
      bool           alpha;
      Data           data=NULL;

      /* open the file */
      gif=DGifOpenFileName(name.c_str());

      if (gif==NULL) {
        return false;
      }

      if (DGifSlurp(gif)!=GIF_OK) {
        DGifCloseFile(gif);
        return false;
      }

      sp=gif->SavedImages;

      cmap=(sp->ImageDesc.ColorMap == NULL) ? gif->SColorMap : sp->ImageDesc.ColorMap;
      row_pointer=(GifRowType)sp->RasterBits;

      width=sp->ImageDesc.Width;
      height=sp->ImageDesc.Height;

      // look for the transparent color extension
      for (int pos=0; pos<sp->ExtensionBlockCount; ++pos) {
        ExtensionBlock* eb = sp->ExtensionBlocks + pos;
        if (eb->Function == 0xF9 && eb->ByteCount == 4) {
          if ((eb->Bytes[0] & 1) == 1) {
            transparent = eb->Bytes[3];
          }
        }
      }

      alpha=false;

      if (cmap!=NULL && row_pointer!=NULL) {

        int bg_color=gif->SBackGroundColor;

        data= new Pixel[width*height];

        if (gif->Image.Interlace) {
          /*      int group;

          for(group=0,i=0;group<4;group++) {
          for(y=startline[group];y<giftype->SHeight;y+=offset[group],i++) {
          for(x=0;x<giftype->SWidth;x++) {
          colorvalue = giftype->SavedImages->RasterBits[i*giftype->SWidth+x];
          XPutPixel(image, x, y, pixels[colorvalue]);
          }
          }
          }*/
        }
        else {
          for (size_t pos=0; pos<width*height; ++pos) {
            int c=row_pointer[pos];
            if (c==(unsigned char)transparent) {
              c=bg_color;
              alpha=true;
              data[pos].a=0;
            }
            else {
              data[pos].a=255;
            }
            data[pos].r=cmap->Colors[c].Red;
            data[pos].g=cmap->Colors[c].Green;
            data[pos].b=cmap->Colors[c].Blue;
          }
        }
      }

      DGifCloseFile(gif);

      image->SetData(width,height,alpha,data);

      return true;
    }
  }
}
