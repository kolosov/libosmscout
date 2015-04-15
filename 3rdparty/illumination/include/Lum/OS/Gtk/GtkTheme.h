#ifndef LUM_OS_X11_GTKTHEME_H
#define LUM_OS_X11_GTKTHEME_H

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

#include <gtk/gtk.h>

#include <Lum/OS/Base/Theme.h>

#include <Lum/OS/Display.h>
#include <Lum/OS/DrawInfo.h>
#include <Lum/OS/Fill.h>
#include <Lum/OS/Frame.h>
#include <Lum/OS/Image.h>

namespace Lum {
  namespace OS {
    namespace Gtk {

      struct GtkWidgets
      {
        enum GtkStyles {
          styleWindow       =  0,
          styleColumn       =  1,
          styleColumnLeft   =  2,
          styleColumnMiddle =  3,
          styleColumnRight  =  4,

          styleButton       =  5,
          styleToolbar      =  6,
          styleToolButton   =  7,
          styleEdit         =  8,
          styleProgressH    =  9,
          styleProgressV    = 10,
          styleHKnob        = 11,
          styleVKnob        = 12,
          styleCheck        = 13,
          styleRadio        = 14,
          styleRadio2       = 15,
          styleTab          = 16,
          styleList         = 17,
          styleComboBox     = 18,
          styleEditComboBox = 19,
          styleHLine        = 20,
          styleVLine        = 21,
          styleHScale       = 22,
          styleVScale       = 23,
          styleStatus       = 24,
          styleScrolled     = 25,
          styleLabel        = 26,
          styleButtonBox    = 27,
          stylePane         = 28,
          styleToggleButton = 29,
          styleDummy        = 30,

          styleMenuBar      = 31,
          styleMenuPullDown = 32,

          styleMax          = 33
        };

      public:
        ::GtkWidget     *layout;
        ::GtkAdjustment *adjustment;
        ::GtkWidget     *widget[styleMax];

      public:
        GtkWidgets();
        ~GtkWidgets();

        GtkWidget* GetWidget(GtkStyles style) const;
        GtkStyle* GetStyle(GtkStyles style) const;
      };

      extern GtkWidgets *gtkWidgets;

      class GtkImage : public OS::Image
      {
      private:
        Display::ImageIndex image;
        size_t              width;
        size_t              height;
        GtkWidget           *widget;

      public:
        GtkImage(Display::ImageIndex image,
                 GtkWidget* widget=NULL);

        size_t GetWidth() const;
        size_t GetHeight() const;
        bool GetAlpha() const;
        unsigned long GetDrawCap() const;

        void Draw(OS::DrawInfo *draw, int x, int y);
        void DrawScaled(OS::DrawInfo *draw, int x, int y, size_t w, size_t h);
        void DrawStretched(OS::DrawInfo *draw, int x, int y, size_t w, size_t h);
      };

      class GtkStockImage : public OS::Image
      {
      private:
        const gchar *gtkStockId;
        GtkIconSet  *gtkIconSet;
        GdkPixbuf   *gdkPixbufNormal;
        GdkPixbuf   *gdkPixbufDisabled;
        GdkPixbuf   *gdkPixbufSelected;
        GdkPixbuf   *gdkPixbufActivated;
        size_t      width;
        size_t      height;

      public:
        GtkStockImage(const gchar* gtkStockId);
        ~GtkStockImage();

        size_t GetWidth() const;
        size_t GetHeight() const;
        bool GetAlpha() const;
        unsigned long GetDrawCap() const;

        void Draw(OS::DrawInfo *draw, int x, int y);
        void DrawScaled(OS::DrawInfo *draw, int x, int y, size_t w, size_t h);
        void DrawStretched(OS::DrawInfo *draw, int x, int y, size_t w, size_t h);
      };

      class GtkFill : public Fill
      {
      private:
        Display::FillIndex fill;

      public:
        GtkFill(Display::FillIndex fill);

        void Draw(OS::DrawInfo* draw,
                  int xOff, int yOff, size_t width, size_t height,
                  int x, int y, size_t w, size_t h);
      };

      class GtkFrame : public Frame
      {
      private:
        Display::FrameIndex frame;

      public:
        GtkFrame(Display::FrameIndex frame);

        void Draw(OS::DrawInfo* draw, int x, int y, size_t w, size_t h);
      };

      extern bool GetWidgetStyleBoolValue(GtkWidget *widget, const char* name);
      extern int GetWidgetStyleIntValue(GtkWidget *widget, const char* name);
      extern float GetWidgetStyleFloatValue(GtkWidget *widget, const char* name);
      extern void GetWidgetStyleBorderValue(GtkWidget *widget, const char* name,
                                            int& left, int&right, int &top, int& bottom);

      class GtkTheme : public OS::Base::Theme
      {
      public:
        Color          helpBackgroundColor;
        Color          whiteColor;
        Color          blackColor;
        Color          shineColor;
        Color          shadowColor;

        ScrollKnobMode scrollKnobMode;

        GtkWidget      *menu;
        GtkWidget      *menuItem;
        GtkWidget      *popup;
        GtkWidget      *window;
        GtkWidget      *layout;

        FillRef        appMenuBackground;

      public:
        GtkTheme(OS::Display *display);
        ~GtkTheme();

        bool IsLightTheme() const;

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

        size_t GetMinimalTableRowHeight() const;

        size_t GetHorizontalFuelBarMinWidth() const;
        size_t GetHorizontalFuelBarMinHeight() const;
        size_t GetVerticalFuelBarMinWidth() const;
        size_t GetVerticalFuelBarMinHeight() const;

        size_t GetDragStartSensitivity() const;

        size_t GetMouseClickHoldSensitivity() const;

        unsigned long GetCursorBlinkTimeMicroSeconds() const;

        ScrollKnobMode GetScrollKnobMode() const;
        ToolbarPosition GetToolbarPosition() const;
        bool ShowButtonImages() const;
        bool RequestFingerFriendlyControls() const;

        Lum::Base::Size GetFingerFriendlyMinWidth() const;
        Lum::Base::Size GetFingerFriendlyMinHeight() const;

        bool ShowKeyShortcuts() const;
        bool FocusOnEditableControlsOnly() const;

        MenuType GetMenuType() const;
        void GetMenuTypeActionPopupOffset(int& x, int& y) const;
        int GetSubMenuHorizontalOffset() const;
        bool OptimizeMenus() const;

        bool SubdialogsAlwaysFlexHoriz() const;

        bool PopupsAreDialogs() const;

        DialogActionPosition GetDialogActionPosition() const;
        bool ImplicitCancelAction() const;
        bool ImplicitDefaultAction() const;
      };
    }
  }
}

#endif
