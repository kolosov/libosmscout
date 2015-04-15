#ifndef LUM_ASPECTRATIO_H
#define LUM_ASPECTRATIO_H

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

#include <Lum/Object.h>
#include <Lum/Scroller.h>

namespace Lum {

  class LUMAPI AspectRatio : public Group
  {
  public:
    enum HAlignment {
      halignLeft,
      halignCenter,
      halignRight
    };

    enum VAlignment {
      valignTop,
      valignCenter,
      valignBottom
    };

  private:
    Object     *object;
    size_t     hAspect;
    size_t     vAspect;
    HAlignment halign;
    VAlignment valign;

  public:
    AspectRatio();
    ~AspectRatio();

    AspectRatio* SetAspectRatio(size_t hAspect, size_t vAspect);
    AspectRatio* SetAlignment(HAlignment halign, VAlignment valign);
    AspectRatio* SetObject(Object* object);

    Object* GetObject() const;

    bool VisitChildren(Visitor &visitor, bool onlyVisible);

    void CalcSize();
    void Layout();

    static AspectRatio* Create(Object* object,
                               size_t hAspect, size_t vAspect,
                               bool horizontalFlex=false, bool verticalFlex=false);
    static AspectRatio* Create(Object* object,
                               size_t hAspect, size_t vAspect,
                               HAlignment halign, VAlignment valign,
                               bool horizontalFlex=false, bool verticalFlex=false);
  };
}

#endif
