#ifndef LUM_TAB_H
#define LUM_TAB_H

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

#include <list>

#include <Lum/Base/Size.h>

#include <Lum/Model/Number.h>

#include <Lum/FrameGroup.h>
#include <Lum/Multi.h>
#include <Lum/Object.h>

namespace Lum {

  class LUMAPI Tab : public Control
  {
  private:
    Model::NumberRef model;       //! numeric model defining the currently visible entry.
    size_t           mw;          //! width of single tab rider
    size_t           mh;          //! height of single tab rider
    Object           *lastDrawn;  //! At that time visible object during last draw
    FrameGroup       *frameGroup; //! FrameGroup object containing the Multi object
    Multi            *multi;      //! Multi object, containing list of objects

    std::list<Object*> list;       //! list of embeded objects

  public:
    Tab();
    ~Tab();

    bool VisitChildren(Visitor &visitor, bool onlyVisible);

    Tab* Add(Object* label, Object* object);
    Tab* Add(const std::wstring& label, Object* object);

    void ActivatePreviousTab();
    void ActivateNextTab();

    bool SetModel(Base::Model* model);

    void CalcSize();

    void Layout();
    void Draw(OS::DrawInfo* draw,
              int x, int y, size_t w, size_t h);

    void Resync(Base::Model* model, const Base::ResyncMsg& msg);

    static Tab* Create(bool horizontalFlex=false, bool verticalFlex=false);
  };
}

#endif
