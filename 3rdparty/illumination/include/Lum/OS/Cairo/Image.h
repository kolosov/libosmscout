#ifndef LUM_OS_CAIRO_IMAGE_H
#define LUM_OS_CAIRO_IMAGE_H

#include <Lum/Private/Config.h>

#include <Lum/Images/Image.h>

// Cairo
#if defined(__WIN32__) || defined(WIN32) || defined(__APPLE__)
#include <cairo.h>
#else
#include <cairo/cairo.h>
#endif

#if defined(__WIN32__) || defined(WIN32)
#include <cairo-win32.h>
#endif

#if defined(__APPLE__)
#include <cairo-quartz.h>
#endif

#if !defined(X_DISPLAY_MISSING) && !defined(__APPLE__)
#include <cairo-xlib.h>
#endif

namespace Lum {
  namespace OS {
    namespace Cairo {

      class Image : public Images::Image
      {
      private:
        cairo_surface_t *image;
        unsigned char   *data;

      private:
        void CreateOSStructures();


      public:
        Image();
        ~Image();

        void FreeOSStructures();

        void Draw(OS::DrawInfo* draw, int x, int y);
        void DrawSub(OS::DrawInfo* draw, int x, int y, size_t w, size_t h, int dx, int dy);
        void DrawSubCliped(OS::DrawInfo* draw, int x, int y, size_t w, size_t h, int dx, int dy);
        void DrawTiled(OS::DrawInfo* draw, int x, int y, size_t w, size_t h, int dx, int dy);
      };

      class ImageFactory : public Images::Factory
      {
      public:
        ImageFactory(OS::Display* display);

        void Deinit(OS::Display* display);

        Images::Image* CreateImage() const;
      };
    }
  }
}

#endif
