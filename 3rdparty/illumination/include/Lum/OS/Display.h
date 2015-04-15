#ifndef LUM_OS_DISPLAY_H
#define LUM_OS_DISPLAY_H

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

#include <memory>
#include <string>

#include <Lum/Private/ImportExport.h>

#include <Lum/Base/DateTime.h>
#include <Lum/Base/DragDrop.h>
#include <Lum/Base/Object.h>

#include <Lum/Model/Action.h>

#include <Lum/OS/Color.h>
#include <Lum/OS/Event.h>
#include <Lum/OS/EventLoop.h>
#include <Lum/OS/Fill.h>
#include <Lum/OS/Font.h>
#include <Lum/OS/Frame.h>
#include <Lum/OS/Image.h>

namespace Lum {
  namespace Manager {
    class Keyboard;
  }

  namespace OS {

    class Theme;     // The display holds a concrete behaviour, but does not know anything about them
    class Window;    // Forward declaration to avoid recursive includes

    /**
      Display represents the graphical display with all its global attributes.
    */
    class LUMAPI Display : public Lum::Base::MsgObject
    {

    public:
      static const size_t colorCount = 16; //! Number of predefined colors
      static const size_t fontCount  =  2; //! Number of predefined fonts
      static const size_t fillCount  = 42; //! Number of predefined fills
      static const size_t frameCount =  7; //! Number of predefined frames
      static const size_t imageCount = 29; //! Number of predefined images

    public:
      enum ColorIndex {
        backgroundColor               =  0, // To be removed!
        tableTextColor                =  1, //! Color of text in tables and edit fields
        textColor                     =  2, //! Color of standard text (window, button...)
        textSelectColor               =  3, //! Color of standard text in pressed objects
        textDisabledColor             =  4, //! Color of disabled text
        fillColor                     =  5, // To be removed!
        fillTextColor                 =  6, //! Color of text in fills (tables, entries, progress...)
        graphScaleColor               =  7, //! Color of scaling marks in a graph
        blackColor                    =  8, // To be removed!
        whiteColor                    =  9, // To be removed!
        tabTextColor                  = 10, //! Font color for a textual tab rider
        tabTextSelectColor            = 11, //! Font color of a selected textual tab rider
        editTextColor                 = 12, //! Font color for edit fields and similar
        editTextFillColor             = 13, //! Font color for selected texts in edit fields and similar
        textSmartAColor               = 14, //! Back font color for smart text
        textSmartBColor               = 15  //! Front font color for smart text
      };

