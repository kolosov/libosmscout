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

#include <Lum/LED.h>

#include <Lum/OS/Theme.h>

namespace Lum {

  LED::LED(Base::Model* model)
   : ImageState(OS::display->GetImage(OS::Display::ledImageIndex),false)
  {
    SetModel(model);
  }

  LED* LED::Create()
  {
    LED *l;

    l=new LED();

    return l;
  }

  LED* LED::Create(Base::Model* model)
  {
    LED *l;

    l=new LED();
    l->SetModel(model);

    return l;
  }
}
