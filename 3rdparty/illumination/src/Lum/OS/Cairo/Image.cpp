#include <Lum/OS/Cairo/Image.h>

#include <Lum/OS/Cairo/DrawInfo.h>

namespace Lum {
  namespace OS {
    namespace Cairo {

      Image::Image()
      : image(NULL),data(NULL)
      {
        // no code
      }

      Image::~Image()
      {
        FreeOSStructures();
      }

      void Image::CreateOSStructures()
      {
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
        int stride = cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32,width);
#else
        int stride = width*4;
#endif
        data=new unsigned char[stride*height];

        // TODO: Handle stride offsets

        /* Premultiplied alpha values */
        if (OS::display->GetSystemByteOrder()==OS::Display::bigEndian) {
          size_t off=0;
          for (size_t s=0; s<width*height; s++) {
            data[off]=current[s].a;
            off++;
            data[off]=(current[s].r*current[s].a) / 256;
            off++;
            data[off]=(current[s].g*current[s].a) / 256;
            off++;
            data[off]=(current[s].b*current[s].a) / 256;
            off++;
          }
        }
        else {
          size_t off=0;
          for (size_t s=0; s<width*height; s++) {
            data[off]=(current[s].b*current[s].a) / 256;
            off++;
            data[off]=(current[s].g*current[s].a) / 256;
            off++;
            data[off]=(current[s].r*current[s].a) / 256;
            off++;
            data[off]=current[s].a;
            off++;
          }
        }

        image=cairo_image_surface_create_for_data(data,
                                                  CAIRO_FORMAT_ARGB32,
                                                  width,height,stride);
      }

      void Image::FreeOSStructures()
      {
        if (image!=NULL) {
          cairo_surface_destroy(image);
          image=NULL;
        }

        delete [] data;
        data=NULL;
      }

      void Image::Draw(::Lum::OS::DrawInfo* draw, int x, int y)
      {
        if (dynamic_cast<DrawInfo*>(draw)==NULL) {
          return;
        }

        if (image==NULL) {
          CreateOSStructures();
        }

        if (image!=NULL) {
          cairo_t *cairo=dynamic_cast<DrawInfo*>(draw)->cairo;

          cairo_save(cairo);
          cairo_set_source_surface(cairo,image,x,y);
          cairo_rectangle(cairo,x,y,width,height);
          cairo_fill(cairo);
          cairo_stroke(cairo);
          cairo_restore(cairo);
        }
      }

      void Image::DrawSub(::Lum::OS::DrawInfo* draw, int x, int y, size_t w, size_t h, int dx, int dy)
      {
        DrawSubCliped(draw,x,y,w,h,dx,dy);
      }

      void Image::DrawSubCliped(::Lum::OS::DrawInfo* draw, int x, int y, size_t w, size_t h, int dx, int dy)
      {
        if (w==0 || h==0 || dynamic_cast<DrawInfo*>(draw)==NULL) {
          return;
        }

        if (image==NULL) {
          CreateOSStructures();
        }

        if (image!=NULL) {
          cairo_t *cairo=dynamic_cast<DrawInfo*>(draw)->cairo;

          cairo_save(cairo);
          cairo_set_source_surface(cairo,image,dx-x,dy-y);
          cairo_rectangle(cairo,dx,dy,w,h);
          cairo_fill(cairo);
          cairo_stroke(cairo);
          cairo_restore(cairo);
        }
      }

      void Image::DrawTiled(::Lum::OS::DrawInfo* draw, int /*x*/, int /*y*/, size_t w, size_t h, int /*dx*/, int /*dy*/)
      {
        if (w==0 || h==0 || dynamic_cast<DrawInfo*>(draw)==NULL) {
          return;
        }

        if (image==NULL) {
          CreateOSStructures();
        }

        if (image!=NULL) {
          // TODO
        }
      }

      ImageFactory::ImageFactory(::Lum::OS::Display* d)
      : Factory(d)
      {
      }

      void ImageFactory::Deinit(::Lum::OS::Display* /*d*/)
      {
        // TODO
      }

      ::Lum::Images::Image* ImageFactory::CreateImage() const
      {
        return new Image();
      }
    }
  }
}
