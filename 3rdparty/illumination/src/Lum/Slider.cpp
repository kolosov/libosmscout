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

#include <Lum/Slider.h>

#include <Lum/OS/Theme.h>

#include <Lum/Base/Util.h>

namespace Lum {

  Slider::Slider()
  : boxFrame(NULL),
    knob(NULL),
    offset(0),
    corr(0),
    model(NULL),
    selected(false),
    useScale(false)
  {
    if (!OS::display->GetTheme()->FocusOnEditableControlsOnly()) {
      SetCanFocus(true);
      RequestFocus();
    }
    SetRedrawOnMouseActive(true);
  }

  void Slider::SetScale(bool useScale)
  {
    this->useScale=useScale;
  }

  bool Slider::SetModel(Base::Model* model)
  {
    this->model=dynamic_cast<Model::Number*>(model);

    Control::SetModel(this->model);

    return this->model.Valid();
  }

  void Slider::OnMoveStart()
  {
    // TODO
  }

  void Slider::OnMoveEnd()
  {
    // TODO
  }
  void Slider::Dec()
  {
    model->Dec();
  }
  void Slider::Inc()
  {
    model->Inc();
  }

  void Slider::Resync(Base::Model* model, const Base::ResyncMsg& msg)
  {
    if (model==this->model && visible) {
      Redraw();
    }
    else {
      Control::Resync(model,msg);
    }
  }

  HSlider::HSlider()
  {
    knob=OS::display->GetImage(OS::Display::hscaleKnobImageIndex);
    boxFrame=OS::display->GetFrame(OS::Display::hscaleFrameIndex);
  }

  bool HSlider::HasBaseline() const
  {
    return useScale;
  }

  size_t HSlider::GetBaseline() const
  {
    OS::FontRef font=OS::display->GetFont();

    assert(useScale);

    return scale->GetOHeight()+
    OS::display->GetSpaceVertical(OS::Display::spaceIntraObject)+
    boxFrame->topBorder+((int)knob->GetHeight()-(int)font->height)/2+font->ascent;
  }

  void HSlider::DrawKnob(OS::DrawInfo* draw)
  {
    int boxSize,kStart,offset;

    draw->activated=IsMouseActive();
    draw->focused=HasFocus();
    draw->selected=selected;

    if (useScale) {
      offset=scale->GetOHeight()+OS::display->GetSpaceVertical(OS::Display::spaceIntraObject);
    }
    else {
      offset=0;
    }

    offset+=boxFrame->topBorder;

    boxSize=width-boxFrame->minWidth-2*(knob->GetWidth()/2);

    if (model->GetMinAsUnsignedLong()==model->GetMaxAsUnsignedLong()) {
      kStart=0;
    }
    else {
      kStart=Base::RoundDiv((model->GetUnsignedLong()-model->GetMinAsUnsignedLong())*boxSize,
                            model->GetMaxAsUnsignedLong()-model->GetMinAsUnsignedLong());
    }

    knobX=x+boxFrame->leftBorder+kStart;
    knobY=y+offset;
    knobWidth=knob->GetWidth();
    knobHeight=knob->GetHeight();

    draw->PushClip(x+boxFrame->leftBorder,
                   y+offset,
                   kStart,
                   knobHeight);

    OS::ImageRef filled=OS::display->GetImage(OS::Display::leftSliderImageIndex);
    OS::ImageRef unfilled=OS::display->GetImage(OS::Display::rightSliderImageIndex);

    filled->DrawStretched(draw,
                          x+boxFrame->leftBorder,
                          y+offset,
                          width-boxFrame->minWidth,
                          knobHeight);

    draw->PopClip();

    draw->PushClip(x+boxFrame->leftBorder+kStart+knobWidth,
                   y+offset,
                   width-boxFrame->minWidth-kStart-knobWidth,
                   knobHeight);

    unfilled->DrawStretched(draw,
                            x+boxFrame->leftBorder,
                            y+offset,
                            width-boxFrame->minWidth,
                            knobHeight);

    draw->PopClip();

    knob->Draw(draw,knobX,knobY);

    draw->activated=false;
    draw->selected=false;
    draw->focused=false;
  }

  void HSlider::CalcSize()
  {
    height=knob->GetHeight();
    width=5*knob->GetWidth();
    offset=knob->GetWidth() / 2;

    if (useScale) {
      scale=new HScale();
      scale->SetParent(this);
      scale->SetInterval(model->GetMinAsUnsignedLong(),model->GetMaxAsUnsignedLong());
      scale->SetFlex(true,true);
      scale->CalcSize();

      height+=scale->GetOHeight()+OS::display->GetSpaceVertical(OS::Display::spaceIntraObject);
      width=std::max(width,scale->GetOWidth());
    }

    width+=boxFrame->minWidth;
    height+=boxFrame->minHeight;
    offset+=boxFrame->leftBorder;

    minWidth=width;
    minHeight=height;

    Control::CalcSize();
  }

