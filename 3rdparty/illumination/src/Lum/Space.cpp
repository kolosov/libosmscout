#include <Lum/Space.h>

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

namespace Lum {

  Space::Space(Size size)
  : size(size)
  {
    // no code
  }

  HSpace::HSpace(Size size, bool flex)
  : Space(size)
  {
    SetFlex(flex,false);
  }

  HSpace::HSpace(bool flex)
  : Space(sizeNormal)
  {
    SetFlex(flex,false);
  }

  void HSpace::CalcSize()
  {
    width=0;
    height=0;
    minHeight=0;

    switch (size) {
    case sizeEmpty:
      width=0;
      break;
    case sizeMicro:
      width=1;
      break;
    case sizeSmall:
      if (OS::display->GetSpaceHorizontal(OS::Display::spaceInterObject)>=2) {
        width=OS::display->GetSpaceHorizontal(OS::Display::spaceInterObject) / 2;
      }	
      else {
        width=1;
      }
      break;
    case sizeNormal:
      width=OS::display->GetSpaceHorizontal(OS::Display::spaceInterObject);
      break;
    case sizeHuge:
      width=2*OS::display->GetSpaceHorizontal(OS::Display::spaceInterObject);
      break;
    }

    minWidth=width;

    Object::CalcSize();
  }

  VSpace::VSpace(Size size, bool flex)
  : Space(size)
  {
    SetFlex(false,flex);
  }

  VSpace::VSpace(bool flex)
  : Space(sizeNormal)
  {
    SetFlex(false,flex);
  }

  void VSpace::CalcSize()
  {
    width=0;
    minWidth=0;
    height=0;

    switch (size) {
    case sizeEmpty:
      height=0;
      break;
    case sizeMicro:
      height=1;
      break;
    case sizeSmall:
      if (OS::display->GetSpaceVertical(OS::Display::spaceInterObject)>=2) {
        height=OS::display->GetSpaceVertical(OS::Display::spaceInterObject) / 2;
      }
      else {
        height=1;
      }
      break;
    case sizeNormal:
      height=OS::display->GetSpaceVertical(OS::Display::spaceInterObject);
      break;
    case sizeHuge:
      height=2*OS::display->GetSpaceVertical(OS::Display::spaceInterObject);
      break;
    }

    minHeight=height;

    Object::CalcSize();
  }
}
