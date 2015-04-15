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

#include <Lum/Tab.h>

#include <Lum/OS/Theme.h>

#include <Lum/Dialog.h>
#include <Lum/Text.h>

namespace Lum {

  /* ----------------------- */

  class TabButton : public Control
  {
  private:
    Tab              *tab;        //! The parent tab control
    Object           *image;      //! The label of the button
    Model::NumberRef model;       //! The model to decided if we are selected or not
    size_t           index;       //! Our index value
    size_t           orgIndex;    //! The original index value before we got selected

  public:
    TabButton(Tab* tab,
              size_t index,
              Object* object,
              const Model::NumberRef& model);
    ~TabButton();

    bool HandlesKeyFocus() const;

    void CalcSize();
    void Layout();
    void PrepareForBackground(OS::DrawInfo* draw);
    void Draw(OS::DrawInfo* draw,
              int x, int y, size_t w, size_t h);
    void Hide();

    bool HandleMouseEvent(const OS::MouseEvent& event);
    bool HandleKeyEvent(const OS::KeyEvent& event);

    OS::Color GetTextColor(OS::DrawInfo *draw, const void* child) const;

    void Resync(Base::Model* model, const Base::ResyncMsg& msg);
  };

  TabButton::TabButton(Tab* tab,
                       size_t index,
                       Object* object,
                       const Model::NumberRef& model)
  : tab(tab),
    image(object),
    model(model),
    index(index)
  {
    assert(tab!=NULL);
    assert(image!=NULL);
    assert(model.Valid());

    SetBackground(OS::display->GetFill(OS::Display::tabRiderBackgroundFillIndex));

    this->model=model;
    SetModel(model.Get());

    if (!OS::display->GetTheme()->FocusOnEditableControlsOnly()) {
      SetCanFocus(true);
      RequestFocus();
    }

    image->SetParent(this);
  }

  TabButton::~TabButton()
  {
    delete image;
  }

  bool TabButton::HandlesKeyFocus() const
  {
    return CanFocus() &&
           RequestedFocus() &&
           IsVisible() &&
           model.Valid() &&
           model->IsEnabled() &&
           !model->IsNull() &&
           model->GetUnsignedLong()==index;
  }

  void TabButton::CalcSize()
  {
    /* Let the frame calculate its size */
    width=2*OS::display->GetSpaceHorizontal(OS::Display::spaceObjectBorder);
    height=2*OS::display->GetSpaceVertical(OS::Display::spaceObjectBorder);

    /* Our minimal size is equal to the normal size */
    minWidth=width;
    minHeight=height;

    image->CalcSize();
    width+=image->GetOWidth();
    height+=image->GetOHeight();
    minWidth+=image->GetOMinWidth();
    minHeight+=image->GetOMinHeight();

    /* We *must* call CalcSize of our superclass! */
    Control::CalcSize();
  }

  bool TabButton::HandleMouseEvent(const OS::MouseEvent& event)
  {
    /* It makes no sense to get the focus if we are currently not visible */
    if (!visible ||
        !model.Valid() ||
        !model->IsEnabled()) {
      return false;
    }

    /*
      When the left mousebutton gets pressed without any qualifier
      in the bounds of our button...
    */

    if (event.type==OS::MouseEvent::down &&
        PointIsIn(event) &&
        event.button==OS::MouseEvent::button1) {
      orgIndex=model->GetUnsignedLong();
      model->Set(index);
      return true;
    }
    else if (event.IsGrabEnd()) {
      /*
        If the users released the left mousebutton over our bounds we really
        got selected.
      */
      if (PointIsIn(event)) {
        model->Set(index);
      }
      else {
        model->Set(orgIndex);
      }
    }
    else if (event.type==OS::MouseEvent::move && event.IsGrabed()) {
      if (PointIsIn(event)) {
        model->Set(index);
      }
      else {
        model->Set(orgIndex);
      }

      return true;
    }

    return false;
  }

  bool TabButton::HandleKeyEvent(const OS::KeyEvent& event)
  {
    if (event.type==OS::KeyEvent::down) {
      if (event.key==OS::keySpace) {
        model->Set(index);

        return true;
      }
      else if (event.key==OS::keyLeft) {
        tab->ActivatePreviousTab();
      }
      else if (event.key==OS::keyRight) {
        tab->ActivateNextTab();
      }
    }

    return false;
  }

