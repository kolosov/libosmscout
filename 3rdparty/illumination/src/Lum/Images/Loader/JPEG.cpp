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

#include <Lum/Images/Loader/JPEG.h>

#include <Lum/Base/String.h>

#include <unistd.h>
#include <stdio.h>
#include <jpeglib.h>
#include <setjmp.h>

namespace Lum {
  namespace Images {

    bool LoaderJPEG::CanLoad(const std::wstring& filename) const
    {
      return filename.length()>=4 && filename.substr(filename.length()-4)==L".jpg";
    }

    struct my_error_mgr
    {
      struct jpeg_error_mgr pub;
      jmp_buf               setjmp_buffer;
    };

    typedef struct my_error_mgr *my_error_ptr;

    void my_error_exit(j_common_ptr cinfo)
    {
      my_error_ptr myerr=(my_error_ptr)cinfo->err;
      (*cinfo->err->output_message)(cinfo);
      longjmp(myerr->setjmp_buffer,1);
    }

    bool LoaderJPEG::Load(const std::wstring& filename,
                          Image* image) const
    {
      std::string         name=::Lum::Base::WStringToString(filename);
      struct              jpeg_decompress_struct cinfo;
      struct my_error_mgr jerr;
      FILE                *infile;
      JSAMPARRAY          buffer;
      size_t              j;
      Data                data;

      infile = fopen(name.c_str(),"rb");
      if (infile==NULL) {
        return false;
      }

      cinfo.err=jpeg_std_error(&jerr.pub);
      jerr.pub.error_exit=my_error_exit;
      if (setjmp(jerr.setjmp_buffer)!=0) {
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        return false;
      }

      jpeg_create_decompress(&cinfo);
      jpeg_stdio_src(&cinfo,infile);
      (void)jpeg_read_header(&cinfo,TRUE);
      cinfo.quantize_colors=FALSE; // we don't want no stinking colormaps!
      cinfo.output_gamma=1/*gamma*/;
      (void)jpeg_start_decompress(&cinfo);

      data=new Pixel[cinfo.output_width*cinfo.output_height];
      buffer=(*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo,JPOOL_IMAGE,
                                        cinfo.output_width*cinfo.output_components,1);

      j=0;
      for (JDIMENSION y=0; y<cinfo.output_height; y++) {
        (void)jpeg_read_scanlines(&cinfo,buffer,1);

        for(JDIMENSION i=0; i<cinfo.output_width; i++) {
          data[j].r=buffer[0][i*3];
          data[j].g=buffer[0][i*3+1];
          data[j].b=buffer[0][i*3+2];
          data[j].a=255;

          j++;
        }
      }

      if (cinfo.output_scanline<cinfo.output_height) {
        jpeg_abort_decompress(&cinfo);
      }
      else {
        (void)jpeg_finish_decompress(&cinfo);
      }
      jpeg_destroy_decompress(&cinfo);

      fclose (infile);

      image->SetData(cinfo.output_width,
                     cinfo.output_height,
                     false,
                     data);

      return true;
    }
  }
}
