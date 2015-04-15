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

#include <Lum/State.h>

namespace Lum {

  State::State(bool active)
  : boolModel(NULL),numberModel(NULL),index(1),active(active)
  {
    SetCanFocus(active);
    if (active && !OS::display->GetTheme()->FocusOnEditableControlsOnly()) {
      RequestFocus();
      SetRedrawOnMouseActive(true);
    }
    SetRequestLabelFocus(true);
  }

  State::~State()
  {
    // no code
  }

  bool State::IsIn(const OS::MouseEvent& event) const
  {
    return PointIsIn(event) || (label!=NULL && label->PointIsIn(event));
  }

  void State::SetIndex(int index)
  {
    this->index=index;
  }

  /**
    State either expects an boolean model or an SizeT model as value models.
   */
  bool State::SetModel(Base::Model* model)
  {
    boolModel=dynamic_cast<Model::Boolean*>(model);
    numberModel=dynamic_cast<Model::Number*>(model);

    if (boolModel.Valid()) {
      Control::SetModel(boolModel);
    }
    else {
      Control::SetModel(numberModel);
    }

    return (GetModel()!=NULL);
  }

  bool State::HandleMouseEvent(const OS::MouseEvent& event)
  {
    if (!active || !visible || GetModel()==NULL || !GetModel()->IsEnabled() ||
        (numberModel.Valid() && numberModel->IsNull()) ||
        (boolModel.Valid() && boolModel->IsNull())) {
      return false;
    }

    if (event.type==OS::MouseEvent::down && IsIn(event) &&
        event.button==OS::MouseEvent::button1) {
      if (boolModel.Valid()) {
        orgState=boolModel->Get();
        boolModel->Set(!orgState);
      }
      else {
        orgState=numberModel->GetUnsignedLong();
        numberModel->Set(index);
      }

      return true;
    }
    else if (event.type==OS::MouseEvent::move && event.IsGrabed()) {
      if (IsIn(event)) {
        if (boolModel.Valid()) {
          boolModel->Set(orgState==0);
        }
        else {
          numberModel->Set(index);
        }
      }
      else {
        if (boolModel.Valid()) {
          boolModel->Set(orgState!=0);
        }
        else {
          numberModel->Set(orgState);
        }
      }
      return true;
    }

    return false;
  }


  bool State::HandleKeyEvent(const OS::KeyEvent& event)
  {
    if (Control::HandleKeyEvent(event)) {
      return true;
    }

    if (!active || !visible || GetModel()==NULL || !GetModel()->IsEnabled() ||
        (numberModel.Valid() && numberModel->IsNull()) ||
        (boolModel.Valid() && boolModel->IsNull())) {
      return false;
    }

    if (event.type==OS::KeyEvent::down && event.key==OS::keySpace) {
      if (boolModel.Valid()) {
        boolModel->Toggle();
        return true;
      }
      else if (numberModel.Valid()) {
        numberModel->Set(index);
        return true;
      }
    }

    return false;
  }

  void State::PrepareForBackground(OS::DrawInfo* draw)
  {
    draw->selected=false;

    if (boolModel.Valid() &&
        !boolModel->IsNull() &&
        boolModel->IsEnabled()) {
      if (boolModel->Get()) {
        draw->selected=true;
      }
    }
    else if (numberModel.Valid() &&
             !numberModel->IsNull() &&
             numberModel->IsEnabled()) {
      if (numberModel->GetUnsignedLong()==index) {
        draw->selected=true;
      }
    }
    else {
      draw->disabled=true;
    }

    draw->focused=HasFocus();
    draw->activated=IsMouseActive();
  }

  void State::Draw(OS::DrawInfo* draw,
                   int x, int y, size_t w, size_t h)
  {
    Control::Draw(draw,x,y,w,h);

    if (!OIntersect(x,y,w,h)) {
      return;
    }

    /* --- */

    draw->selected=false;

    if (boolModel.Valid() &&
        !boolModel->IsNull() &&
        boolModel->IsEnabled()) {
      if (boolModel->Get()) {
        draw->selected=true;
      }
    }
    else if (numberModel.Valid() &&
             !numberModel->IsNull() &&
             numberModel->IsEnabled()) {
      if (numberModel->GetUnsignedLong()==index) {
        draw->selected=true;
      }
    }
    else {
      draw->disabled=true;
    }

    draw->focused=HasFocus();
    draw->activated=IsMouseActive();

    DrawContent(draw,x,y,w,h);

    draw->selected=false;
    draw->activated=false;
    draw->focused=false;
    draw->disabled=false;
  }

  void State::Resync(Base::Model* model, const Base::ResyncMsg& msg)
  {
    if (model==boolModel || model==numberModel) {
      Redraw();
    }

    Control::Resync(model,msg);
  }

  ImageState::ImageState(OS::Image* image, bool active)
  : State(active),
    image(image)
  {
    assert(image!=NULL);
    // no code
  }

  void ImageState::CalcSize()
  {
    assert(image.Valid());

    width=image->GetWidth();
    height=image->GetHeight();

    State::CalcSize();
  }

  void ImageState::DrawContent(OS::DrawInfo* draw,
                               int x, int y, size_t w, size_t h)
  {
    assert(image.Valid());

    image->Draw(draw,
                this->x,this->y);
  }

}
