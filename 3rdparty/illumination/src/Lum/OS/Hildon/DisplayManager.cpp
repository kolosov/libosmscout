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

#include <Lum/OS/Hildon/Display.h>

#include <Lum/OS/X11/Display.h>
#include <iostream>
namespace Lum {
  namespace OS {
    namespace Hildon {

      void OssoDisplayEventCallback(osso_display_state_t state, gpointer data)
      {
        Display  *manager=static_cast<Display*>(data);

        assert(manager!=NULL);

        switch (state) {
        case OSSO_DISPLAY_ON:
        case OSSO_DISPLAY_DIMMED:
          manager->SetDisplayState(Display::DisplayStateOn);
          break;
        case OSSO_DISPLAY_OFF:
          manager->SetDisplayState(Display::DisplayStateOff);
          break;
        default:
          manager->SetDisplayState(Display::DisplayStateUnknown);
          break;
        }
      }

      Display::Display()
       : displayState(DisplayStateUnknown),
         displayStateChangeAction(new Model::Action()),
         timerAction(new Model::Action())
      {
        osso_return_t result;
        X11::Display  *display=dynamic_cast<X11::Display*>(OS::display);

        // Hildon is currently X11 only!
        assert(display!=NULL);

        result=osso_hw_set_display_event_cb(display->ossoContext,
                                            OssoDisplayEventCallback,
                                            this);

        if (result!=OSSO_OK) {
          std::cout << "Installing osso display event callback failed: " << result << std::endl;
        }

        Observe(timerAction);
      }

      void Display::SetDisplayState(DisplayState displayState)
      {
        if (this->displayState!=displayState) {
          this->displayState=displayState;
          displayStateChangeAction->Trigger();
        }
      }

      Display::DisplayState Display::GetDisplayState() const
      {
        return displayState;
      };

      Model::Action* Display::GetDisplayStateChangeAction() const
      {
        return displayStateChangeAction;
      }

      bool Display::AllowScreenBlanking(bool blankable)
      {
        X11::Display *display=dynamic_cast<X11::Display*>(OS::display);

        // Hildon is currently X11 only!
        assert(display!=NULL);

        if (!blankable) {
          if (osso_display_blanking_pause(display->ossoContext)==OSSO_OK) {
            OS::display->RemoveTimer(timerAction);
            OS::display->AddTimer(45,0,timerAction);
            return true;
          }
          else {
            return false;
          }
        }
        else {
          OS::display->RemoveTimer(timerAction);
          return true;
        }
      }

      void Display::Resync(Lum::Base::Model* model, const Lum::Base::ResyncMsg& msg)
      {
        if (model==timerAction && timerAction->IsFinished()) {
          X11::Display *display=dynamic_cast<X11::Display*>(OS::display);

          // Hildon is currently X11 only!
          assert(display!=NULL);

          osso_display_blanking_pause(display->ossoContext);
          OS::display->AddTimer(45,0,timerAction);
        }
      }
    }
  }
}
