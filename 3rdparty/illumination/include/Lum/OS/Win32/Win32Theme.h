#ifndef LUM_OS_WIN32_WIN32THEME_H
#define LUM_OS_WIN32_WIN32THEME_H

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

#include <Lum/OS/Base/Theme.h>

#include <Lum/OS/Display.h>
#include <Lum/OS/DrawInfo.h>
#include <Lum/OS/Fill.h>
#include <Lum/OS/Frame.h>
#include <Lum/OS/Image.h>

#include <Lum/OS/Win32/OSAPI.h>

namespace Lum {
  namespace OS {
    namespace Win32 {

      extern bool HasTheming();

      class ThemeImage : public OS::Image
      {
      private:
        Display::ImageIndex image;
        size_t              width;
        size_t              height;

      public:
        ThemeImage(Display::ImageIndex image);

        size_t GetWidth() const;
        size_t GetHeight() const;
        bool GetAlpha() const;
        unsigned long GetDrawCap() const;

        void Draw(OS::DrawInfo *draw, int x, int y);
        void DrawScaled(OS::DrawInfo *draw, int x, int y, size_t w, size_t h);
        void DrawStretched(OS::DrawInfo *draw, int x, int y, size_t w, size_t h);
      };

      class Fill : public OS::Fill
      {
      private:
        //HBRUSH brush;
        Display::FillIndex fill;


      public:
        Fill(Display::FillIndex fill);
        void Draw(OS::DrawInfo* draw,
                  int xOff, int yOff, size_t width, size_t height,
                  int x, int y, size_t w, size_t h);
      };

      class Frame : public OS::Frame
      {
      private:
        Display::FrameIndex frame;

      public:
        Frame(Display::FrameIndex frame);

        void Draw(OS::DrawInfo* draw, int x, int y, size_t w, size_t h);
      };

      class Win32Theme : public OS::Base::Theme
      {
      public:
        Win32Theme(OS::Display *display);
        ~Win32Theme();

        bool HasFontSettings() const;

        std::wstring GetProportionalFontName() const;
        double GetProportionalFontSize() const;

        std::wstring GetFixedFontName() const;
        double GetFixedFontSize() const;

        size_t GetSpaceHorizontal(OS::Display::Space space) const;
        size_t GetSpaceVertical(OS::Display::Space space) const;

        size_t GetIconWidth() const;
        size_t GetIconHeight() const;

        size_t GetMinimalButtonWidth() const;
        size_t GetFirstTabOffset() const;
        size_t GetLastTabOffset() const;
        size_t GetDragStartSensitivity() const;
        size_t GetMouseClickHoldSensitivity() const;

        ScrollKnobMode GetScrollKnobMode() const;
        ToolbarPosition GetToolbarPosition() const;
        bool ShowButtonImages() const;
        MenuType GetMenuType() const;
      };
    }
  }
}

#endif
