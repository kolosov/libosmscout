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

#include <Lum/Knob.h>

#include <Lum/Base/Util.h>

#include <Lum/OS/Display.h>

namespace Lum {

  Knob::Knob()
  : knob(new Image()),
    offset(0),
    corr(0),
    adjustment(NULL),
    incAction(new Model::Action),
    decAction(new Model::Action),
    vert(true),
    selected(false)
  {
    SetCanFocus(true);
    SetRedrawOnMouseActive(true);

    knob->SetParent(this);
    knob->SetFlex(true,true);

    Observe(incAction);
    Observe(decAction);
  }

  Knob::~Knob()
  {
    delete knob;
  }

  /**
    Set the direction of the knob.
  */
  void Knob::Set(bool vert)
  {
    this->vert=vert;
  }

  /**
    Don't use it!
  */
  void Knob::SetOffset(int offset)
  {
    corr=offset;
  }

  bool Knob::SetModel(Base::Model* model)
  {
    adjustment=dynamic_cast<Model::Adjustment*>(model);

    Control::SetModel(adjustment);

    return adjustment.Valid();
  }

  void Knob::CalcSize()
  {
    if (vert) {
      SetBackground(OS::display->GetFill(OS::Display::vknobBoxFillIndex));
      knob->SetImage(OS::display->GetImage(OS::Display::vknobImageIndex));
    }
    else {
      SetBackground(OS::display->GetFill(OS::Display::hknobBoxFillIndex));
      knob->SetImage(OS::display->GetImage(OS::Display::hknobImageIndex));
    }

    knob->CalcSize();

    if (vert) {
      width=knob->GetOWidth();
      height=knob->GetOWidth();
    }
    else {
      width=knob->GetOHeight();
      height=knob->GetOHeight();
    }

    minWidth=width;
    minHeight=height;

    Control::CalcSize();
  }

  void Knob::HandleMouseMovement(int x, int y)
  {
    if (vert) {
      if (y-offset<=this->y) {
        adjustment->SetTop(1-corr);
      }
      else if (y-offset>=this->y+(int)this->height-(int)knob->GetOHeight()) {
        adjustment->SetTop(adjustment->GetTotal()+corr-adjustment->GetVisible()+1);
      }
      else {
        adjustment->SetTop(((y-offset-this->y)*adjustment->GetTotal())/(this->height)+1);
      }
    }
    else {
      if (x-offset<=this->x) {
        adjustment->SetTop(1-corr);
      }
      else if (x-offset>=this->x+(int)this->width-(int)knob->GetOWidth()) {
        adjustment->SetTop(adjustment->GetTotal()+corr-adjustment->GetVisible()+1);
      }
      else {
        adjustment->SetTop(((x-offset-this->x)*adjustment->GetTotal())/(this->width)+1);
      }
    }
  }

  bool Knob::HandleMouseEvent(const OS::MouseEvent& event)
  {
    if (!visible ||
        !adjustment.Valid() ||
        !adjustment->IsValid() ||
        !adjustment->IsEnabled()) {
      return false;
    }

    if (adjustment->GetVisible()==adjustment->GetTotal()) {
      return false;
    }

    if (event.type==OS::MouseEvent::down && PointIsIn(event) && event.button==OS::MouseEvent::button1) {
      if (knob->PointIsIn(event)) {
        if (vert) {
          offset=event.y-knob->GetOY();
        }
        else {
          offset=event.x-knob->GetOX();
        }
        selected=true;
        Redraw();
      }
      else {
        offset=-1;
        if (vert) {
          if (event.y<knob->GetOY()) {
            adjustment->PageBack();
          }
          else if (event.y>knob->GetOY()+(int)knob->GetOHeight()) {
            adjustment->PageForward();
          }
        }
        else {
          if (event.x<knob->GetOX()) {
            adjustment->PageBack();
          }
          else if (event.x>knob->GetOX()+(int)knob->GetOWidth()) {
            adjustment->PageForward();
          }
        }
      }
      return true;
    }
    else if (event.IsGrabEnd()) {
      if (offset>=0) {
        HandleMouseMovement(event.x,event.y);
      }

      if (selected) {
        selected=false;
        Redraw();
      }
    }
    else if (event.type==OS::MouseEvent::move && event.IsGrabed() && offset>=0) {
      HandleMouseMovement(event.x,event.y);
      return true;
    }

    return false;
  }

  bool Knob::HandleKeyEvent(const OS::KeyEvent& event)
  {
    if (!visible ||
        !adjustment.Valid() ||
        !adjustment->IsValid() ||
        !adjustment->IsEnabled()) {
      return false;
    }

    if (event.type==OS::KeyEvent::down) {
      if ((vert && event.key==OS::keyUp) ||
          (!vert && event.key==OS::keyLeft)) {
        decAction->Trigger();
        return true;
      }
      else if ((vert && event.key==OS::keyDown) ||
               (!vert && event.key==OS::keyRight)) {
        incAction->Trigger();
        return true;
      }
    }

    return false;
  }

  void Knob::Draw(OS::DrawInfo* draw,
                  int x, int y, size_t w, size_t h)
  {
    Control::Draw(draw,x,y,w,h);

    if (!OIntersect(x,y,w,h)) {
      return;
    }

    /* --- */

    if (!adjustment.Valid() ||
        !adjustment->IsValid() ||
        adjustment->GetVisible()==adjustment->GetTotal()) {
      return;
    }

    size_t       kSize,bSize;
    int          kStart;

    if (IsMouseActive()) {
      draw->activated=true;
    }

    if (vert) {
      bSize=height;
    }
    else {
      bSize=width;
    }

    if (adjustment->GetTotal()+corr==0) {
      kSize=bSize;
      kStart=0;
    }
    else {
      kSize=Base::RoundDiv((bSize*adjustment->GetVisible()),adjustment->GetTotal()+corr);
      kStart=Base::RoundDiv(bSize*(adjustment->GetTop()+corr-1),adjustment->GetTotal()+corr);
    }

    if (vert) {
      if (kSize<knob->GetOMinWidth()) {
        kSize=knob->GetOMinWidth();
      }
    }
    else {
      if (kSize<knob->GetOMinHeight()) {
        kSize=knob->GetOMinHeight();
      }
    }

    if (kSize>bSize) {
      kSize=bSize;
    }

    if (kStart+kSize>bSize) {
      kStart=bSize-kSize;
    }

    if (vert) {
      knob->MoveResize(this->x,this->y+kStart,width,kSize);
    }
    else {
      knob->MoveResize(this->x+kStart,this->y,kSize,height);
    }

    draw->selected=selected;
    knob->Draw(draw,knob->GetOX(),knob->GetOY(),knob->GetOWidth(),knob->GetOHeight());
    draw->selected=false;

    draw->activated=false;
  }

  void Knob::Hide()
  {
    if (visible) {
      knob->Hide();
      Control::Hide();
    }
  }

  Model::Action* Knob::GetIncAction() const
  {
    return incAction.Get();
  }

  Model::Action* Knob::GetDecAction() const
  {
    return decAction.Get();
  }

  void Knob::Resync(Base::Model* model, const Base::ResyncMsg& msg)
  {
    if (model==incAction &&
        incAction->IsFinished()) {
      if (adjustment.Valid()) {
        adjustment->IncTop();
      }
    }
    else if (model==decAction &&
             decAction->IsFinished()) {
      if (adjustment.Valid()) {
        adjustment->DecTop();
      }
    }
    else if (model==adjustment &&
             visible &&
             adjustment.Valid() &&
             adjustment->IsEnabled()) {
      Redraw();
    }
  }
}
