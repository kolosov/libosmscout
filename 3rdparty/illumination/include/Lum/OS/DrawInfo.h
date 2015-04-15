#ifndef LUM_OS_DRAWINFO_H
#define LUM_OS_DRAWINFO_H

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

#include <Lum/Private/ImportExport.h>

#include <Lum/OS/Color.h>
#include <Lum/OS/Display.h>
#include <Lum/OS/Font.h>

namespace Lum {
  namespace OS {

    class Bitmap;
    class Window;

    /**
      Data structure holdinmg drawing primitive methods for drawing in to a window or of screen
      bitmaps.
    */
    class LUMAPI DrawInfo
    {
    public:
      enum DrawMode {
        drawModeCopy,
        drawModeInvert
      };

      enum BitmapMode {
        bitmapModeFg,
        bitmapModeFgBg
      };

      enum Pen {
        penNormal,
        penRound
      };

      enum DashMode {
        fDash,
        fbDash
      };

      struct Point
      {
        int x;
        int y;
      };

    public:
      bool selected;
      bool activated;
      bool disabled;
      bool focused;

    protected:
      DrawInfo(Window* window);
      DrawInfo(Bitmap* bitmap);

    public:
      virtual ~DrawInfo();

      virtual Window* GetWindow() const = 0;
      virtual Bitmap* GetBitmap() const = 0;

      virtual void SetNewSize(size_t width, size_t height) = 0;

      virtual void OnMap() = 0;
      virtual void OnUnmap() = 0;

      virtual void PushClip(size_t x, size_t y, size_t width, size_t height) = 0;
      virtual void PushClipBegin(size_t x, size_t y, size_t width, size_t height) = 0;
      virtual void AddClipRegion(size_t x, size_t y, size_t width, size_t height) = 0;
      virtual void SubClipRegion(size_t x, size_t y, size_t width, size_t height) = 0;
      virtual void PushClipEnd() = 0;
      virtual void PopClip() = 0;

      virtual void ReinstallClip() = 0;

      virtual void PushFont(Font *font, unsigned long style=Font::normal) = 0;
      virtual void PopFont() = 0;

      virtual void DrawString(int x, int y, const std::wstring& string) = 0;
      virtual void DrawString(int x, int y, const std::wstring& string, size_t start, size_t length) = 0;
      virtual void DrawFillString(int x, int y,
                                  const std::wstring& string,
                                  Color background) = 0;

      virtual void PushForeground(Color color) = 0;
      virtual void PushForeground(Display::ColorIndex color) = 0;
      virtual void PopForeground() = 0;

      virtual void PushDrawMode(DrawMode mode) = 0;
      virtual void PopDrawMode() = 0;

      virtual void PushPen(int size, Pen pen=penNormal) = 0;
      virtual void PopPen() = 0;

      virtual void PushDash(const char* dashList, size_t len, DashMode mode) = 0;
      virtual void PopDash() = 0;

      virtual void PushPattern(const char *pattern, int width, int height, int mode) = 0;
      virtual void PopPattern() = 0;

      virtual void PushBitmap(Bitmap* bitmap, BitmapMode mode) = 0;
      virtual void PopBitmap() = 0;

      virtual void PushUniqueFill(size_t index, size_t maximum);
      virtual void PopUniqueFill(size_t index, size_t maximum);

      virtual void DrawPoint(int x, int y) = 0;
      virtual void DrawPointWithColor(int x, int y, Color color);

      virtual void DrawLine(int x1, int y1, int x2, int y2) = 0;

      virtual void DrawRectangle(int x, int y, int width, int height) = 0;
      virtual void FillRectangle(int x, int y, int width, int height) = 0;

      virtual void DrawArc(int x, int y, int width, int height, int angle1, int angle2) = 0;
      virtual void FillArc(int x, int y, int width, int height, int angle1, int angle2) = 0;

      virtual void DrawPolygon(const Point points[], size_t count) = 0;
      virtual void FillPolygon(const Point points[], size_t count) = 0;

      virtual void FillBackground(int x, int y, int width, int height);

      virtual void CopyArea(int sX, int sY, int width, int height, int dX, int dY) = 0;
      virtual void CopyFromBitmap(Bitmap* bitmap, int sX, int sY, int width, int height, int dX, int dY) = 0;
      virtual void CopyToBitmap(int sX, int sY, int width, int height, int dX, int dY, Bitmap* bitmap) = 0;
    };
  }
}

#endif
