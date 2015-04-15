#ifndef LUM_OS_BASE_THEME_H
#define LUM_OS_BASE_THEME_H

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

#include <Lum/OS/Display.h>
#include <Lum/OS/DrawInfo.h>
#include <Lum/OS/Fill.h>
#include <Lum/OS/Frame.h>
#include <Lum/OS/Image.h>
#include <Lum/OS/Theme.h>

namespace Lum {
  namespace OS {
    namespace Base {

      class Theme : public OS::Theme
      {
      protected:
        Display   *display;
        Color     color[Display::colorCount];
        FillRef   fill[Display::fillCount];
        FrameRef  frame[Display::frameCount];
        ImageRef  image[Display::imageCount];
        ImageRef  stockImage[imageCount];

        FrameRef  emptyFrame;

        Color     fillColor[3];

      public:
        Theme(OS::Display *display);
        ~Theme();

        Color GetColor(Display::ColorIndex color) const;
        Fill* GetFill(Display::FillIndex fill) const;
        Frame* GetFrame(Display::FrameIndex frame) const;
        Image* GetImage(Display::ImageIndex image) const;
        Image* GetStockImage(StockImage image) const;

        void PushUniqueFill(OS::DrawInfo* draw, size_t index, size_t maximum);
        void PopUniqueFill(OS::DrawInfo* draw, size_t index, size_t maximum);

        bool IsLightTheme() const;

        size_t GetMinimalTableRowHeight() const;

        size_t GetHorizontalFuelBarMinWidth() const;
        size_t GetHorizontalFuelBarMinHeight() const;
        size_t GetVerticalFuelBarMinWidth() const;
        size_t GetVerticalFuelBarMinHeight() const;

        size_t GetCursorWidth(OS::Font* font) const;

        int GetSubMenuHorizontalOffset() const;
        bool OptimizeMenus() const;
        size_t MaxPopupMenuEntries() const;

        unsigned long GetCursorBlinkTimeMicroSeconds() const;

        bool RequestFingerFriendlyControls() const;
        Lum::Base::Size GetFingerFriendlyMinWidth() const;
        Lum::Base::Size GetFingerFriendlyMinHeight() const;

        bool ShowKeyShortcuts() const;
        bool FocusOnEditableControlsOnly() const;

        bool SubdialogsAlwaysFlexHoriz() const;
        bool SubdialogsAlwaysFlexVert() const;

        bool PopupsAreDialogs() const;

        DialogActionPosition GetDialogActionPosition() const;
        bool ImplicitCancelAction() const;
        bool ImplicitDefaultAction() const;
      };

      class DefaultTheme : public Theme
      {
      private:
        Color fillColors[Display::fillCount];

      public:
        Color shineColor;
        Color halfShineColor;
        Color halfShadowColor;
        Color shadowColor;

      public:
        DefaultTheme(OS::Display *display);
        ~DefaultTheme();

        bool HasFontSettings() const;

        std::wstring GetProportionalFontName() const;
        double GetProportionalFontSize() const;

        std::wstring GetFixedFontName() const;
        double GetFixedFontSize() const;

        size_t GetSpaceHorizontal(Display::Space space) const;
        size_t GetSpaceVertical(Display::Space space) const;

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