  OS::Color TabButton::GetTextColor(OS::DrawInfo *draw, const void* child) const
  {
    if (draw->selected) {
      return OS::display->GetColor(OS::Display::tabTextSelectColor);
    }
    else {
      return OS::display->GetColor(OS::Display::tabTextColor);
    }
  }

  /*
     We tell the image to resize themself to
     our current bounds. Our bounds could have changed
     because Resize may have been called by some layout-objects
     between TabButton.CalcSize and TabButton.Draw.
   */
  void TabButton::Layout()
  {
    image->Resize(width-2*OS::display->GetSpaceHorizontal(OS::Display::spaceObjectBorder),
                  height-2*OS::display->GetSpaceVertical(OS::Display::spaceObjectBorder));
    image->Move(x+(width-image->GetOWidth()) / 2,
                y+(height-image->GetOHeight()) / 2);
    image->Layout();

    Control::Layout();
  }

  void TabButton::PrepareForBackground(OS::DrawInfo* draw)
  {
    draw->activated=IsMouseActive();
    draw->selected=model.Valid() && !model->IsNull() && model->GetUnsignedLong()==index;
    draw->focused=HasFocus();
  }

  void TabButton::Draw(OS::DrawInfo* draw,
                       int x, int y, size_t w, size_t h)
  {
    Control::Draw(draw,x,y,w,h); /* We must call Draw of our superclass */

    if (!OIntersect(x,y,w,h)) {
      return;
    }

    /* --- */

    draw->activated=IsMouseActive();
    draw->selected=model.Valid() && !model->IsNull() && model->GetUnsignedLong()==index;
    draw->focused=HasFocus();

    image->Draw(draw,x,y,w,h);

    draw->activated=false;
    draw->selected=false;
    draw->focused=false;
  }

  void TabButton::Hide()
  {
    if (visible) {
      /* Hide the image */
      image->Hide();

      /* hide the frame */
      Control::Hide();
    }
  }

  void TabButton::Resync(Base::Model* model, const Base::ResyncMsg& msg)
  {
    if (model==this->model) {
      Redraw();
    }

    Control::Resync(model,msg);
  }

  /* ----------------------- */

  Tab::Tab()
  : mw(0),
    mh(0),
    lastDrawn(NULL),
    frameGroup(new FrameGroup()),
    multi(new Multi())
  {
    SetCanFocus(true);

    SetModel(new Model::Number(0ul));

    frameGroup->SetFrame(new OS::EmptyFrame());
    frameGroup->SetBackground(OS::display->GetFill(OS::Display::tabBackgroundFillIndex));
    frameGroup->SetFlex(true,true);
    frameGroup->SetSpace(true);

    multi->SetFlex(true,true);
    multi->SetModel(GetModel());

    frameGroup->SetObject(multi);

    Observe(model);
  }

  Tab::~Tab()
  {
    for (std::list<Object*>::iterator iter=list.begin();
         iter!=list.end();
         ++iter) {
      delete *iter;
    }

    delete frameGroup;
  }

  bool Tab::VisitChildren(Visitor &visitor, bool onlyVisible)
  {
    if (!visitor.Visit(frameGroup)) {
      return false;
    }

    for (std::list<Object*>::iterator iter=list.begin(); iter!=list.end(); ++iter) {
      if (!visitor.Visit(*iter)) {
        return false;
      }
    }

    return true;
  }

  Tab* Tab::Add(Object* label, Object* object)
  {
    assert(label!=NULL);
    assert(object!=NULL);

    Object *rider=new TabButton(this,list.size(),label,model);

    rider->SetFlex(true,true);

    list.push_back(rider);

    multi->Append(object);

    return this;
  }

  Tab* Tab::Add(const std::wstring& label, Object* object)
  {
    return Add(new Text(label),object);
  }

  void Tab::ActivatePreviousTab()
  {
    if (list.size()<=1) {
      return;
    }

    if (model->GetUnsignedLong()==0) {
      model->Set(list.size()-1);
    }
    else {
      model->Dec();
    }
  }

  void Tab::ActivateNextTab()
  {
    if (list.size()<=1) {
      return;
    }

    if (model->GetUnsignedLong()==list.size()-1) {
      model->Set(0);
    }
    else {
      model->Inc();
    }
  }

  bool Tab::SetModel(Base::Model* model)
  {
    this->model=dynamic_cast<Model::Number*>(model);

    Control::SetModel(this->model);

    multi->SetModel(GetModel());

    return this->model.Valid();
  }

