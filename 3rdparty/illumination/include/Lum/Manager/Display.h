#ifndef LUM_MANAGER_DISPLAY_H
#define LUM_MANAGER_DISPLAY_H

/*
  This source is part of the Illumination library
  Copyright (C) 2009  Tim Teulings

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

#include <Lum/Base/Model.h>

#include <Lum/Model/Action.h>

namespace Lum {
  namespace Manager {
    /**
      Manager for handling of advanced display aspects.
    */
    class LUMAPI Display : public Lum::Base::MsgObject
    {
    public:
      enum DisplayState
      {
        DisplayStateUnknown,
        DisplayStateOff,
        DisplayStateOn
      };

    protected:
      Display();

    public:
      virtual ~Display();

      /**
        Return the global DisplayManager instance.

        An instance is available as soon as the display is initialized.
        */
      static Display* Instance();

      /**
        Returns the state of the display (on, off or unknown).
        */
      virtual DisplayState GetDisplayState() const = 0;


      /**
        Register an action that gets triggered if the state of the display
        changes.
        */
      virtual Model::Action* GetDisplayStateChangeAction() const = 0;

      /**
        Forbit or allow screen blanking.

        Screenblanking could be supressed if you wnat to show a move or a slideshow
        and do not want the screen blanker to kick in because of inactivity.
        */
      virtual bool AllowScreenBlanking(bool blankable) = 0;
    };
  }
}

#endif