      enum FillIndex {
        backgroundFillIndex                =  0, //! Normal window background
        tableBackgroundFillIndex           =  1, //! Table background for even rows
        tableBackground2FillIndex          =  2, //! Table background for uneven rows
        buttonBackgroundFillIndex          =  3, //! Background of normal buttons
        positiveButtonBackgroundFillIndex  =  4,
        negativeButtonBackgroundFillIndex  =  5,
        defaultButtonBackgroundFillIndex   =  6,
        toolbarButtonBackgroundFillIndex   =  7, //! Background of toolbar button
        scrollButtonBackgroundFillIndex    =  8, //! Background of scroll button
        comboBackgroundFillIndex           =  9, //! Background of combobox
        editComboBackgroundFillIndex       = 10,
        entryBackgroundFillIndex           = 11, //! Background of entry fields
        hprogressBackgroundFillIndex       = 12, //! Background of progress bar
        hprogressFillIndex                 = 13, //! Filled part of progress bar
        vprogressBackgroundFillIndex       = 14, //! Background of progress bar
        vprogressFillIndex                 = 15, //! Filled part of progress bar
        hknobBoxFillIndex                  = 16, //! Background of horizontal scoll box
        vknobBoxFillIndex                  = 17, //! Background of vertical scoll box
        tabBackgroundFillIndex             = 18, //! Background of tab control
        tabRiderBackgroundFillIndex        = 19, //! Background of tab rider
        columnBackgroundFillIndex          = 20, //! Background of table column header (for one-column tables)
        columnLeftBackgroundFillIndex      = 21, //! Background of left table column header
        columnMiddleBackgroundFillIndex    = 22, //! Background of middle table column header
        columnRightBackgroundFillIndex     = 23, //! Background of right table column header
        columnEndBackgroundFillIndex       = 24, //! Background of the area behind the last table column
        menuStripBackgroundFillIndex       = 25, //! Background of a menu strip
        menuPulldownBackgroundFillIndex    = 26, //! Background of a pulldown menu (entry in a menu strip)
        menuEntryBackgroundFillIndex       = 27, //! Background of a menu entry
        menuWindowBackgroundFillIndex      = 28, //! Background of a menu window
        popupWindowBackgroundFillIndex     = 29, //! Background of a popup window
        dialogWindowBackgroundFillIndex    = 30, //! Background of a popup window
        tooltipWindowBackgroundFillIndex   = 31, //! Background of a popup window
        graphBackgroundFillIndex           = 32, //! Background of graph-like controls
        toolbarBackgroundFillIndex         = 33, //! Background of a toolbar
        labelBackgroundFillIndex           = 34, //! Background of an label
        listboxBackgroundFillIndex         = 35, //! Background of a listbox (without scrollbars)
        scrolledBackgroundFillIndex        = 36, //! Background of a scrolled area (including scrollbar)
        hscrollBackgroundFillIndex         = 37, //! Background of horizontal scrollbar (inkludings croll buttons)
        vscrollBackgroundFillIndex         = 38, //! Background of vertical scrollbar (inkludings croll buttons)
        boxedBackgroundFillIndex           = 39, //! Background of boxed content
        plateBackgroundFillIndex           = 40, //! Background of controls presenting some simple content (e.g. color chooser)
        statusbarBackgroundFillIndex       = 41  //! Background of the whole status line
      };

      enum FrameIndex {
        tabFrameIndex             =  0, //! tab frame
        hscaleFrameIndex          =  1, //! Frame around a horizontal slider
        vscaleFrameIndex          =  2, //! Frame around a vertical slider
        focusFrameIndex           =  3, //! Frame for showing focus
        statuscellFrameIndex      =  4, //! Frame around a single status cell
        valuebarFrameIndex        =  5, //! Frame around value bars
        groupFrameIndex           =  6  //! Frame for grouping a number of elements
      };

      /**
        List of necessary theming pecific images for visualisation of controls.
       */
      enum ImageIndex {
        hknobImageIndex            =  0, //! Image of a horizontal scrolling knob
        vknobImageIndex            =  1, //! Image of a vertical scrolling knob
        arrowLeftImageIndex        =  2, //! Image of a arrow pointing to the left side
        arrowRightImageIndex       =  3, //! Image of a arrow pointing to the right side
        arrowUpImageIndex          =  4, //! Image of a arrow pointing up
        arrowDownImageIndex        =  5, //! Image of a arrow pointing down
        scrollLeftImageIndex       =  6, //! Image for scroll bar button (left)
        scrollRightImageIndex      =  7, //! Image for scroll bar button (right)
        scrollUpImageIndex         =  8, //! Image for scroll bar button (up)
        scrollDownImageIndex       =  9, //! Image for scroll bar button (down)
        comboImageIndex            = 10, //! Image for read only combo box popup (normally just an arrow)
        comboEditButtonImageIndex  = 11, //! Image for editable combo box popup (normally just an arrow)
        comboDividerImageIndex     = 12, //! Divider between content and image
        treeExpanderImageIndex     = 13, //! Image for tree node (de-)expansion
        hscaleKnobImageIndex       = 14, //! Horizontal slider knob
        vscaleKnobImageIndex       = 15, //! Vertical slider knob
        leftSliderImageIndex       = 16, //! Left (filled) part of the slider control
        rightSliderImageIndex      = 17, //! Right (unfilled) part of the slider control
        topSliderImageIndex        = 18, //! Top (unfilled) part of the slider control
        bottomSliderImageIndex     = 19, //! bottom (filled) part of the slider control
        menuDividerImageIndex      = 20, //! Divider between menu entry groups
        menuSubImageIndex          = 21, //! Image for signaling a sub menu
        menuCheckImageIndex        = 22, //! Image for checkmark in menues
        menuRadioImageIndex        = 23, //! Image for radio button in menues
        hpaneHandleImageIndex      = 24, //! Grip object for the pane control
        vpaneHandleImageIndex      = 25, //! Grip object for the pane control
        radioImageIndex            = 26, //! Radio control image
        checkImageIndex            = 27, //! Checkbox image
        ledImageIndex              = 28 //! LED image (for read-only signaling of boolean state)
      };

