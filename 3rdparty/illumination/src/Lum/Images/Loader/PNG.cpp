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

#include <Lum/Images/Loader/PNG.h>

#include <Lum/Base/String.h>

#include <cstdio>
#include <cstdlib>

#include <unistd.h>
#include <png.h>

namespace Lum {
  namespace Images {

    bool LoaderPNG::CanLoad(const std::wstring& filename) const
    {
      std::string   name=::Lum::Base::WStringToString(filename);
      FILE          *file;
      unsigned char sig[8];

      /* open the file */
      file=fopen(name.c_str(),"rb");

      if (file==NULL) {
        return false;
      }

      /* check, if its a PNG */
      fread(sig,1,8,file);
      if (!png_check_sig(sig,8)) {
        fclose(file);
        return false;   /* bad signature */
      }
      else {
        fclose(file);
        return true;   /* bad signature */
      }
    }

    bool LoaderPNG::Load(const std::wstring& filename,
                         Image* image) const
    {
      std::string   name=::Lum::Base::WStringToString(filename);
      FILE          *file;
      png_structp   png_ptr;
      png_infop     info_ptr;
      png_uint_32   width, height;
      int           bit_depth, color_type;
      int           channels, intent;
      double        screen_gamma;
      png_uint_32   i,j, rowbytes;
      png_bytepp    row_pointers = NULL;
      unsigned char *image_data = NULL;
      Data          data;
      bool          alpha=false;

      /* open the file */
      file=fopen(name.c_str(),"rb");

      if (file==NULL) {
        return false;
      }

      /* could pass pointers to user-defined error handlers instead of NULLs: */

      png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
      if (!png_ptr) {
        fclose(file);
        return false;   /* out of memory */
      }

      info_ptr = png_create_info_struct(png_ptr);
      if (!info_ptr) {
        png_destroy_read_struct(&png_ptr,NULL,NULL);
        fclose(file);
        return false;   /* out of memory */
      }

      if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
        fclose(file);
        return false;
      }

      png_init_io(png_ptr,file);
      png_read_info(png_ptr,info_ptr);

      png_get_IHDR(png_ptr,info_ptr,&width,&height,&bit_depth,&color_type,
                   NULL,NULL,NULL);

      if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
        fclose(file);
        return false;
      }

      /* We always want RGB or RGBA */
      if (color_type==PNG_COLOR_TYPE_PALETTE) {
        png_set_expand(png_ptr);
      }

      if (color_type==PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
        png_set_expand(png_ptr);
      }

      if (png_get_valid(png_ptr,info_ptr,PNG_INFO_tRNS)) {
        png_set_expand(png_ptr);
      }

      if (bit_depth==16) {
        png_set_strip_16(png_ptr);
      }

      if (color_type==PNG_COLOR_TYPE_GRAY || color_type==PNG_COLOR_TYPE_GRAY_ALPHA) {
        png_set_gray_to_rgb(png_ptr);
      }

      screen_gamma=2.2; /* TODO: Make it configurable */

      if (png_get_sRGB(png_ptr,info_ptr,&intent)) {
        png_set_gamma(png_ptr,screen_gamma,0.45455);
      }
      else {
        double image_gamma;
        if (png_get_gAMA(png_ptr,info_ptr,&image_gamma)) {
          png_set_gamma(png_ptr,screen_gamma,image_gamma);
        }
        else {
          png_set_gamma(png_ptr,screen_gamma,0.45455);
        }
      }

      png_read_update_info(png_ptr, info_ptr);

      rowbytes = png_get_rowbytes(png_ptr, info_ptr);
      channels = (int)png_get_channels(png_ptr, info_ptr);

      if ((image_data = (unsigned char *)malloc(rowbytes*height)) == NULL) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(file);
        return false;
      }

      if ((row_pointers = (png_bytepp)malloc(height*sizeof(png_bytep))) == NULL) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        free(image_data);
        fclose(file);
        return false;
      }

      for (i = 0;  i < height;  ++i) {
        row_pointers[i] = image_data + i*rowbytes;
      }

      png_read_image(png_ptr,row_pointers);

      free(row_pointers);
      row_pointers = NULL;

      png_read_end(png_ptr, NULL);

      data=new Pixel[width*height];

      i=0;
      j=0;
      while (i<width*height) {
        data[i].r=image_data[j];
        j++;
        data[i].g=image_data[j];
        j++;
        data[i].b=image_data[j];
        j++;

        if (channels==4) {
          data[i].a=image_data[j];

          if (data[i].a!=255) {
            alpha=true;
          }

          j++;
        }
        else {
          data[i].a=255;
        }

        i++;
      }

      image->SetData(width,height,alpha,data);

      png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
      free(image_data);
      fclose(file);

      return true;
    }
  }
}
