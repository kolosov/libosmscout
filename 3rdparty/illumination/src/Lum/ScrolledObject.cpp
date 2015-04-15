/*
  This source is part of the Illumination library
  Copyright (C) 2010  Tim Teulings

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

#include <Lum/ScrolledObject.h>

namespace Lum {

  ScrolledObject::ScrolledObject()
  : object(NULL),
    horizontalScroll(false),
    verticalScroll(false)
  {
    // no code
  }

  ScrolledObject::~ScrolledObject()
  {
    delete object;
  }

  bool ScrolledObject::RequiresKnobs() const
  {
    return false;
  }

  ScrolledObject* ScrolledObject::SetObject(Object *object)
  {
    assert(this->object==NULL);
    assert(!inited);

    this->object=object;
    this->object->SetParent(this);

    return this;
  }

  ScrolledObject* ScrolledObject::SetScroll(bool horizontalScroll,
                                            bool verticalScroll)
  {
    this->horizontalScroll=horizontalScroll;
    this->verticalScroll=verticalScroll;

    return this;
  }

  bool ScrolledObject::VisitChildren(Visitor &visitor, bool onlyVisible)
  {
    assert(object!=NULL);

    return visitor.Visit(object);
  }

  void ScrolledObject::CalcSize()
  {
    assert(object!=NULL);

    object->SetFlex(HorizontalFlex(),VerticalFlex());

    object->CalcSize();

    if (horizontalScroll) {
      minWidth=0;
    }
    else {
      minWidth=object->GetOWidth();
    }

    if (verticalScroll) {
      minHeight=0;
    }
    else {
      minHeight=object->GetOHeight();
    }

    width=object->GetOWidth();
    height=object->GetOHeight();
    maxWidth=object->GetOMaxWidth();
    maxHeight=object->GetOMaxHeight();

    Scrollable::CalcSize();
  }

  void ScrolledObject::Layout()
  {
    assert(object!=NULL);

    object->MoveResize(this->x-hAdjustment->GetTop()+1,
                       this->y-vAdjustment->GetTop()+1,
                       width,
                       height);

    hAdjustment->SetDimension(width,std::max(width,object->GetOWidth()));
    vAdjustment->SetDimension(height,std::max(height,object->GetOHeight()));

    Scrollable::Layout();
  }

  void ScrolledObject::GetDimension(size_t& width, size_t& height)
  {
    assert(object!=NULL);

    object->Resize(width,height);

    width=object->GetOWidth();
    height=object->GetOHeight();
  }

  void ScrolledObject::Resync(Lum::Base::Model* model, const Lum::Base::ResyncMsg& msg)
  {
    if (model==hAdjustment->GetTopModel() ||
        model==vAdjustment->GetTopModel()) {
      SetRelayout();
      Redraw();
    }

    Scrollable::Resync(model,msg);
  }

  ScrolledObject* ScrolledObject::Create(Object *object,
                                         bool horizontalFlex,
                                         bool verticalFlex)
  {
    ScrolledObject *s=new ScrolledObject();

    s->SetFlex(horizontalFlex,verticalFlex);
    s->SetObject(object);

    return s;
  }
}
