/*
  This source is part of the Illumination library
  Copyright (C) 2007  Tim Teulings

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

#include <Lum/AspectRatio.h>

#include <Lum/Scroller.h>

namespace Lum {

  AspectRatio::AspectRatio()
  : object(NULL),
    hAspect(1),vAspect(1),
    halign(halignCenter),valign(valignCenter)
  {
    // no code
  }

  AspectRatio::~AspectRatio()
  {
    delete object;
  }

  AspectRatio* AspectRatio::SetAspectRatio(size_t hAspect, size_t vAspect)
  {
    this->hAspect=hAspect;
    this->vAspect=vAspect;

    if (object!=NULL && visible) {
      Redraw();
    }

    return this;
  }

  AspectRatio* AspectRatio::SetAlignment(HAlignment halign, VAlignment valign)
  {
    bool redraw;

    redraw=visible && object!=NULL && (this->halign!=halign || this->valign!=valign);

    this->halign=halign;
    this->valign=valign;

    if (redraw) {
      Redraw();
    }

    return this;
  }

  AspectRatio* AspectRatio::SetObject(Object* object)
  {
    assert(object!=NULL);

    delete this->object;

    this->object=object;
    this->object->SetParent(this);
    this->object->SetFlex(true,true);

    return this;
  }

  Object* AspectRatio::GetObject() const
  {
    return object;
  }

  bool AspectRatio::VisitChildren(Visitor &visitor, bool onlyVisible)
  {
    if (object!=NULL) {
      if (!visitor.Visit(object)) {
        return false;
      }
    }

    return true;
  }

  void AspectRatio::CalcSize()
  {
    assert(object!=NULL);

    object->CalcSize();

    if (vAspect>=hAspect) {
      minWidth=object->GetOMinWidth();
      width=object->GetOWidth();
      minHeight=std::max(object->GetOMinHeight(),(minWidth*vAspect)/hAspect);
      height=std::max(object->GetOHeight(),(width*vAspect)/hAspect);
    }
    else {
      minHeight=object->GetOMinHeight();
      height=object->GetOHeight();
      minWidth=std::max(object->GetOMinWidth(),(minHeight*hAspect)/vAspect);
      width=std::max(object->GetOHeight(),(height*hAspect)/vAspect);
    }

    Group::CalcSize();
  }

  void AspectRatio::Layout()
  {
    int    x=0,y=0;
    size_t w,h;

    // First calculate height based on width
    w=width;
    h=(width*vAspect)/hAspect;

    // If this does not fit, calculate width based on height
    if (h>height) {
      w=(height*hAspect)/vAspect;
      h=height;
    }

    switch (halign) {
    case halignLeft:
      x=this->x;
      break;
    case halignCenter:
      x=this->x+(width-w)/2;
      break;
    case halignRight:
      x=this->x+(width-w);
      break;
    }

    switch (valign) {
    case valignTop:
      y=this->y;
      break;
    case valignCenter:
      y=this->y+(height-h)/2;
      break;
    case valignBottom:
      y=this->y+(height-h);
      break;
    }

    object->MoveResize(x,y,w,h);

    Group::Layout();
  }

  AspectRatio* AspectRatio::Create(Object* object,
                                   size_t hAspect, size_t vAspect,
                                   bool horizontalFlex, bool verticalFlex)
  {
    AspectRatio* o;

    o=new AspectRatio();
    o->SetFlex(horizontalFlex,verticalFlex);
    o->SetAspectRatio(hAspect,vAspect);
    o->SetObject(object);

    return o;
  }

  AspectRatio* AspectRatio::Create(Object* object,
                                   size_t hAspect, size_t vAspect,
                                   HAlignment halign, VAlignment valign,
                                   bool horizontalFlex, bool verticalFlex)
  {
    AspectRatio* o;

    o=new AspectRatio();
    o->SetFlex(horizontalFlex,verticalFlex);
    o->SetAspectRatio(hAspect,vAspect);
    o->SetAlignment(halign,valign);
    o->SetObject(object);

    return o;
  }
}

