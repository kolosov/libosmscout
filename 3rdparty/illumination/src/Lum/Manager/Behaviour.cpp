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

#include <Lum/Manager/Behaviour.h>

#include <Lum/Private/Config.h>

#include <Lum/Manager/Behaviour.h>

#include <Lum/OS/Base/Behaviour.h>

#if defined(LUM_HAVE_LIB_HILDON)
  #include <Lum/OS/Hildon/Behaviour.h>
#endif

namespace Lum {
  namespace Manager {

    static Behaviour* instance=NULL;

    Behaviour::CustomMenuHandler::CustomMenuHandler(OS::Window* window)
     : window(window)
    {
    }

    Behaviour::CustomMenuHandler::~CustomMenuHandler()
    {
      // no code
    }

    Behaviour::Behaviour()
    {
      // no code
    }

    Behaviour::~Behaviour()
    {
      // no code
    }

    Behaviour* Behaviour::Instance()
    {
      if (instance==NULL) {
        assert(OS::display!=NULL);

#if defined(LUM_HAVE_LIB_HILDON)
        instance=new Lum::OS::Hildon::Behaviour();
#else
        instance=new Lum::OS::Base::DefaultBehaviour();
#endif
      }

      assert(instance!=NULL);

      return instance;
    }
  }
}
