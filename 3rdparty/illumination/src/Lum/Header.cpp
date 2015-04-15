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

#include <Lum/Header.h>

#include <algorithm>

#include <Lum/Base/Size.h>

#include <Lum/OS/Theme.h>

#include <Lum/Text.h>

namespace Lum {

  class HeaderEntry : public Object
  {
  private:
    Object *label;

  public:
    HeaderEntry(const std::wstring& label, size_t current, size_t max)
    : label(new Lum::Text(label))
    {
      if (max==1) {
        SetBackground(OS::display->GetFill(OS::Display::columnBackgroundFillIndex));
      }
      else if (current==1) {
        SetBackground(OS::display->GetFill(OS::Display::columnLeftBackgroundFillIndex));
      }
      else if (current==max) {
        SetBackground(OS::display->GetFill(OS::Display::columnRightBackgroundFillIndex));
      }
      else {
        SetBackground(OS::display->GetFill(OS::Display::columnMiddleBackgroundFillIndex));
      }

      SetRedrawOnMouseActive(true);
    }

    ~HeaderEntry()
    {
      delete label;
    }

    void SetLabel(Object *label)
    {
      delete label;
      this->label=label;

      Redraw();
    }

    void CalcSize()
    {
      label->SetFlex(true,true);
      label->SetParent(this);
      label->CalcSize();

      minWidth=1;
      minHeight=label->GetOHeight();

      width=label->GetOWidth()+2*OS::display->GetSpaceHorizontal(OS::Display::spaceObjectBorder);
      height=minHeight;

      Object::CalcSize();
    }

    void Layout()
    {
      label->MoveResize(x+OS::display->GetSpaceHorizontal(OS::Display::spaceObjectBorder),
                        y,
                        width-2*OS::display->GetSpaceHorizontal(OS::Display::spaceObjectBorder),
                        height);
      label->Layout();

      Object::Layout();
    }

    void PrepareForBackground(OS::DrawInfo* draw)
    {
      draw->activated=IsMouseActive();
    }

    void Draw(OS::DrawInfo* draw,
              int x, int y, size_t w, size_t h)
    {
      Object::Draw(draw,x,y,w,h);

      if (!OIntersect(x,y,w,h)) {
        return;
      }

      draw->disabled=false;
      draw->activated=IsMouseActive();

      label->Draw(draw,x,y,w,h);

      draw->activated=false;
    }
  };

  Header::Header()
  : model(NULL),adjustment(NULL)
  {
    SetBackground(OS::display->GetFill(OS::Display::backgroundFillIndex));
  }

  Header::~Header()
  {
    for (size_t x=0; x<objects.size(); x++) {
      delete objects[x];
    }
  }

  bool Header::VisitChildren(Visitor &visitor, bool /*onlyVisible*/)
  {
    for (size_t x=0; x<objects.size(); x++) {
      if (!visitor.Visit(objects[x])) {
        return false;
      }
    }

    return true;
  }

  void Header::SetAdjustment(Model::Adjustment* adjustment)
  {
    assert(adjustment!=NULL);

    if (this->adjustment.Valid()) {
      Forget(this->adjustment->GetTopModel());
    }

    this->adjustment=adjustment;

    if (this->adjustment.Valid()) {
      Observe(this->adjustment->GetTopModel());
    }
  }

  void Header::CalcSize()
  {

    if (!model.Valid()) {
      width=1;
      height=OS::display->GetFont()->height;
    }
    else {
      objects.resize(model->GetColumns());

      for (size_t x=0; x<model->GetColumns(); x++) {
        objects[x]=new HeaderEntry(model->GetLabel(x),x+1,model->GetColumns());
        objects[x]->SetFlex(true,true);
        objects[x]->SetParent(this);
        objects[x]->CalcSize();

        if (model->GetColumnWidth(x)==0) {
          model->SetColumnWidth(x,objects[x]->GetOWidth());
        }
        height=std::max(height,objects[x]->GetOMinHeight());
      }

      width=0;
    }

    Control::CalcSize();
  }

  void Header::Layout()
  {
    int xPos;

    if (adjustment.Valid()) {
      xPos=x-adjustment->GetTop()+1;
    }
    else {
      xPos=x;
    }

    for (size_t count=0; count<objects.size(); count++) {
      objects[count]->MoveResize(xPos,y,model->GetColumnWidth(count),height);
      xPos+=model->GetColumnWidth(count);
    }

    Control::Layout();
  }

  bool Header::SetModel(Base::Model* model)
  {
    this->model=dynamic_cast<Model::Header*>(model);

    Control::SetModel(this->model);

    return this->model.Valid();
  }

  void Header::PreDrawChilds(OS::DrawInfo* draw)
  {
    draw->PushClip(GetOX(),GetOY(),GetOWidth(),GetOHeight());
  }

  void Header::PostDrawChilds(OS::DrawInfo* draw)
  {
    draw->PopClip();
  }

  void Header::Draw(OS::DrawInfo* draw,
                    int x, int y, size_t w, size_t h)
  {
    Control::Draw(draw,x,y,w,h);

    if (!OIntersect(x,y,w,h)) {
      return;
    }

    /* --- */

    if (adjustment.Valid()) {
      size_t columnsWidth=0;

      for (size_t count=0; count<objects.size(); count++) {
        columnsWidth+=model->GetColumnWidth(count);
      }

      adjustment->SetDimension(width,columnsWidth);
    }
  }

  bool Header::HandleMouseEvent(const OS::MouseEvent& event)
  {
    if (!visible || !model.Valid() || !model->IsEnabled()) {
      return false;
    }

    if (event.type==OS::MouseEvent::down && PointIsIn(event) && event.button==OS::MouseEvent::button1) {
      int pos;

      pos=x;
      if (adjustment.Valid()) {
        pos-=adjustment->GetTop()-1;
      }

      for (size_t current=0; current<objects.size(); current++) {
        pos+=model->GetColumnWidth(current);

        if (abs(pos-event.x)<=2) {
          if (GetWindow()->IsDoubleClicked()) {
            model->RequestFit(current);
          }
          else {
            offset=event.x-pos;
            grabStart=pos-model->GetColumnWidth(current);
            grabColumn=current;
          }

          return true;
        }
      }
    }
    else if (event.type==OS::MouseEvent::move && event.IsGrabed()) {
      if (event.x-offset>grabStart+(int)OS::display->GetTheme()->GetDragStartSensitivity()) {
        model->SetColumnWidth(grabColumn,event.x-offset-grabStart);
      }
    }

    return false;
  }

  void Header::Resync(Base::Model* model, const Base::ResyncMsg& msg)
  {
    if (model==this->model) {
      const Model::Header::ResizedColumn *resized;

      if ((resized=dynamic_cast<const Model::Header::ResizedColumn*>(&msg))!=NULL) {
        // TODO: Just do a Redraw over the to be redrawn area...
        SetRelayout();
        Redraw();
      }
      else {
        SetRelayout();
        Redraw();
      }
    }
    else if (adjustment.Valid() &&
             model==adjustment->GetTopModel()) {
      SetRelayout();
      Redraw();
    }
  }
}