  void HSlider::Layout()
  {
    if (useScale) {
      scale->MoveResize(x+offset,y,width-knob->GetWidth(),scale->GetOHeight());
    }

    Control::Layout();
  }

  void HSlider::Draw(OS::DrawInfo* draw,
                     int x, int y, size_t w, size_t h)
  {
    size_t fOffset,fSize;

    draw->focused=HasFocus();

    Control::Draw(draw,x,y,w,h);

    if (!OIntersect(x,y,w,h)) {
      draw->focused=false;
      return;
    }

    /* --- */

    if (useScale) {
      fOffset=scale->GetOHeight()+OS::display->GetSpaceVertical(OS::Display::spaceIntraObject);
    }
    else {
      fOffset=0;
    }
    fSize=knob->GetHeight()+boxFrame->minHeight;

    if (useScale) {
      scale->Draw(draw,x,y,w,h);
    }

    draw->focused=false;

    boxFrame->Draw(draw,this->x,this->y+fOffset,this->width,fSize);

    if (model.Valid() && !model->IsNull()) {
      DrawKnob(draw);
    }
  }

  void HSlider::HandleMouseMovement(int x, int /*y*/)
  {
    int newValue;

    if (x<this->x+(int)boxFrame->leftBorder+corr) {
      x=this->x+boxFrame->leftBorder+corr;
    }

    newValue=model->GetMinAsUnsignedLong()+
        ((x-(this->x+boxFrame->leftBorder)-corr)*
            (model->GetMaxAsUnsignedLong()-model->GetMinAsUnsignedLong()+1))/
        (width-boxFrame->minWidth-knob->GetWidth());

    if (newValue>=model->GetMinAsUnsignedLong() && newValue<=model->GetMaxAsUnsignedLong()) {
      model->Set(newValue);
    }
    else if (newValue<model->GetMinAsUnsignedLong()) {
      model->SetToMin();
    }
    else if (newValue>model->GetMaxAsUnsignedLong()) {
      model->SetToMax();
    }
  }

  bool HSlider::HandleMouseEvent(const OS::MouseEvent& event)
  {
    if (!visible || !model.Valid() || !model->IsEnabled() || model->IsNull()) {
      return false;
    }

    if (event.type==OS::MouseEvent::down && PointIsIn(event)) {
      if (event.button==OS::MouseEvent::button1) {
        if (event.x>=knobX && event.y>=knobY &&
            event.x<(int)(knobX+knobWidth) &&
            event.y<(int)(knobY+knobHeight)) {
          corr=event.x-knobX;
          selected=true;
          Redraw(); // TODO: ONly knob

          OnMoveStart();
        }
        else if (event.x<knobX) {
          Dec();
        }
        else if (event.x>(int)(knobX+knobWidth)) {
          Inc();
        }
      }
      else if (event.button==OS::MouseEvent::button4) {
        Dec();
      }
      else if (event.button==OS::MouseEvent::button3) {
        Inc();
      }

      return true;
    }
    else if (event.IsGrabEnd()) {
      if (selected) {
        HandleMouseMovement(event.x,event.y);
        selected=false;
      }

      Redraw(); // TODO: Only knob

      OnMoveEnd();

      return true;
    }
    else if (event.type==OS::MouseEvent::move && event.IsGrabed() && selected)  {
      HandleMouseMovement(event.x,event.y);
      return true;
    }

    return false;
  }

  bool HSlider::HandleKeyEvent(const OS::KeyEvent& event)
  {
    unsigned long old,tmp;

    if (event.type==OS::KeyEvent::down && model.Valid()) {
      old=model->GetUnsignedLong();
      if (event.key==OS::keyLeft) {
        Dec();
      }
      else if (event.key==OS::keyRight) {
        Inc();
      }
      else if (event.key==OS::keyHome) {
        model->SetToMin();
      }
      else if (event.key==OS::keyEnd) {
        model->SetToMax();
      }
      else if (event.key==OS::keyNext) {
        tmp=old;
        tmp-=(model->GetMaxAsUnsignedLong()-model->GetMinAsUnsignedLong()+1) / 10;
        if (tmp<model->GetMinAsUnsignedLong()) {
          tmp=model->GetMinAsUnsignedLong();
        }
        model->Set(tmp);
      }
      else if (event.key==OS::keyPrior) {
        tmp=old;
        tmp+=(model->GetMaxAsUnsignedLong()-model->GetMinAsUnsignedLong()+1) / 10;
        if (tmp>model->GetMaxAsUnsignedLong()) {
          tmp=model->GetMaxAsUnsignedLong();
        }
        model->Set(tmp);
      }
      else {
        return false;
      }

      return true;
    }

    return false;
  }

