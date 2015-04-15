#ifndef LUM_OS_THEME_H
#define LUM_OS_THEME_H

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

#include <set>
#include <vector>

#include <Lum/Private/ImportExport.h>

#include <Lum/Base/Model.h>
#include <Lum/Base/Size.h>

#include <Lum/Def/Menu.h>

#include <Lum/OS/Color.h>
#include <Lum/OS/Display.h>
#include <Lum/OS/DrawInfo.h>
#include <Lum/OS/Fill.h>
#include <Lum/OS/Frame.h>
#include <Lum/OS/Image.h>

namespace Lum {
  namespace OS {

    /**
      A abstract base class for holding all information for theming a user interface.
    */
    class LUMAPI Theme
    {
    public:
      /**
        List of (optional!) images for enriching visual controls.
       */
      enum StockImage {
        imagePositive    =  0, //! Image for marking a positive action (e.g. for a button)
        imageNegative    =  1, //! Image for marking a negative action (e.g. for a button)
        imageDefault     =  2, //! Image for marking a default action (e.g. for a button)
        imageHelp        =  3, //! Image for marking a help action (e.g. for a button)
        imageClose       =  4, //! Image for closing widget
        imageAdd         =  5, //! Image for adding something
        imageRemove      =  6, //! Image for removing something
        imageClear       =  7, //! Image for clearing something (like e.g. a string field)
        imageEdit        =  8, //! Image for editing something
        imageSave        =  9, //! Image for saving something
        imageMediaRecord = 10, //! Image for start recordin media playing
        imageMediaPlay   = 11, //! Image for playing media playing
        imageMediaPause  = 12, //! Image for pausing media playing
        imageMediaStop   = 13  //! Image for stoping media playing
      };

      static const size_t imageCount = 14; //! Number of predefined stock images

      enum ScrollKnobMode {
        scrollKnobModeSingle,
        scrollKnobModeDouble,
        scrollKnobModeNone
      };

      enum ToolbarPosition {
        toolbarTop,
        toolbarBottom
      };

      enum MenuType {
        menuTypeStrip,
        menuTypePopup,
        menuTypeCustom
      };

      enum DialogActionPosition {
        actionPosHorizButtonRowBelow,
        actionPosVertButtonRowRight
      };

    public:
      Theme(Display *display);
      virtual ~Theme();

      /**
        @name Fonthandling
        Information about the to be used fonts for this theme.
      */
      //@{
      virtual bool HasFontSettings() const = 0;

      virtual std::wstring GetProportionalFontName() const = 0;
      virtual double GetProportionalFontSize() const = 0;

      virtual std::wstring GetFixedFontName() const = 0;
      virtual double GetFixedFontSize() const = 0;
      //@}

      /**
        @name Theme specific GUI primitives
        Colors, fills, frames and images for themes.
      */
      //@{
      virtual Color GetColor(Display::ColorIndex color) const = 0;
      virtual Fill* GetFill(Display::FillIndex fill) const = 0;
      virtual Frame* GetFrame(Display::FrameIndex frame) const = 0;
      virtual Image* GetImage(Display::ImageIndex image) const = 0;
      virtual Image* GetStockImage(StockImage image) const = 0;

      virtual void PushUniqueFill(DrawInfo* draw, size_t index, size_t maximum) = 0;
      virtual void PopUniqueFill(DrawInfo* draw, size_t index, size_t maximum) = 0;

      /**
        Background is light (instead of dark).
        */
      virtual bool IsLightTheme() const = 0;
      //@}

      /**
        @name Spaces
        Theme specific spacing.
      */
      //@{
      virtual size_t GetSpaceHorizontal(Display::Space space) const = 0;
      virtual size_t GetSpaceVertical(Display::Space space) const = 0;

      virtual size_t GetIconWidth() const = 0;
      virtual size_t GetIconHeight() const = 0;

      virtual size_t GetMinimalButtonWidth() const = 0;

      virtual size_t GetMinimalTableRowHeight() const = 0;

      virtual size_t GetFirstTabOffset() const = 0;
      virtual size_t GetLastTabOffset() const = 0;

      virtual size_t GetHorizontalFuelBarMinWidth() const = 0;
      virtual size_t GetHorizontalFuelBarMinHeight() const = 0;
      virtual size_t GetVerticalFuelBarMinWidth() const = 0;
      virtual size_t GetVerticalFuelBarMinHeight() const = 0;

      /**
        Return the width of the cursor depending on the font.
        */
      virtual size_t GetCursorWidth(OS::Font* font) const = 0;

      /**
        How many pixels must the mouse move before this is recognized as drag start?
      */
      virtual size_t GetDragStartSensitivity() const = 0;
      /**
        How many pixels may the move move after click (and while holding) to get this
        recognizes as "click and hold" (right mouse button replacement)?
      */
      virtual size_t GetMouseClickHoldSensitivity() const = 0;
      //@}

      /**
        @name Timing
        Timing information like cursor blink time etc.
      */
      //@{
      virtual unsigned long GetCursorBlinkTimeMicroSeconds() const = 0;
      //@}

      /**
        @name Variants and behaviours
        Choosing between display variants and different behaviours.
      */
      //@{
      virtual ScrollKnobMode GetScrollKnobMode() const = 0;
      virtual ToolbarPosition GetToolbarPosition() const = 0;
      virtual bool ShowButtonImages() const = 0;
      virtual bool RequestFingerFriendlyControls() const = 0;
      virtual Lum::Base::Size GetFingerFriendlyMinWidth() const = 0;
      virtual Lum::Base::Size GetFingerFriendlyMinHeight() const = 0;

      virtual bool ShowKeyShortcuts() const = 0;
      virtual bool FocusOnEditableControlsOnly() const = 0;

      virtual MenuType GetMenuType() const = 0;
      virtual int GetSubMenuHorizontalOffset() const = 0;
      virtual bool OptimizeMenus() const = 0;
      virtual size_t MaxPopupMenuEntries() const = 0;

      virtual bool SubdialogsAlwaysFlexHoriz() const = 0;
      virtual bool SubdialogsAlwaysFlexVert() const = 0;

      virtual bool PopupsAreDialogs() const = 0;

      virtual DialogActionPosition GetDialogActionPosition() const =0;
      virtual bool ImplicitCancelAction() const =0;
      virtual bool ImplicitDefaultAction() const =0;
      //@}
    };
  }
}

#endif
