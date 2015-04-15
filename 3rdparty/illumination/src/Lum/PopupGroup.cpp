/*
  This source is part of the Illumination library
  Copyright (C) 2007  Tim Teulings

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

#include <Lum/PopupGroup.h>

#include <algorithm>

#include <Lum/OS/Theme.h>

namespace Lum {

  PopupGroup::PopupGroup()
  : main(NULL),seamless(true)
  {
    // no code
  }

  PopupGroup::~PopupGroup()
  {
    delete main;
  }

  void PopupGroup::SetMain(Object* object, bool seamless)
  {
    this->main=object;
    this->seamless=seamless;
    if (main!=NULL) {
      main->SetParent(this);
    }
  }

  bool PopupGroup::VisitChildren(Visitor &visitor, bool /*onlyVisible*/)
  {
    if (main!=NULL) {
      if (!visitor.Visit(main)) {
        return false;
      }
    }

    return true;
  }

  void PopupGroup::CalcSize()
  {
    main->CalcSize();

    minWidth=main->GetOMinWidth();
    minHeight=main->GetOMinHeight();
    width=main->GetOWidth();
    height=main->GetOHeight();
    maxWidth=std::min(maxWidth,main->GetOMaxWidth());
    maxHeight=std::min(maxHeight,main->GetOMaxHeight());

    if (!seamless) {
      minWidth+=2*OS::display->GetSpaceHorizontal(OS::Display::spaceWindowBorder);
      minHeight+=2*OS::display->GetSpaceVertical(OS::Display::spaceWindowBorder);
      width+=2*OS::display->GetSpaceHorizontal(OS::Display::spaceWindowBorder);
      height+=2*OS::display->GetSpaceVertical(OS::Display::spaceWindowBorder);
      // We do not increase our max size....
    }

    Group::CalcSize();
  }

  void PopupGroup::Layout()
  {
    if (seamless) {
      main->MoveResize(x,y,width,height);
    }
    else {
      main->MoveResize(x+OS::display->GetSpaceHorizontal(OS::Display::spaceWindowBorder),
                       y+OS::display->GetSpaceVertical(OS::Display::spaceWindowBorder),
                       width-2*OS::display->GetSpaceHorizontal(OS::Display::spaceWindowBorder),
                       height-2*OS::display->GetSpaceVertical(OS::Display::spaceWindowBorder));
    }

    Group::Layout();
  }
}
