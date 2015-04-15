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

#include <Lum/KineticScroller.h>

#include <cstdlib>
#include <cmath>

#include <Lum/Base/Util.h>

#define MOVE_TIMER_MILLISECONDS      150
#define KINETIC_TIMER_MILLISECONDS    50
#define KINETIC_MIN_SPEED          100.0
#define KINETIC_MAX_SPEED          800.0
#define KINETIC_DECLINE             0.93

namespace Lum {

  KineticScroller::KineticScroller()
  : scrollable(NULL),
    state(pending),
    moveTimer(new Model::Action()),
    kineticTimer(new Model::Action())
  {
    Observe(moveTimer);
    Observe(kineticTimer);
  }

  void KineticScroller::SetScrollable(Scrollable* scrollable)
  {
    // Do not support dynamic updates
    assert(this->scrollable==NULL);

    this->scrollable=scrollable;

    Observe(scrollable->GetHAdjustment()->GetTopModel());
    Observe(scrollable->GetVAdjustment()->GetTopModel());
  }

  void KineticScroller::ReplayEvents()
  {
    class MouseEventVisitor : public Visitor
    {
    public:
      OS::MouseEventRef event;
      Lum::Object       *grab;

    public:
      bool Visit(Lum::Object* object)
      {
        object->VisitChildren(*this,true);

        if (grab!=NULL) {
          return false;
        }

        if (!object->InterceptMouseEvents() &&
            object->HandleMouseEvent(*event)) {
          grab=object;
        }

        return grab==NULL;
      }
    };


    if (eventCache.size()==0) {
      return;
    }

    MouseEventVisitor visitor;

    visitor.event=eventCache.front();
    visitor.grab=NULL;

    visitor.Visit(scrollable);

    if (visitor.grab==NULL) {
      return;
    }

    eventCache.pop_front();

    for (std::list<OS::MouseEventRef>::const_iterator iter=eventCache.begin();
         iter!=eventCache.end();
         ++iter) {
      OS::MouseEventRef event=*iter;

      visitor.grab->HandleMouseEvent(*event.Get());
    }

    eventCache.clear();
  }

  bool KineticScroller::CouldScroll() const
  {
    if (scrollable->GetHAdjustment()!=NULL &&
        scrollable->GetHAdjustment()->IsValid() &&
        !scrollable->GetHAdjustment()->IsCompletelyVisible()) {
      return true;
    }

    if (scrollable->GetVAdjustment()!=NULL &&
        scrollable->GetVAdjustment()->IsValid() &&
        !scrollable->GetVAdjustment()->IsCompletelyVisible()) {
      return true;
    }

    return false;
  }

  bool KineticScroller::IsScrolling() const
  {
    if (scrollable->GetHAdjustment()->IsValid() &&
        scrollable->GetHAdjustment()->GetVisible()<scrollable->GetHAdjustment()->GetTotal() &&
        dragDiffX>=(int)OS::display->GetTheme()->GetDragStartSensitivity()) {
      return true;
    }
    else if (scrollable->GetVAdjustment()->IsValid() &&
             scrollable->GetVAdjustment()->GetVisible()<scrollable->GetVAdjustment()->GetTotal() &&
             dragDiffY>=(int)OS::display->GetTheme()->GetDragStartSensitivity()) {
      return true;
    }
    else {
      return false;
    }
  }