      enum FontScale {
        fontScaleFootnote = 85,
        fontScaleNormal   = 100,
        fontScaleCaption2 = 115,
        fontScaleCaption1 = 135,
        fontScaleLogo     = 200
      };

      enum FontType {
        fontTypeProportional,
        fontTypeFixed
      };

      enum TextDirection {
        textDirectionLeftToRight,
        textDirectionRightToLeft
      };

      enum Type {
        typeGraphical = 0,
        typeTextual   = 1
      };

      enum ColorMode {
        colorModeMonochrome = 0,
        colorModeGreyScale  = 1,
        colorModeColor      = 2
      };

      enum Size {
        sizeTiny   = 0,
        sizeSmall  = 1,
        sizeNormal = 2,
        sizeLarge  = 3,
        sizeHuge   = 4
      };

      enum Space {
        spaceWindowBorder      =  0, //! Space between window border and its content
        spaceInterGroup        =  1, //! Space between groups of elements
        spaceGroupIndent       =  2, //! Indention between groups
        spaceInterObject       =  3, //! Space between individual objects
        spaceObjectBorder      =  4, //! Space between object border and its elements
        spaceIntraObject       =  5, //! Space between elements within an object
        spaceLabelObject       =  6, //! Space between an object and its label
        spaceObjectDetail      =  7  //! Space for object visualizing details
      };

      enum ByteOrder {
        littleEndian,
        bigEndian
      };

    public:
      /**
        @name (De)initialisation
        Initialisation and deinitialisation of Display instances.
      */
      //@{
      /**
        Open the display. Opening a display may fail.
      */
      virtual bool Open() = 0;

      /**
        Close the display.
      */
      virtual void Close() = 0;
      //@}

      /**
        @name Screen and workarea dimensions and resolution
        Returns the dimensions of the screen and the workarea, which is the useable part
        of the screen not covered by task bars and similar. LAlso returns the resolution of the
        screen and DPI.
      */
      //@{
      virtual double GetDPI() const = 0;

      /**
        Return the screen width in pixel.
      */
      virtual unsigned long GetScreenWidth() const = 0;

      /**
        Return the screen height in pixel.
      */
      virtual unsigned long GetScreenHeight() const = 0;

      /**
        Return the work area width in pixel. Work area is the part
        of the desktop where you can place application window on.
      */
      virtual unsigned long GetWorkAreaWidth() const = 0;

      /**
        Return the work area height in pixel. Work area is the part
        of the desktop where you can place application window on.
      */
      virtual unsigned long GetWorkAreaHeight() const = 0;

      /**
        Returns a simple hint about the estimated size of the display.
        This hint can be used to use different GUI elements for different
        types of screen resolutions.
        */
      virtual Size GetSize() const = 0;

      virtual bool IsLandscape() const = 0;
      virtual bool IsPortrait() const = 0;

      virtual Model::Action* GetScreenRotatedAction() const = 0;
      //@}

      /**
        @name Fonts
        Method for font management.
      */
      //@{
      /**
        Get a default font of the given size, where
        the size is handed to the function as percent points in relation
        to the default font size.
      */
      virtual Font* GetFont(size_t scale=fontScaleNormal) const = 0;

