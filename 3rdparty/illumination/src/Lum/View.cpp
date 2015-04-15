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

#include <Lum/View.h>

#include <cstdlib>
#include <cmath>

#include <Lum/Base/Util.h>

#include <Lum/Manager/Behaviour.h>

#include <Lum/PosMarker.h>
#include <Lum/Scroller.h>

namespace Lum {

  View::View()
  : object(NULL),
    hScroller(NULL),
    vScroller(NULL),
    kineticScroller(new KineticScroller()),
    hVisible(false),
    vVisible(false),
    kineticScrolling(true)
  {
    SetBackground(OS::display->GetFill(OS::Display::scrolledBackgroundFillIndex));
  }

  View::~View()
  {
    delete kineticScroller;
    delete hScroller;
    delete vScroller;
    delete object;
  }

  Lum::Object* View::GetMouseGrabFocusObject() const
  {
    return object;
  }

  bool View::VisitChildren(Visitor &visitor, bool onlyVisible)
  {
    if (hScroller!=NULL &&
        (!onlyVisible || hVisible)) {
      if (!visitor.Visit(hScroller)) {
        return false;
      }
    }

    if (vScroller!=NULL &&
        (!onlyVisible || vVisible)) {
      if (!visitor.Visit(vScroller)) {
        return false;
      }
    }

    if (object!=NULL &&
        !visitor.Visit(object)) {
      return false;
    }

    return true;
  }

  Scrollable* View::GetObject() const
  {
    return object;
  }

  void View::SetObject(Scrollable* object)
  {
    assert(object!=NULL);

    if (this->object!=NULL) {
      Forget(object->GetHAdjustment()->GetTotalModel());
      Forget(object->GetVAdjustment()->GetTotalModel());
      delete this->object;
    }

    this->object=object;

    object->SetParent(this);
    object->SetFlex(true,true);

    Observe(object->GetHAdjustment()->GetTotalModel());
    Observe(object->GetVAdjustment()->GetTotalModel());

    kineticScroller->SetScrollable(object);
  }

  bool View::SetModel(Base::Model* model)
  {
    return object->SetModel(model);
  }

  void View::SetKineticScrolling(bool kineticScrolling)
  {
    this->kineticScrolling=kineticScrolling;
  }

  void View::CalcSize()
  {
    assert(object!=NULL);

    object->SetScrollView(this);

    if (hScroller==NULL) {
      if (!Manager::Behaviour::Instance()->ScrollKnobActive() &&
          !object->RequiresKnobs()) {
        hScroller=new PosMarker(false);
      }
      else {
        hScroller=new Scroller(false);
      }

      hScroller->SetParent(this);
      hScroller->SetFlex(true,false);
      hScroller->SetModel(object->GetHAdjustment());
    }

    if (vScroller==NULL) {
      if (!Manager::Behaviour::Instance()->ScrollKnobActive() &&
          !object->RequiresKnobs()) {
        vScroller=new PosMarker(true);
      }
      else {
        vScroller=new Scroller(true);
      }

      vScroller->SetParent(this);
      vScroller->SetFlex(false,true);
      vScroller->SetModel(object->GetVAdjustment());
    }

    hScroller->CalcSize();
    vScroller->CalcSize();
    object->CalcSize();

    if (object->RequestedFocus()) {
      /* Delegate focusing to the real table */
      UnrequestFocus();
    }

    // Initial dimension equal to table dimension
    minWidth=object->GetOMinWidth();
    width=object->GetOWidth();
    minHeight=object->GetOMinHeight();
    height=object->GetOHeight();

    // Add Horizontal scroller
    minWidth=std::max(hScroller->GetOWidth(),minWidth);
    width=std::max(hScroller->GetOWidth(),width);
    minHeight+=hScroller->GetOHeight();


    // Add Vertical scroller
    minWidth+=vScroller->GetOMinWidth();
    width+=vScroller->GetOWidth();
    minHeight=std::max(vScroller->GetOHeight(),minHeight);
    height=std::max(vScroller->GetOHeight(),height);

    maxWidth=std::min(maxWidth,object->GetOMaxWidth()+vScroller->GetOMaxWidth());
    maxHeight=std::min(maxHeight,object->GetOMaxHeight()+hScroller->GetOMaxHeight());

    Group::CalcSize();
  }

