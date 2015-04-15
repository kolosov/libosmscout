#ifndef LUM_PANE_H
#define LUM_PANE_H

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

#include <Lum/Object.h>

namespace Lum {
   /**
     Pane allows to change the size of horizontal or vertical
     aligned object by draging a knob between the object, thus changing
     sizesof neighboring objects on the fly.
    */
  class LUMAPI Pane : public List
  {
  public:
    /**
      The alignment direction of the control.
     */
    enum Direction {
      horizontal,
      vertical
    };

  private:
    Direction direction; //! The direction
    size_t    pos;       //! The position of the know during drag & drop
    size_t    offset;    //! The offset to the knob origin during drag & drop

  private:
    void HandleMouseMoveEvent(const OS::MouseEvent& event);

  public:
    Pane(Direction direction);

    void Add(Object* object);

    void CalcSize();
    void Layout();

    bool HandleMouseEvent(const OS::MouseEvent& event);
  };
}

#endif
