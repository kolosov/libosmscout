#ifndef LUM_OS_HILDON_DISPLAY_H
#define LUM_OS_HILDON_DISPLAY_H

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

#include <Lum/Manager/Display.h>

#include <Lum/Model/Action.h>

namespace Lum {
  namespace OS {
    namespace Hildon {

      /**
        Manager for handling of advanced display aspects.
      */
      class LUMAPI Display : public Manager::Display
      {
      private:
        DisplayState     displayState;
        Model::ActionRef displayStateChangeAction;
        Model::ActionRef timerAction;

      public:
        Display();

        void SetDisplayState(DisplayState displayState);
        DisplayState GetDisplayState() const;
        Model::Action* GetDisplayStateChangeAction() const;
        bool AllowScreenBlanking(bool blankable);

        void Resync(Lum::Base::Model* model, const Lum::Base::ResyncMsg& msg);
      };
    }
  }
}

#endif
