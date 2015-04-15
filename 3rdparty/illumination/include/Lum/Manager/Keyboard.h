#ifndef LUM_MANAGER_KEYBOARD_H
#define LUM_MANAGER_KEYBOARD_H

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

#include <Lum/Private/ImportExport.h>

#include <Lum/OS/Window.h>

#include <Lum/Object.h>

namespace Lum {
  namespace Manager {

    class LUMAPI Keyboard : public Lum::Base::MsgObject
    {
    protected:
      Keyboard();

    public:
      virtual ~Keyboard();

      /**
        Return the global KeyboardManager instance.

        An instance is available as soon as the display is initialized.
        */
      static Keyboard* Instance();

      virtual void Shutdown() = 0;

      virtual void OnWindowFocusIn(OS::Window* window) = 0;
      virtual void OnWindowFocusOut(OS::Window* window) = 0;
      virtual void OnFocusChange(OS::Window* window,
                                 Lum::Object* oldObject,
                                 Lum::Object* newObject) = 0;
      virtual void OnMouseClick(OS::Window* window,
                                Lum::Object* object,
                                OS::MouseEvent* event) = 0;

      virtual bool HandleEvent(OS::Window* window, OS::Event* event)= 0;
    };
  }
}

#endif
