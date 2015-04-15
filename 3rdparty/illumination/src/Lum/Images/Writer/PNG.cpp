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

#include <Lum/Images/Writer/PNG.h>

#include <cstdio>
#include <cstdlib>

#include <png.h>

#include <Lum/Base/String.h>

namespace Lum {
  namespace Images {
    ::Lum::Base::Status WritePNG(const std::wstring& filename,
                                 const ImageRef& image,
                                 const WriterOptions& options)
    {
      FILE              *file;
      png_structp       png_ptr;
      png_infop         info_ptr;
      png_uint_32       i;
      png_bytepp        row_pointers=NULL;
      png_time          pngTime;
      Lum::Base::Status status;

      /* open the file */
      file=fopen(Lum::Base::WStringToString(filename).c_str(),"w+b");

      if (file==NULL) {
        status.SetToCurrentErrno();
        return status;
      }

      /* could pass pointers to user-defined error handlers instead of NULLs: */
      png_ptr=png_create_write_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
      if (!png_ptr) {
        fclose(file);
        status.SetError(L"png_create_write_struct returned error");
        return status;
      }

      info_ptr=png_create_info_struct(png_ptr);
      if (!info_ptr) {
        png_destroy_write_struct(&png_ptr,NULL);
        fclose(file);
        status.SetError(L"png_create_info_struct returned error");
        return status;
      }

      if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr,&info_ptr);
        fclose(file);
        status.SetError(L"setjmp returned error");
        return status;
      }

      /* allocate memory for row pointer vector */
      if ((row_pointers=(png_bytepp)malloc(image->GetHeight()*sizeof(png_bytep)))==NULL) {
        png_destroy_write_struct(&png_ptr,&info_ptr);
        fclose(file);
        status.SetError(L"Out of memory");
        return status;
      }

      /* build up array of row pointers */
      for (i=0; i<image->GetHeight(); ++i) {
        row_pointers[i]=(png_byte*)image->GetData()+i*4*image->GetWidth();
      }

      png_init_io(png_ptr,file);

      png_set_IHDR(png_ptr,info_ptr,
                   image->GetWidth(),image->GetHeight(),
                   8,PNG_COLOR_TYPE_RGB_ALPHA,
                   PNG_INTERLACE_NONE,
                   PNG_COMPRESSION_TYPE_DEFAULT,
                   PNG_FILTER_TYPE_DEFAULT);

/*  if (!image->alpha) {*/
/*    png_set_filler(png_ptr,0,PNG_FILLER_AFTER);*/
/*  }*/

      /* assign data */
      png_set_rows(png_ptr,info_ptr,row_pointers);

      /* Write the current time in a sepcial 'time of creation' chunk */
      png_convert_from_time_t(&pngTime,time(NULL));
      png_set_tIME(png_ptr,info_ptr,&pngTime);

      /*
      TODO: fill in more headers
      */

      png_write_png(png_ptr,info_ptr,0,NULL);
      png_write_end(png_ptr,info_ptr);

      png_destroy_write_struct(&png_ptr,&info_ptr);
      free(row_pointers);
      fclose(file);

      return status;
    }
  }
}