      /**
        Get a font of the given type and the given size, where
        the size is handed to the function as percent points in relation
        to the default font size of the given font type.
      */
      virtual Font* GetFont(FontType type,
                            size_t scale=fontScaleNormal) const = 0;

      /**
        Get a font of the given type and the given size, where
        the size is given in pixel. If no font with the exact pixel
        size can be found, NULL will be returned.
      */
      virtual Font* GetFontByPixel(FontType type, size_t size) const = 0;
      /**
        Get a font of the given type and the given maximum size, where
        the maxium size is given in pixel. Note that the gven pixel
        size must be at least the height in pixel of the given default
        font size for the given font type, else it cannot be assured
        that a font can be found!
      */
      virtual Font* GetFontByMaxPixel(FontType type, size_t size) const = 0;
      //FontList GetFontList() = 0;
      //@}

      /**
        @name Colors
        Method for handling of colors and fills.
      */
      //@{
      virtual ColorMode GetColorMode() const = 0;

      /**
        Return the number of bits available for defining a color.
      */
      virtual size_t GetColorDepth() const = 0;

      /**
        Return the given predefined color.
      */
      virtual Color GetColor(ColorIndex color) const = 0;
      virtual Color GetColorByName(const std::string& name) = 0;
      virtual bool AllocateColor(double red, double green, double blue,
                                 Color& color) = 0;
      virtual bool AllocateNamedColor(const std::string& name,
                                      Color& color) = 0;
      virtual void FreeColor(Color color) = 0;

      /**
        @name Objects
        Method for handling of various highlevel objects like fills and frames.
      */
      //@{
      virtual Fill* GetFill(FillIndex fill) const = 0;
      virtual Frame* GetFrame(FrameIndex frame) const = 0;
      virtual Image* GetImage(ImageIndex image) const = 0;
      //@}

      /**
        @name Event loop
        Methods for dealing with the global event loop.
      */
      //@{
      virtual EventLoop* GetEventLoop() const = 0;
      virtual void MainLoop() = 0;
      virtual void Exit() = 0;
      virtual void QueueActionForAsyncNotification(Model::Action *action) = 0;
      virtual void QueueActionForEventLoop(Model::Action *action) = 0;
      //@}

      // Mouse
      virtual bool GetMousePos(int& x, int& y) const = 0;

      /**
        @name Selection
        Methods for dealing with OS specific global selection support. Whil clipboard must
        exlipcitely be copied a selection is imediately active.
      */
      //@{
      virtual bool RegisterSelection(Lum::Base::DataExchangeObject* object, Window* window) = 0;
      virtual void CancelSelection() = 0;
      virtual Lum::Base::DataExchangeObject* GetSelectionOwner() const = 0;
      virtual bool QuerySelection(Window* window, Lum::Base::DataExchangeObject* object) = 0;
      //@}

      /**
        @name Clipboard
        Methods dealing with the OS clipboard.
      */
      //@{
      /**
        Set the operating system clipboard to the given string value.

        Note, that the interface of this method will change to support more
        datatypes in the future.
      */
      virtual bool SetClipboard(const std::wstring& content) = 0;

      /**
        Request the current value of the operating system clipboard.

        If Illumination can get the clipboard and the clipboard does have
        a texttual value, it is returned. Otherwise an empty string is returned.
      */

      virtual std::wstring GetClipboard() const = 0;

      /**
        Clear the operating system global clipboard.
      */
      virtual void ClearClipboard() = 0;

      /**
        Returns true, if there is data available in the clipboard.
       */
      virtual bool HasClipboard() const = 0;
      //@}

      /**
        @name Timer
        Methods for dealing with timer.
      */
      //@{
      virtual void AddTimer(long seconds, long microseconds, Model::Action* action) = 0;
      virtual void RemoveTimer(Model::Action* action) = 0;
      //@}

