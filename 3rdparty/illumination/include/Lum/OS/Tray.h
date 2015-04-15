#ifndef LUM_OS_TRAY_H
#define LUM_OS_TRAY_H

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

#include <Lum/Private/ImportExport.h>

#include <Lum/Model/Action.h>

#include <Lum/OS/Bitmap.h>
#include <Lum/OS/Window.h>

namespace Lum {
  namespace OS {

    class LUMAPI Tray
    {
    public:
      enum TrayStatus {
        statusHidden,  /** Tray is currently not visible */
        statusOpening, /** Tray is triggered to ge shown, but is not yet visible */
        statusShown    /** Tray is visible */
      };

    public:
      virtual ~Tray();

      /**
        Returns the current status of the tray
      */
      virtual TrayStatus GetStatus() const = 0;

      /**
       Return a pointer to the internal bitmap.

       You must draw to this bitmap respeciting its size. If you
       are finished drawing, call redraw to trigger a redisplay using
       the method Redraw().

       Only try to access the bitmap, if the Tray is in status
       statusShown. In all other cases NULL will be returned.
      */
      virtual Bitmap* GetBitmap() const = 0;

      /**
       Define the relative width and height of the tray entry. This are not
       absolute pixel values but relative values. A value of 50,50 or 1,1 does mean,
       that the entry has the same width and height. A value of 100,50, or 2,1 does
       mean that the entry has retangular size where the width is two times the height.

       Note that this acts only as an recomendation. Depending on the implementation,
       the resulting bitmap may have different bounds.
      */
      virtual void SetRelativeSize(size_t width, size_t height) = 0;

      /**
        Assign a menu to the tray entry. Menu will automatically opened if apropiate events
        are received.
      */
      virtual void SetMenu(Window* window) = 0;

      /**
        Show the tray.
      */
      virtual bool Show() = 0;

      /**
        Hide the tray.
      */
      virtual bool Hide() = 0;

      /**
        Make the contents of the bitmap visible.
      */
      virtual void Redraw() = 0;

      /**
        Set the title for the tooltip.
      */
      virtual void SetTitle(const std::wstring& title) = 0;

      /**
        Action for triggering mouse and keyboard events. Not ethat depending of the underlying
        OS not all events will be trigger. Try to make use of the more high level event actions
        where possible.
        See Lum::OS::Window::GetEventAction().
      */
      virtual ::Lum::Model::Action* GetEventAction() const = 0;

      /**
        Action triggered when the tray has opened.
      */
      virtual ::Lum::Model::Action* GetOpenedAction() const = 0;
    };
  }
}

#endif
