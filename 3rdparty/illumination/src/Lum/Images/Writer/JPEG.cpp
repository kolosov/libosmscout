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

#include <Lum/Images/Writer/JPEG.h>

#include <cstdio>
#include <cstdlib>
#include <csetjmp>

#include <jpeglib.h>

#include <Lum/Base/String.h>

namespace Lum {
  namespace Images {

    struct my_error_mgr
    {
      struct jpeg_error_mgr pub;
      jmp_buf               setjmp_buffer;
    };

    typedef struct my_error_mgr *my_error_ptr;

    void my_writer_error_exit (j_common_ptr cinfo)
    {
      my_error_ptr  myerr = (my_error_ptr) cinfo->err;
      (*cinfo->err->output_message) (cinfo);
      longjmp (myerr->setjmp_buffer, 1);
    }

    ::Lum::Base::Status WriteJPEG(const std::wstring& filename,
                                  const ImageRef& image,
                                  const WriterOptions& options)
    {
      struct jpeg_compress_struct cinfo;
      struct my_error_mgr         jerr;
      FILE                        *outfile;
      JSAMPROW                    row_pointer[1];
      size_t                      src;
      Data                        data;
      Lum::Base::Status           status;

      outfile=fopen(Lum::Base::WStringToString(filename).c_str(),"w+b");
      if (outfile==NULL) {
        status.SetToCurrentErrno();
        return status;
      }

      cinfo.err=jpeg_std_error(&jerr.pub);
      jerr.pub.error_exit=my_writer_error_exit;
      if (setjmp(jerr.setjmp_buffer)) {
        jpeg_destroy_compress(&cinfo);
        fclose(outfile);
        status.SetError(L"setjmp returned error");
        return status;
      }

      jpeg_create_compress(&cinfo);
      jpeg_stdio_dest(&cinfo,outfile);

      cinfo.image_width=image->GetWidth();
      cinfo.image_height=image->GetHeight();
      cinfo.input_components=3;
      cinfo.in_color_space=JCS_RGB;

      jpeg_set_defaults(&cinfo);

      jpeg_set_quality(&cinfo,options.GetQuality(),TRUE);

      if ((row_pointer[0]=(JSAMPROW)malloc(image->GetWidth()*3))==NULL) {
        jpeg_destroy_compress(&cinfo);
        fclose(outfile);
        status.SetError(L"Out of memory");
        return status;
      }

      jpeg_start_compress(&cinfo,TRUE);

      src=0;
      data=image->GetData();
      while (cinfo.next_scanline<cinfo.image_height) {
        size_t dst=0;

        for (size_t x=0; x<cinfo.image_width; x++) {
          row_pointer[0][dst]=data[src].r;
          ++dst;
          row_pointer[0][dst]=data[src].g;
          ++dst;
          row_pointer[0][dst]=data[src].b;
          ++dst;

          ++src;
        }

        jpeg_write_scanlines(&cinfo,row_pointer,1);
      }

      jpeg_finish_compress(&cinfo);

      fclose(outfile);

      jpeg_destroy_compress(&cinfo);

      free(row_pointer[0]);

      return status;
    }
  }
}