  Slider* HSlider::Create(bool horizontalFlex, bool verticalFlex)
  {
    Slider *s;

    s=new HSlider();
    s->SetFlex(horizontalFlex,verticalFlex);

    return s;
  }

  Slider* HSlider::Create(Base::Model* model,
                          bool horizontalFlex, bool verticalFlex)
  {
    Slider *s;

    s=new HSlider();
    s->SetFlex(horizontalFlex,verticalFlex);
    s->SetModel(model);

    return s;
  }

  Slider* HSlider::Create(Base::Model* model,
                          bool useScale,
                          bool horizontalFlex, bool verticalFlex)
  {
    Slider *s;

    s=new HSlider();
    s->SetFlex(horizontalFlex,verticalFlex);
    s->SetScale(useScale);
    s->SetModel(model);

    return s;
  }

  VSlider::VSlider()
  {
    knob=OS::display->GetImage(OS::Display::vscaleKnobImageIndex);
    boxFrame=OS::display->GetFrame(OS::Display::hscaleFrameIndex);
  }

  void VSlider::DrawKnob(OS::DrawInfo* draw)
  {
    int boxSize,kStart,offset;

    draw->activated=IsMouseActive();
    draw->focused=HasFocus();
    draw->selected=selected;

    if (useScale) {
      offset=scale->GetOWidth()+OS::display->GetSpaceHorizontal(OS::Display::spaceIntraObject);
    }
    else {
      offset=0;
    }

    offset+=boxFrame->leftBorder;

    boxSize=height-boxFrame->minHeight-knob->GetHeight();

    if (model->GetMaxAsUnsignedLong()==model->GetMinAsUnsignedLong()) {
      kStart=0;
    }
    else {
      kStart=boxSize-Base::RoundDiv((model->GetUnsignedLong()-model->GetMinAsUnsignedLong())*boxSize,
                                    model->GetMaxAsUnsignedLong()-model->GetMinAsUnsignedLong());
    }

    knobX=x+offset;
    knobY=y+boxFrame->topBorder+kStart;
    knobWidth=knob->GetWidth();
    knobHeight=knob->GetHeight();

    draw->PushClip(x+offset,
                   y+boxFrame->topBorder,
                   knob->GetWidth(),
                   kStart);

    OS::ImageRef filled=OS::display->GetImage(OS::Display::bottomSliderImageIndex);
    OS::ImageRef unfilled=OS::display->GetImage(OS::Display::topSliderImageIndex);

    unfilled->DrawStretched(draw,
                            x+offset,
                            y+boxFrame->topBorder,
                            knobWidth,
                            height-boxFrame->minHeight);

    draw->PopClip();

    draw->PushClip(x+offset,
                   y+boxFrame->topBorder+kStart+knobHeight,
                   knobWidth,
                   height-boxFrame->minHeight-kStart-knobHeight);

    filled->DrawStretched(draw,
                          x+offset,
                          y+boxFrame->topBorder,
                          knobWidth,
                          height-boxFrame->minHeight);

    draw->PopClip();

    knob->Draw(draw,knobX,knobY);

    draw->activated=false;
    draw->focused=false;
    draw->selected=false;
  }

  void VSlider::CalcSize()
  {
    width=knob->GetWidth();
    height=3*knob->GetHeight();
    offset=knob->GetHeight() / 2;

    if (useScale) {
      scale=new VScale();
      scale->SetParent(this);
      scale->SetInterval(model->GetMinAsUnsignedLong(),model->GetMaxAsUnsignedLong());
      scale->SetFlex(true,true);
      scale->CalcSize();

      width+=scale->GetOWidth()+OS::display->GetSpaceHorizontal(OS::Display::spaceIntraObject);
      height=std::max(height,scale->GetOHeight());
    }

    width+=boxFrame->minWidth;
    height+=boxFrame->minHeight;
    offset+=boxFrame->topBorder;

    minWidth=width;
    minHeight=height;

    Control::CalcSize();
  }

  void VSlider::Layout()
  {
    if (useScale) {
      scale->MoveResize(x,y+offset,scale->GetOWidth(),height-knob->GetHeight());
    }
    Control::Layout();
  }

  void VSlider::Draw(OS::DrawInfo* draw,
                     int x, int y, size_t w, size_t h)
  {
    size_t fOffset,fSize;

    draw->focused=HasFocus();

    Control::Draw(draw,x,y,w,h);

    if (!OIntersect(x,y,w,h)) {
      draw->focused=false;
      return;
    }

    /* --- */

    if (useScale) {
      fOffset=scale->GetOWidth()+OS::display->GetSpaceHorizontal(OS::Display::spaceIntraObject);
    }
    else {
      fOffset=0;
    }
    fSize=knob->GetWidth()+boxFrame->minWidth;

    if (useScale) {
      scale->Draw(draw,x,y,w,h);
    }

    boxFrame->Draw(draw,this->x+fOffset,this->y,fSize,this->height);

    draw->focused=false;

    if (model.Valid() && !model->IsNull()) {
      DrawKnob(draw);
    }
  }