  void View::Layout()
  {
    size_t tWidth,tHeight;

    hScroller->ResizeWidth(GetOWidth());
    vScroller->ResizeHeight(GetOHeight());

    tWidth=width;
    tHeight=height;
    if (object->GetBackground()!=NULL) {
      tWidth-=object->GetBackground()->leftBorder+object->GetBackground()->rightBorder;
      tHeight-=object->GetBackground()->topBorder+object->GetBackground()->bottomBorder;
    }
    object->GetODimension(tWidth,tHeight);
    if (object->GetBackground()!=NULL) {
      tWidth+=object->GetBackground()->leftBorder+object->GetBackground()->rightBorder;
      tHeight+=object->GetBackground()->topBorder+object->GetBackground()->bottomBorder;
    }

    hVisible=width<tWidth;
    vVisible=height<tHeight;

    // Showing one scrollbar reduces the visible table bar, so the other scrollbar
    // might be shown, too

    if (hVisible && !vVisible) {
      tHeight=height-hScroller->GetOHeight();
      if (object->GetBackground()!=NULL) {
        tWidth-=object->GetBackground()->leftBorder+object->GetBackground()->rightBorder;
        tHeight-=object->GetBackground()->topBorder+object->GetBackground()->bottomBorder;
      }
      object->GetODimension(tWidth,tHeight);
      if (object->GetBackground()!=NULL) {
        tWidth+=object->GetBackground()->leftBorder+object->GetBackground()->rightBorder;
        tHeight+=object->GetBackground()->topBorder+object->GetBackground()->bottomBorder;
      }
      vVisible=height-hScroller->GetOHeight()<tHeight;
    }
    else if (!hVisible && vVisible) {
      tWidth=width-vScroller->GetOWidth();
      if (object->GetBackground()!=NULL) {
        tWidth-=object->GetBackground()->leftBorder+object->GetBackground()->rightBorder;
        tHeight-=object->GetBackground()->topBorder+object->GetBackground()->bottomBorder;
      }
      object->GetODimension(tWidth,tHeight);
      if (object->GetBackground()!=NULL) {
        tWidth+=object->GetBackground()->leftBorder+object->GetBackground()->rightBorder;
        tHeight+=object->GetBackground()->topBorder+object->GetBackground()->bottomBorder;
      }
      hVisible=width-vScroller->GetOWidth()<tWidth;
    }

    tWidth=width;
    tHeight=height;

    // Calculate object dimensions

    if (hVisible) {
      tHeight-=hScroller->GetOHeight();
    }

    if (vVisible) {
      tWidth-=vScroller->GetOWidth();
    }

    if (hVisible) {
      hScroller->MoveResize(x,y+tHeight,tWidth,hScroller->GetOHeight());
    }
    else {
      hScroller->Hide();
    }

    if (vVisible) {
      vScroller->MoveResize(x+tWidth,y,vScroller->GetOWidth(),tHeight);
    }
    else {
      vScroller->Hide();
    }

    object->MoveResize(x,y,tWidth,tHeight);

    Group::Layout();
  }

  void View::PreDrawChilds(OS::DrawInfo* draw)
  {
    if (object==NULL) {
      return;
    }

    if (object->GetHAdjustment()==NULL ||
        object->GetVAdjustment()==NULL) {
      return;
    }

    if (!object->GetHAdjustment()->IsValid() ||
        !object->GetVAdjustment()->IsValid()) {
      return;
    }

    if (object->GetHAdjustment()->GetTotal()>width ||
        object->GetVAdjustment()->GetTotal()>height) {
      draw->PushClip(x,y,width,height);
    }
  }

  void View::PostDrawChilds(OS::DrawInfo* draw)
  {
    if (object==NULL) {
      return;
    }

    if (object->GetHAdjustment()==NULL ||
        object->GetVAdjustment()==NULL) {
      return;
    }

    if (!object->GetHAdjustment()->IsValid() ||
        !object->GetVAdjustment()->IsValid()) {
      return;
    }

    if (object->GetHAdjustment()->GetTotal()>width ||
        object->GetVAdjustment()->GetTotal()>height) {
      draw->PopClip();
    }
  }

  bool View::InterceptMouseEvents() const
  {
    return true;
  }

  bool View::HandleMouseEvent(const OS::MouseEvent& event)
  {
    if (object==NULL) {
      return false;
    }

    if (!object->RequiresKnobs() &&
        kineticScrolling &&
        kineticScroller->HandleMouseEvent(event)) {
      return true;
    }

    return Group::HandleMouseEvent(event);
  }

  void View::Resync(Base::Model* model, const Base::ResyncMsg& msg)
  {
    if (object==NULL) {
      return;
    }

    if (model==object->GetHAdjustment()->GetTotalModel() ||
        model==object->GetVAdjustment()->GetTotalModel()) {
      SetRelayout();
    }

    if (!object->RequiresKnobs() &&
        kineticScrolling) {
      kineticScroller->Resync(model,msg);
    }

    Group::Resync(model,msg);
  }

  View* View::Create(bool horizontalFlex, bool verticalFlex)
  {
    View *v;

    v=new View();
    v->SetFlex(horizontalFlex,verticalFlex);

    return v;
  }

  View* View::Create(Scrollable* object, bool horizontalFlex, bool verticalFlex)
  {
    View *v;

    v=new View();
    v->SetFlex(horizontalFlex,verticalFlex);
    v->SetObject(object);

    return v;
  }
}

