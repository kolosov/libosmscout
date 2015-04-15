#include <Lum/Toolbar.h>

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

#include <Lum/Base/Util.h>

namespace Lum {

  Toolbar::Toolbar()
  {
    SetBackground(OS::display->GetFill(OS::Display::toolbarBackgroundFillIndex));
  }

  void Toolbar::CalcSize()
  {
    std::list<Object*>::iterator iter;

    width=0;
    height=0;
    minWidth=0;
    minHeight=0;

    iter=list.begin();
    while (iter!=list.end()) {
      (*iter)->CalcSize();

      minWidth+=(*iter)->GetOMinWidth();
      width+=(*iter)->GetOWidth();
      minHeight=std::max(minHeight,(*iter)->GetOMinHeight());
      height=std::max(height,(*iter)->GetOHeight());

      ++iter;
    }

    List::CalcSize();
  }

  void Toolbar::Layout()
  {
    std::list<Object*>::iterator iter;
    int                          pos;

    pos=x;

    iter=list.begin();
    while (iter!=list.end()) {
      (*iter)->ResizeHeight(height);
      (*iter)->Move(pos,y+(height-(*iter)->GetOHeight()) / 2);
      pos+=(*iter)->GetOWidth();

      ++iter;
    }

    List::Layout();
  }
}
