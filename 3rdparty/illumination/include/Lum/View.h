#ifndef LUM_VIEW_H
#define LUM_VIEW_H

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

#include <Lum/Model/Action.h>

#include <Lum/KineticScroller.h>
#include <Lum/Object.h>
#include <Lum/Scroller.h>

namespace Lum {

  class LUMAPI View : public Group
  {
  private:
    enum State {
      pending,
      scrollMaybe,
      scrolling,
      kinetic
    };

  private:
    Scrollable                   *object;
    Control                      *hScroller;
    Control                      *vScroller;
    KineticScroller              *kineticScroller;

    bool                         hVisible;
    bool                         vVisible;

    bool                         kineticScrolling;

  private:
    void ReplayEvents();
    bool IsScrolling() const;
    void CalculateSpeed();

  public:
    View();
    ~View();

    bool VisitChildren(Visitor &visitor, bool onlyVisible);

    Lum::Object* GetMouseGrabFocusObject() const;
    Scrollable* GetObject() const;

    void SetObject(Scrollable* object);
    bool SetModel(Base::Model* model);

    void SetKineticScrolling(bool kineticScrolling);

    void CalcSize();
    void Layout();
    void PreDrawChilds(OS::DrawInfo* draw);
    void PostDrawChilds(OS::DrawInfo* draw);

    bool InterceptMouseEvents() const;
    bool HandleMouseEvent(const OS::MouseEvent& event);
    void Resync(Base::Model* model, const Base::ResyncMsg& msg);

    static View* Create(bool horizontalFlex=false, bool verticalFlex=false);
    static View* Create(Scrollable* object, bool horizontalFlex=false, bool verticalFlex=false);
  };
}

#endif