  void VSlider::HandleMouseMovement(int /*x*/, int y)
  {
    int newValue;

    if (y<this->y+(int)boxFrame->topBorder+corr) {
      y=this->y+boxFrame->topBorder+corr;
    }

    newValue=model->GetMaxAsUnsignedLong()-((y-(this->y+boxFrame->topBorder)-corr)*(model->GetMaxAsUnsignedLong()-model->GetMinAsUnsignedLong()+1))/(height-boxFrame->minHeight-knob->GetHeight());

    if (newValue>=model->GetMinAsUnsignedLong() && newValue<=model->GetMaxAsUnsignedLong()) {
      model->Set(newValue);
    }
    else if (newValue<model->GetMinAsUnsignedLong()) {
      model->SetToMin();
    }
    else if (newValue>model->GetMaxAsUnsignedLong()) {
      model->SetToMax();
    }
  }

  bool VSlider::HandleMouseEvent(const OS::MouseEvent& event)
  {
    if (!visible || !model.Valid() || !model->IsEnabled() || model->IsNull()) {
      return false;
    }

    if (event.type==OS::MouseEvent::down && PointIsIn(event)) {
      if (event.button==OS::MouseEvent::button1) {
        if (event.x>=knobX && event.y>=knobY &&
            event.x<(int)(knobX+knobWidth) &&
            event.y<(int)(knobY+knobHeight)) {
          corr=event.y-knobY;
          selected=true;
          Redraw(); // TODO: Only knob

          OnMoveStart();
        }
        else if (event.y<knobY) {
          Dec();
        }
        else if (event.y>(int)(knobY+knobHeight)) {
          Inc();
        }
      }
      else if (event.button==OS::MouseEvent::button4) {
        Dec();
      }
      else if (event.button==OS::MouseEvent::button3) {
        Inc();
      }

      return true;
    }
    else if (event.IsGrabEnd()) {
      if (selected) {
        HandleMouseMovement(event.x,event.y);
        selected=false;
      }

      Redraw(); // TODO: Only knob

      OnMoveEnd();

      return true;
    }
    else if (event.type==OS::MouseEvent::move && event.IsGrabed() && selected)  {
      HandleMouseMovement(event.x,event.y);
      return true;
    }

    return false;
  }

  bool VSlider::HandleKeyEvent(const OS::KeyEvent& event)
  {
    unsigned long old,tmp;

    if (event.type==OS::KeyEvent::down && model.Valid()) {
      old=model->GetUnsignedLong();
      if (event.key==OS::keyDown) {
        Dec();
      }
      else if (event.key==OS::keyUp) {
        Inc();
      }
      else if (event.key==OS::keyHome) {
        model->SetToMin();
      }
      else if (event.key==OS::keyEnd) {
        model->SetToMax();
      }
      else if (event.key==OS::keyNext) {
        tmp=old;
        tmp-=(model->GetMaxAsUnsignedLong()-model->GetMinAsUnsignedLong()+1) / 10;
        if (tmp<model->GetMinAsUnsignedLong()) {
          tmp=model->GetMinAsUnsignedLong();
        }
        model->Set(tmp);
      }
      else if (event.key==OS::keyPrior) {
        tmp=old;
        tmp+=(model->GetMaxAsUnsignedLong()-model->GetMinAsUnsignedLong()+1) / 10;
        if (tmp>model->GetMaxAsUnsignedLong()) {
          tmp=model->GetMaxAsUnsignedLong();
        }
        model->Set(tmp);
      }
      else {
        return false;
      }

      return true;
    }

    return false;
  }

  Slider* VSlider::Create(bool horizontalFlex, bool verticalFlex)
  {
    Slider *s;

    s=new VSlider();
    s->SetFlex(horizontalFlex,verticalFlex);

    return s;
  }

  Slider* VSlider::Create(Base::Model* model,
                          bool horizontalFlex, bool verticalFlex)
  {
    Slider *s;

    s=new VSlider();
    s->SetFlex(horizontalFlex,verticalFlex);
    s->SetModel(model);

    return s;
  }

  Slider* VSlider::Create(Base::Model* model,
                          bool useScale,
                          bool horizontalFlex, bool verticalFlex)
  {
    Slider *s;

    s=new VSlider();
    s->SetFlex(horizontalFlex,verticalFlex);
    s->SetScale(useScale);
    s->SetModel(model);

    return s;
  }

}
