#ifndef LUM_OS_WIN32_IMAGE_H
#define LUM_OS_WIN32_IMAGE_H

#include <Lum/Images/Image.h>

#include <Lum/OS/Win32/OSAPI.h>

namespace Lum {
  namespace OS {
    namespace Win32 {

      class Image : public Images::Image
      {
      private:
      HDC            dc;
      HBITMAP        bitmap;
      unsigned char* data;

      private:
        void GetBitmapHeader(BITMAPINFO &info);
        void AlphaBlend(OS::DrawInfo* draw, int x, int y, size_t w, size_t h, int dx, int dy);
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