  void KineticScroller::CalculateSpeed()
  {
    {
      Base::SystemTime diff(thisMoveX);

      diff.Sub(lastMoveX);

      long diffMillis=diff.GetTime()*1000+diff.GetMicroseconds()/1000;

      speedX=0;

      if (diffMillis!=0) {
        speedX=(lmX-tmX)*1000.0/diffMillis;
        speedX=Lum::Base::RoundRange(speedX,-KINETIC_MAX_SPEED,KINETIC_MAX_SPEED);
      }
      else if (lmX>tmX) {
        speedX=KINETIC_MAX_SPEED;
      }
      else if (lmX<tmX) {
        speedX=-KINETIC_MAX_SPEED;
      }

      if (std::abs(speedX)<KINETIC_MIN_SPEED) {
        speedX=0;
      }
    }

    {
      Base::SystemTime diff(thisMoveY);

      diff.Sub(lastMoveY);

      long diffMillis=diff.GetTime()*1000+diff.GetMicroseconds()/1000;

      speedY=0;

      if (diffMillis!=0) {
        speedY=(lmY-tmY)*1000.0/diffMillis;
        speedY=Lum::Base::RoundRange(speedY,-KINETIC_MAX_SPEED,KINETIC_MAX_SPEED);
      }
      else if (lmY>tmY) {
        speedY=KINETIC_MAX_SPEED;
      }
      else if (lmY<tmY) {
        speedY=-KINETIC_MAX_SPEED;
      }

      if (std::abs(speedY)<KINETIC_MIN_SPEED) {
        speedY=0;
      }
    }
  }

  bool KineticScroller::HandleMouseEvent(const OS::MouseEvent& event)
  {
    if (scrollable==NULL) {
      return false;
    }

    if (!CouldScroll()) {
      return false;
    }

    if (event.type==Lum::OS::MouseEvent::down &&
        scrollable->PointIsIn(event) &&
        event.button==Lum::OS::MouseEvent::button1) {
      dragStartX=event.x;
      dragStartY=event.y;
      dragDiffX=0;
      dragDiffY=0;

      topStartX=scrollable->GetHAdjustment()->GetTop();
      topStartY=scrollable->GetVAdjustment()->GetTop();

      thisMoveX.SetToNow();
      lastMoveX=thisMoveX;
      tmX=event.x;
      lmX=tmX;

      thisMoveY.SetToNow();
      lastMoveY=thisMoveY;
      tmY=event.y;
      lmY=tmY;

      state=scrollMaybe;
      eventCache.clear();
      eventCache.push_back(const_cast<OS::MouseEvent*>(&event));

      OS::display->AddTimer(0,MOVE_TIMER_MILLISECONDS*1000,moveTimer);

      return true;
    }
    else if (event.IsGrabEnd() &&
             event.qualifier==OS::qualifierButton1) {

      if (tmX!=event.x) {
        lmX=tmX;
        tmX=event.x;
        lastMoveX=thisMoveX;
        thisMoveX.SetToNow();
      }

      if (tmY!=event.y) {
        lmY=tmY;
        tmY=event.y;
        lastMoveY=thisMoveY;
        thisMoveY.SetToNow();
      }

      if (state==pending) {
        eventCache.clear();
        eventCache.push_back(const_cast<OS::MouseEvent*>(&event));
        ReplayEvents();
      }
      else if (state==scrollMaybe) {
        OS::display->RemoveTimer(moveTimer);
        state=pending;
        eventCache.push_back(const_cast<OS::MouseEvent*>(&event));
        ReplayEvents();

        return true;
      }
      else if (state==scrolling) {
        state=kinetic;
        CalculateSpeed();
        OS::display->AddTimer(0,KINETIC_TIMER_MILLISECONDS*1000,kineticTimer);
        return true;
      }
      else { // state==kinetic

        state=pending;

        return true;
      }
    }
    else if (event.type==Lum::OS::MouseEvent::move &&
             event.IsGrabed()) {
      if (tmX!=event.x) {
        lmX=tmX;
        tmX=event.x;
        lastMoveX=thisMoveX;
        thisMoveX.SetToNow();
      }

      if (tmY!=event.y) {
        lmY=tmY;
        tmY=event.y;
        lastMoveY=thisMoveY;
        thisMoveY.SetToNow();
      }

      if (state==pending) {
        return false;
      }
      else if (state==scrollMaybe) {
        dragDiffX=std::max(dragDiffX,std::abs(dragStartX-event.x));
        dragDiffY=std::max(dragDiffY,std::abs(dragStartY-event.y));

        if (IsScrolling()) {
          state=scrolling;
          eventCache.clear();
          OS::display->RemoveTimer(moveTimer);
        }
        else {
          eventCache.push_back(const_cast<OS::MouseEvent*>(&event));
        }
        return true;
      }
      else if (state==scrolling) {
        int posX,posY;

        posX=topStartX-(event.x-dragStartX);
        posY=topStartY-(event.y-dragStartY);

        if (posX<=0) {
          posX=1;
        }
        else if (scrollable->GetHAdjustment()!=NULL &&
                 scrollable->GetHAdjustment()->IsValid() &&
                 posX>(int)(scrollable->GetHAdjustment()->GetTotal()-
                            scrollable->GetHAdjustment()->GetVisible()+1)) {
          posX=scrollable->GetHAdjustment()->GetTotal()-
               scrollable->GetHAdjustment()->GetVisible()+1;
        }

        if (posY<=0) {
          posY=1;
        }
        else if (scrollable->GetVAdjustment()!=NULL &&
                 scrollable->GetVAdjustment()->IsValid() &&
                 posY>(int)(scrollable->GetVAdjustment()->GetTotal()-
                            scrollable->GetVAdjustment()->GetVisible()+1)) {
          posY=scrollable->GetVAdjustment()->GetTotal()-
               scrollable->GetVAdjustment()->GetVisible()+1;
        }

        if (scrollable->GetHAdjustment()!=NULL &&
            scrollable->GetHAdjustment()->IsValid()) {
          scrollable->GetHAdjustment()->SetTop(posX);
        }

        if (scrollable->GetVAdjustment()!=NULL &&
            scrollable->GetVAdjustment()->IsValid()) {
          scrollable->GetVAdjustment()->SetTop(posY);
        }
      }

      return true;
    }

    return false;
  }

