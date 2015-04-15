#ifndef LUM_PANEL_H
#define LUM_PANEL_H

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

#include <Lum/Object.h>

namespace Lum {
   /**
     A layouting class for grouping objects horizontaly or vertically.
     Does some complex resizing operations to make the arrangement
     of its children look lice.

     On initialisation @otype{Panel} tries to use maximum space by
     resizing resizable controls to claim all available space.

     On resize the same space is given and taken from all controls
     that can change their size.

     If space is still left, object will be drawing from left to right
     or top to bottom depending on the direction of @otype{Panel}.
   */
  class LUMAPI Panel : public List
  {
  protected:
    Panel();

  public:
    virtual Panel* Add(Lum::Object* object);
    virtual Panel* AddSpace(bool flex=false) = 0;
  };

  /**
    Variant of Panel that arranges all containing objects horizontally.
   */
  class LUMAPI HPanel : public Panel
  {
  public:
    HPanel();

    HPanel* AddSpace(bool flex=false);

    void CalcSize();
    void Layout();

    static HPanel* Create(bool horizontalFlex=false, bool verticalFlex=false);
  };

  /**
    Variant of Panel that arranges all containing objects vertically.
   */
  class LUMAPI VPanel : public Panel
  {
  public:
    VPanel();

    VPanel* AddSpace(bool flex=false);

    void CalcSize();
    void Layout();

    static VPanel* Create(bool horizontalFlex=false, bool verticalFlex=false);
  };
}

#endif
