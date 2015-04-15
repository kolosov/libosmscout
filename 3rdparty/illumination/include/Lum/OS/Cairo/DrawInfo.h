#ifndef LUM_OS_CAIRO_DRAWINFO_H
#define LUM_OS_CAIRO_DRAWINFO_H

/*
  This source is part of the Illumination library
  Copyright (C) 2007  Tim Teulings

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

// std
#include <stack>

#include <Lum/Features.h>

// Cairo
#if defined(__WIN32__) || defined(WIN32) || defined(__APPLE__)
  #include <cairo.h>
#else
  #include <cairo/cairo.h>
#endif

#if defined(__WIN32__) || defined(WIN32)
  #include <cairo-win32.h>
  #undef CreateWindow
  #undef CreateFont
#endif

#if defined(__APPLE__)
  #include <cairo-quartz.h>
#endif

#if defined(LUM_HAVE_LIB_X)
  #include <cairo-xlib.h>
#endif

#include <Lum/OS/Base/DrawInfo.h>

namespace Lum {
  namespace OS {
    namespace Cairo {

      class LUMAPI DrawInfo : public OS::Base::DrawInfo
      {
      private:
        class FontEntry
        {
        public:
          OS::FontRef   font;
          unsigned long style;
          int           count;
        };

      public:
        ::cairo_t             *cairo;
        ::cairo_surface_t     *surface;
        bool                  deleteSurface;
        size_t                clipPushCount;
        std::stack<FontEntry> fontStack;
        size_t                colorPushCount;
        size_t                dashPushCount;
/*
#if defined(__APPLE__)
        CGContextRef       context;
#endif*/
      protected:
        void SetDefaults();

      public:
        DrawInfo(OS::Window* window);
        DrawInfo(OS::Bitmap* bitmap);
        virtual ~DrawInfo();

        void PushClip(size_t x, size_t y, size_t width, size_t height);
        void PushClipBegin(size_t x, size_t y, size_t width, size_t height);
        void AddClipRegion(size_t x, size_t y, size_t width, size_t height);
        void SubClipRegion(size_t x, size_t y, size_t width, size_t height);
        void PushClipEnd();
        void PopClip();

        void ReinstallClip();

        void PushFont(::Lum::OS::Font* font, unsigned long style);
        void PopFont();

        void DrawString(int x, int y, const std::wstring& text, size_t start, size_t length);
        void DrawFillString(int x, int y, const std::wstring& text,
                            OS::Color background);

        void PushForeground(OS::Color color);
        void PopForeground();

        void PushDrawMode(OS::DrawInfo::DrawMode mode);
        void PopDrawMode();

        void PushPen(int size, Pen pen);
        void PopPen();

        void PushDash(const char* dashList, size_t len, DashMode mode);
        void PopDash();

        void PushPattern(const char* pattern, int width, int height, int mode);
        void PopPattern();

        void PushBitmap(OS::Bitmap* bitmap, BitmapMode mode);
        void PopBitmap();

        void DrawPoint(int x, int y);
        void DrawPointWithColor(int x, int y, ::Lum::OS::Color color);

        void DrawLine(int x1, int y1, int x2, int y2);

        void DrawRectangle(int x, int y, int width, int height);
        void FillRectangle(int x, int y, int width, int height);

        void DrawArc(int x, int y, int width, int height, int angle1, int angle2);
        void FillArc(int x, int y, int width, int height, int angle1, int angle2);

        void FillPolygon(const Point points[], size_t count);

        void CopyArea(int sX, int sY, int width, int height, int dX, int dY);

        void CopyFromBitmap(OS::Bitmap* bitmap, int sX, int sY, int width, int height, int dX, int dY);
        void CopyToBitmap(int sX, int sY, int width, int height, int dX, int dY, OS::Bitmap* bitmap);
      };

#if defined(CAIRO_HAS_WIN32_SURFACE)
      class LUMAPI Win32DrawInfo : public DrawInfo
      {
      public:
        HDC dc;

      protected:
        void GetRGBValue(Color color, double &r, double &g, double &b);

      public:
        Win32DrawInfo(OS::Window* window);
        Win32DrawInfo(OS::Window* window, HDC dc);
        Win32DrawInfo(OS::Bitmap* bitmap);

        ~Win32DrawInfo();
      };
#endif

#if defined(CAIRO_HAS_XLIB_SURFACE)
      class LUMAPI X11DrawInfo : public DrawInfo
      {
      public:
        ::Display *display;
        ::Window  drawable;

      public:
        X11DrawInfo(OS::Window* window);
        X11DrawInfo(OS::Bitmap* bitmap);

        ~X11DrawInfo();

        void SetNewSize(size_t width, size_t height);
      };
#endif
    }
  }
}

#endif
