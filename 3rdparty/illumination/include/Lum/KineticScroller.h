#ifndef LUM_KINETICSCROLLER_H
#define LUM_KINETICSCROLLER_H

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

#include <list>

#include <Lum/Base/DateTime.h>
#include <Lum/Base/Model.h>

#include <Lum/Model/Action.h>

#include <Lum/Object.h>
#include <Lum/Scroller.h>

namespace Lum {

  class LUMAPI KineticScroller : public Base::MsgObject
  {
  private:
    enum State {
      pending,     //! We are waiting for inoput
      scrollMaybe, //! Some input happened, but we are unsure if we are already scrolling
      scrolling,   //! Scrolling activated
      kinetic      //! User raised finger again but we are stills crolling until stop
    };

  private:
    Scrollable                   *scrollable;

    int                          dragStartX,dragStartY;
    int                          dragDiffX,dragDiffY;

    int                          lmX,lmY;
    int                          tmX,tmY;
    Base::SystemTime             lastMoveX;
    Base::SystemTime             thisMoveX;
    Base::SystemTime             lastMoveY;
    Base::SystemTime             thisMoveY;
    double                       speedX;
    double                       speedY;

    int                          topStartX,topStartY;

    State                        state;

    Model::ActionRef             moveTimer;
    Model::ActionRef             kineticTimer;

    std::list<OS::MouseEventRef> eventCache;

  private:
    void ReplayEvents();
    bool CouldScroll() const;
    bool IsScrolling() const;
    void CalculateSpeed();

  public:
    KineticScroller();

    void SetScrollable(Scrollable* scrollable);

    bool HandleMouseEvent(const OS::MouseEvent& event);
    void Resync(Base::Model* model, const Base::ResyncMsg& msg);
  };
}

#endif
