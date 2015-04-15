#ifndef LUM_OS_CARBON_DRAWINFO_H
#define LUM_OS_CARBON_DRAWINFO_H

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

// std
#include <stack>
#include <string>
#include <vector>

#include <Lum/Private/Config.h>

// Carbon
#include <Carbon/Carbon.h>

#include <Lum/OS/Base/Display.h>
#include <Lum/OS/Base/DrawInfo.h>
#include <Lum/OS/Base/Font.h>
#include <Lum/OS/Base/Window.h>

namespace Lum {
  namespace OS {
    namespace Carbon {

      class Bitmap;
      typedef Bitmap *BitmapPtr;

      class Window;
      typedef Window *WindowPtr;

      class Display;
      typedef Display *DisplayPtr;


      class LUMAPI DrawInfo : public ::Lum::OS::Base::DrawInfo
      {
      private:

        class PenColor
        {
        public:
          ::Lum::OS::Color color;
          int count;
        };

        class PenStyle
        {
        public:
          int size;
          Pen pen;
          int count;
        };

        class DrawMode
        {
        public:
          int mode;
        };

        class DashEntry
        {
        public:
          ::Pattern   pattern;
          const char* dash;
          size_t      len;
          DashMode    mode;
        };

        class FontEntry
        {
        public:
          ::Lum::OS::FontRef font;
          unsigned long      style;
          int                count;
        };

        class ClipEntry
        {
        public:
          ::RgnHandle region;
          DrawInfo    *draw;

          void Init(DrawInfo *draw);
          void Free();

          void Add(int x, int y, int width, int height);
          void Sub(int x, int y, int width, int height);
        };

        class PatternEntry
        {
        public:
          ::Pattern pattern;
          //Pixmap pixmap;
          //BitmapMode mode;
        };

      private:
        std::stack<PenColor>     fPenStack;
        std::stack<PenStyle>     penStack;
        std::stack<DrawMode>     modeStack;
        std::vector<ClipEntry>   clipStack;
        std::stack<FontEntry>    fontStack;
        std::stack<DashEntry>    dashStack;
        std::stack<PatternEntry> patternStack;
        //::XPoint                 pointArray[256];

      public:
        CGrafPtr port;

      private:
        void RecalcClipRegion();
        void SetForeground(::Lum::OS::Color color);

      public:
        DrawInfo(::Lum::OS::WindowPtr window);
        DrawInfo(::Lum::OS::BitmapPtr window);
        virtual ~DrawInfo();

        void PushClip(size_t x, size_t y, size_t width, size_t height);
        void PushClipBegin(size_t x, size_t y, size_t width, size_t height);
        void AddClipRegion(size_t x, size_t y, size_t width, size_t height);
        void SubClipRegion(size_t x, size_t y, size_t width, size_t height);
        void PushClipEnd();
        void PopClip();

        void ReinstallClip();

        void PushFont(const ::Lum::OS::FontPtr font, unsigned long style);
        void PopFont();

        void DrawString(int x, int y, const std::wstring& text, size_t start, size_t length);
        void DrawFillString(int x, int y,
                            const std::wstring& text,
                            ::Lum::OS::Color background);

        void PushForeground(::Lum::OS::Color color);
        void PopForeground();

        void PushDrawMode(::Lum::OS::DrawInfo::DrawMode mode);
        void PopDrawMode();

        void PushPen(int size, Pen pen);
        void PopPen();

        void PushDash(const char* dashList, size_t len, DashMode mode);
        void PopDash();

        void PushPattern(const char* pattern, int width, int height, int mode);
        void PopPattern();

        void PushBitmap(::Lum::OS::BitmapPtr bitmap, BitmapMode mode);
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

        void CopyFromBitmap(::Lum::OS::BitmapPtr bitmap, int sX, int sY, int width, int height, int dX, int dY);
        void CopyToBitmap(int sX, int sY, int width, int height, int dX, int dY, ::Lum::OS::BitmapPtr bitmap);
      };

      typedef DrawInfo *DrawInfoPtr;
    }
  }
}

#endif