      /*
      Sleep AddSleep(::VO::Base::Object::MsgObject object) = 0;
      void RemoveSleep(Sleep sleep) = 0;

      Channel AddChannel(Channel channel, unsigned long ops, ::VO::Base::Object::MsgObject object) = 0;
      void RemoveChannel(Channel channel) = 0;*/

      /**
        @name Textual representation of keyboard shortcuts.
        Methods for converting textual representations of keyboard shortcuts to strings
        and vice versa.
      */
      //@{
      /**
        Converts a given key description including qualifier and
        key into a textual description. This routine does
        not differentiate between logical identical qualifiers, that apear
        mutliple time, like Lum::OS::qualifierShiftLeft and Lum::OS::qualifierShiftRight.
        The will be maped to the same string.

        The current implementation does only handle printable keys
        correctly.
      */
      virtual void KeyToKeyDescription(unsigned long qualifier,
                                       const std::wstring& key,
                                       std::wstring& description) = 0;

      /**
        Converts a given key description including qualifier and
        key into a textual description for displaying them.
        The resulting string tries to mimic the syntax of the underlying driver
        instead of Display::KeyToKeyDescription which uses the internal
        format and thus is reversible. The result of this method cannot be
        reversed by Display::KeyDescriptionToKey.

        This method can be overloaded by display drivers.

        This routine does not differentiate between logical identical qualifiers,
        that apear mutliple time, like Lum::OS::qualifierShiftLeft and
        Lum::OS::qualifierShiftRight. The will be maped to the same string.

        The current implementation does only handle printable keys
        correctly.
       */
      virtual void KeyToDisplayKeyDescription(unsigned long qualifier,
                                              const std::wstring& key,
                                              std::wstring& description) = 0;

      /**
        Converts a given textual key \p description like it is returned
        by Display::KeyToKeyDescription into a keydescription containing of
        \p qualifier and \p key.

        The method returns 'true' if the conversion succeeds, 'false'
        if the conversion was not possible because of syntactical error in the
        description format.
      */
      virtual bool KeyDescriptionToKey(const std::wstring& description,
                                       unsigned long& qualifier,
                                       std::wstring& key) = 0;

      //@}


      virtual Type GetType() const = 0;

      virtual void SetProgramName(const std::wstring& name) = 0;
      virtual std::wstring GetProgramName() const = 0;

      virtual void SetAppName(const std::wstring& name) = 0;
      virtual std::wstring GetAppName() const = 0;

      /**
        @name Theming
        Methods for dealing with themes.
      */
      //@{
      virtual std::wstring GetThemePath() const = 0;
      virtual std::wstring GetThemeName() const = 0;
      virtual Theme* GetTheme() const = 0;
      //@}

      virtual size_t GetSpaceHorizontal(Space space) const = 0;
      virtual size_t GetSpaceVertical(Space space) const = 0;

      virtual TextDirection GetDefaultTextDirection() const = 0;

      /**
        Set the maximum time between mouse clicks that are detected as double click
        (or tripple click). Time is measured in milliseconds.
      */
      virtual void SetMultiClickTime(unsigned long time) = 0;
      virtual unsigned long GetMultiClickTime() const = 0;

      /**
        @name Virtual keyboards
        Mehods for dealing with support for virtual keyboards and other advanced
        input methods.
      */
      //@{
      virtual Manager::Keyboard* GetKeyboardManager() const = 0;
      //@}


      virtual void Beep() = 0 ;

      virtual void ReinitWindows() = 0;

      virtual ByteOrder GetSystemByteOrder() const = 0;

    };

    extern "C" {
      extern Display::ColorIndex GetColorIndexByName(const std::string& name);
    }

    extern LUMAPI const char* colorNames[Display::colorCount];
    extern LUMAPI const char* fillNames[Display::fillCount];
    extern LUMAPI const char* frameNames[Display::frameCount];
/*
      bool GetRGB32ByColorName(char name[], int r, int g, int b);
*/
    extern LUMAPI Display* display;
  }
}

#endif