  void Tab::CalcSize()
  {
    size_t ow,oh;

    width=0;
    height=0;
    mw=0;
    mh=0;
    ow=0;
    oh=0;

    for (std::list<Object*>::iterator iter=list.begin();
         iter!=list.end();
         ++iter) {
      (*iter)->SetParent(this);
      (*iter)->CalcSize();
      mw=std::max(mw,(*iter)->GetOWidth());
      mh=std::max(mh,(*iter)->GetOHeight());
    }

    frameGroup->SetParent(this);
    frameGroup->CalcSize();
    ow=frameGroup->GetOWidth();
    oh=frameGroup->GetOHeight();

    OS::FrameRef tabFrame=OS::display->GetFrame(OS::Display::tabFrameIndex);

    mh=std::max(mh,tabFrame->topBorder);

    height=mh+oh+tabFrame->minHeight;
    width=std::max(ow+tabFrame->minWidth,
                   list.size()*mw+
                   OS::display->GetTheme()->GetFirstTabOffset()+
                   OS::display->GetTheme()->GetLastTabOffset());

    minWidth=width;
    minHeight=height;

    Control::CalcSize();
  }

  void Tab::Layout()
  {
    int     xPos,yPos;
    size_t  width,height;
    size_t  pos;

    OS::FrameRef tabFrame=OS::display->GetFrame(OS::Display::tabFrameIndex);

    xPos=x+tabFrame->leftBorder;
    yPos=y+mh+tabFrame->topBorder;
    width=this->width-tabFrame->minWidth;
    height=this->height-mh-tabFrame->minHeight;

    pos=x+OS::display->GetTheme()->GetFirstTabOffset();
    for (std::list<Object*>::iterator iter=list.begin(); iter!=list.end(); ++iter) {
      // Position rider relative to last rider
      (*iter)->MoveResize(pos,y,mw,mh);
      (*iter)->Layout();
      pos+=mw;
    }

    frameGroup->Resize(width,height);
    frameGroup->Move(xPos+(width-frameGroup->GetOWidth()) / 2,
                     yPos+(height-frameGroup->GetOHeight()) / 2);
    frameGroup->Layout();

    Control::Layout();
  }

  void Tab::Draw(OS::DrawInfo* draw,
                 int x, int y, size_t w, size_t h)
  {
    if (lastDrawn!=NULL) {
     /*
        This is a little trick:
        We must hide the old object, but we do not want it to clean up the
        covered area, since it will be overwritten by the new object anyway.
        So we create a clipping region that completely covers the to be drawn
        area :-)
      */
      draw->PushClipBegin(lastDrawn->GetOX(),lastDrawn->GetOY(),
                          lastDrawn->GetOWidth(),lastDrawn->GetOHeight());
      draw->SubClipRegion(lastDrawn->GetOX(),lastDrawn->GetOY(),
                          lastDrawn->GetOWidth(),lastDrawn->GetOHeight());
      draw->PushClipEnd();
      lastDrawn->Hide();
      draw->PopClip();
      lastDrawn=NULL;
    }

    Control::Draw(draw,x,y,w,h);

    if (!OIntersect(x,y,w,h)) {
      return;
    }

    /* --- */

    OS::FrameRef tabFrame=OS::display->GetFrame(OS::Display::tabFrameIndex);

    // Draw frame around object area
    tabFrame->SetGap(OS::display->GetTheme()->GetFirstTabOffset()+model->GetUnsignedLong()*mw,mw,0);

    tabFrame->Draw(draw,
                   this->x,
                   this->y+mh,
                   this->width,
                   this->height-mh);
  }

  void Tab::Resync(Base::Model* model, const Base::ResyncMsg& msg)
  {
    if (model==this->model &&
        visible) {
      unsigned long                pos=this->model->GetUnsignedLong();
      std::list<Object*>::iterator iter=list.begin();
      Dialog                       *dialog=dynamic_cast<Dialog*>(GetWindow()->GetMaster());

      Redraw();

      if (dialog!=NULL) {
        while (pos>0) {
          ++iter;
          --pos;
        }

        dialog->SetFocus((*iter));
      }
    }

    Control::Resync(model,msg);
  }

  Tab* Tab::Create(bool horizontalFlex, bool verticalFlex)
  {
    Tab *object;

    object=new Tab();
    object->SetFlex(horizontalFlex,verticalFlex);

    return object;
  }
}