  void KineticScroller::Resync(Base::Model* model, const Base::ResyncMsg& msg)
  {
    if (scrollable==NULL) {
      return;
    }

    if (model==moveTimer && moveTimer->IsFinished()) {
      if (state==scrollMaybe) {
        if (IsScrolling()) {
          state=scrolling;
          eventCache.clear();
        }
        else {
          state=pending;
          ReplayEvents();
        }
      }
    }
    else if (model==kineticTimer &&
             kineticTimer->IsFinished() &&
             state==kinetic) {
      int pixelX=(int)rint(speedX*KINETIC_TIMER_MILLISECONDS/1000);
      int pixelY=(int)rint(speedY*KINETIC_TIMER_MILLISECONDS/1000);

      if (scrollable->GetHAdjustment()!=NULL &&
          scrollable->GetHAdjustment()->IsValid()) {
        if (pixelX>0) {
          scrollable->GetHAdjustment()->IncTop((size_t)pixelX);
        }
        else if (pixelX<0) {
          scrollable->GetHAdjustment()->DecTop((size_t)std::abs(pixelX));
        }
      }

      if (scrollable->GetVAdjustment()!=NULL &&
          scrollable->GetVAdjustment()->IsValid()) {
        if (pixelY>0) {
          scrollable->GetVAdjustment()->IncTop((size_t)pixelY);
        }
        else if (pixelY<0) {
          scrollable->GetVAdjustment()->DecTop((size_t)std::abs(pixelY));
        }
      }

      if (std::abs(speedX)<1 && std::abs(speedY)<1) {
        state=pending;
      }
      else {
        speedX=speedX*KINETIC_DECLINE;
        speedY=speedY*KINETIC_DECLINE;

        OS::display->AddTimer(0,KINETIC_TIMER_MILLISECONDS*1000,kineticTimer);
      }
    }
    else if (model==scrollable->GetHAdjustment()->GetTopModel() ||
             model==scrollable->GetVAdjustment()->GetTopModel()) {
      if (state==scrollMaybe) {
        state=pending;
        eventCache.clear();
      }
    }
  }
}

