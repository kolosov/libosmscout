#ifndef LUM_OS_HILDON_KEYBOARD_H
#define LUM_OS_HILDON_KEYBOARD_H

/*
  This source is part of the Illumination library
  Copyright (C) 2008  Tim Teulings

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

#include <X11/Xlib.h>

#include <Lum/Manager/Keyboard.h>

#include <hildon-input-method/hildon-im-protocol.h>

namespace Lum {
  namespace OS {
    namespace Hildon {

      class LUMAPI Keyboard : public Manager::Keyboard
      {
      private:
        OS::Window      *window;
        Lum::Object     *object;
        std::string     buffer;
        HildonIMTrigger triggerType;

      private:
        ::Window GetKeyboardWindow() const;
        void SendClipboardSelectionQueryResponse();
        void SendInputMode();
        void SendSurrounding();
        void SendKey(Key key);
        bool SendCommand(HildonIMCommand command);
        void ShowKeyboard();
        void HideKeyboard();

      public:
        Keyboard();
        ~Keyboard();

        void Shutdown();

        void OnWindowFocusIn(OS::Window* window);
        void OnWindowFocusOut(OS::Window* window);
        void OnFocusChange(OS::Window* window,
                           Lum::Object* oldObject,
                           Lum::Object* newObject);
        void OnMouseClick(Window* window,
                          Lum::Object* object,
                          MouseEvent* event);

        bool HandleEvent(Window* window, Event* event);

        void Cut();
        void Copy();
        void Paste();
      };
    }
  }
}

#endif
