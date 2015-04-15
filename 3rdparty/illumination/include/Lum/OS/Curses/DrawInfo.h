#ifndef LUM_OS_X11_DRAWINFO_H
#define LUM_OS_X11_DRAWINFO_H

/*
  This source is part of the Illumination library
  Copyright (C) 2005  Tim Teulings

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

// Curses
#include <curses.h>

#include <Lum/OS/Base/Display.h>
#include <Lum/OS/Base/DrawInfo.h>
#include <Lum/OS/Base/Font.h>
#include <Lum/OS/Base/Window.h>

#include <Lum/Private/Config.h>

namespace Lum {
  namespace OS {
    namespace Curses {

      class Bitmap;
      class Window;
      class Display;

      class LUMAPI DrawInfo : public OS::Base::DrawInfo
      {
      private:

        class PenColor
        {
        public:
          OS::Color color;
          int       count;
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
          const char* dash;
          size_t      len;
          DashMode    mode;
        };

        class FontEntry
        {
        public:
          OS::FontRef   font;
          unsigned long style;
          int           count;
        };

        class ClipEntry
        {
        public:
          std::vector<bool> region;
          DrawInfo          *draw;

          void Init(DrawInfo *draw);
          void Free();

          void Add(int x, int y, int width, int height);
          void Sub(int x, int y, int width, int height);
        };

        class PatternEntry
        {
        public:
          //Pixmap pixmap;
          BitmapMode mode;
        };

      private:
        std::stack<PenColor>     fPenStack;
        std::stack<PenStyle>     penStack;
        std::stack<DrawMode>     modeStack;
        std::vector<ClipEntry>   clipStack;
        std::stack<FontEntry>    fontStack;
        std::stack<DashEntry>    dashStack;
        std::stack<PatternEntry> patternStack;

        std::vector<bool>        clip;

      public:
        ::WINDOW   *window;
        size_t     width;
        size_t     height;

      private:
        void RecalcClipRegion();

        int GetBackground(int x, int y) const;
        void DrawChar(int x, int y, char character);
        void DrawDirect(int x, int y, int character);
        void Fill(int x, int y);

      public:
        DrawInfo(OS::Window* window);
        DrawInfo(OS::Bitmap* window);
        virtual ~DrawInfo();

        void SetNewSize(size_t width, size_t height);

        void PushClip(size_t x, size_t y, size_t width, size_t height);
        void PushClipBegin(size_t x, size_t y, size_t width, size_t height);
        void AddClipRegion(size_t x, size_t y, size_t width, size_t height);
        void SubClipRegion(size_t x, size_t y, size_t width, size_t height);
        void PushClipEnd();
        void PopClip();

        void ReinstallClip();

        void PushFont(OS::Font *font, unsigned long style);
        void PopFont();

        void DrawString(int x, int y, const std::wstring& text, size_t start, size_t length);
        void DrawFillString(int x, int y,
                            const std::wstring& text,
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
        void DrawPointWithColor(int x, int y, OS::Color color);

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
    }
  }
}

#endif
