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

#include <Lum/Multi.h>

#include <limits>

namespace Lum {

  Multi::Multi()
   : lastDrawn(std::numeric_limits<unsigned long>::max())
  {
    // no code
  }

  Multi::~Multi()
  {
    for (size_t x=0; x<list.size(); x++) {
      delete list[x];
    }
  }

  bool Multi::VisitChildren(Visitor &visitor, bool onlyVisible)
  {
    if (onlyVisible) {
      if (model.Valid() &&
          !model->IsNull() &&
          !visitor.Visit(list[model->GetUnsignedLong()])) {
        return false;
      }
    }
    else {
      for (size_t x=0; x<list.size(); x++) {
        if (!visitor.Visit(list[x])) {
          return false;
        }
      }
    }

    return true;
  }

  bool Multi::SetModel(Base::Model* model)
  {
    this->model=dynamic_cast<Model::Number*>(model);

    Control::SetModel(this->model);

    return this->model.Valid();
  }

  void Multi::CalcSize()
  {
    width=0;
    height=0;
    minWidth=0;
    minHeight=0;

    for (size_t x=0; x<list.size(); x++) {
      list[x]->SetParent(this);
      list[x]->CalcSize();

      width=std::max(width,list[x]->GetOWidth());
      height=std::max(height,list[x]->GetOHeight());
      minWidth=std::max(minWidth,list[x]->GetOMinWidth());
      minHeight=std::max(minHeight,list[x]->GetOMinHeight());
    }

    Control::CalcSize();
  }

  void Multi::Layout()
  {
    // TODO: Layout only required objects...
    for (size_t i=0; i<list.size(); i++) {
      list[i]->Resize(width,height);
      list[i]->Move(x+(width-list[i]->GetOWidth())/2,
                    y+(height-list[i]->GetOHeight())/2);
    }
  }

  void Multi::Draw(OS::DrawInfo* draw,
                   int x, int y, size_t w, size_t h)
  {
    if (lastDrawn<list.size() &&
        ((!model.Valid() || model->IsNull()) ||
         (model.Valid() && !model->IsNull() && lastDrawn!=model->GetUnsignedLong()))) {
      /*
        This is a little trick:
        We must hide the old object, but we do not want it to clean up the
        covered area, since it will be overwritten by the new object anyway.
        So we create a clipping region that completely covers the top be drawn
        area :-)
      */
      draw->PushClipBegin(x,y,width,height);
      draw->SubClipRegion(x,y,width,height);
      draw->PushClipEnd();
      list[lastDrawn]->Hide();
      draw->PopClip();

      lastDrawn=std::numeric_limits<size_t>::max();
    }

    Control::Draw(draw,x,y,w,h); /* We must call Draw of our superclass */

    if (!OIntersect(x,y,w,h)) {
      return;
    }

    if (!model.Valid() || model->IsNull()) {
      return;
    }

    lastDrawn=model->GetUnsignedLong();
  }

  void Multi::Hide()
  {
    if (visible) {
      if (lastDrawn<list.size()) {
        list[lastDrawn]->Hide();
      }
      /* hide the frame */
      Control::Hide();
    }
  }

  unsigned long Multi::GetSize() const
  {
    return list.size();
  }

  Object* Multi::GetEntry(unsigned long pos) const
  {
    return list[pos];
  }

  Multi* Multi::Append(Object* object)
  {
    if (inited) {
      object->CalcSize();
    }

    list.push_back(object);

    return this;
  }

  Multi* Multi::Remove(Object* object)
  {
    for (size_t x=0; x<list.size(); x++) {
      if (list[x]==object) {
        Remove(x);
        return this;
      }
    }

    return this;
  }

  Multi* Multi::Remove(unsigned long /*pos*/)
  {
    assert(false);

    return this;
  }

  void Multi::ShowPrevious()
  {
    if (model->GetUnsignedLong()>0) {
      model->Set(model->GetUnsignedLong()-1);
    }
    else {
      model->Set(list.size()-1);
    }
  }

  void Multi::ShowNext()
  {
    if (model->GetUnsignedLong()<list.size()-1) {
      model->Set(model->GetUnsignedLong()+1);
    }
    else {
      model->Set(0);
    }
  }

  void Multi::Resync(Base::Model* model, const Base::ResyncMsg& msg)
  {
    if (model==this->model && visible) {
      Redraw();
    }

    Control::Resync(model,msg);
  }

  Multi* Multi::Create(Base::Model* index,
                       bool horizontalFlex, bool verticalFlex)
  {
    Multi *multi;

    multi=new Multi();
    multi->SetFlex(horizontalFlex,verticalFlex);
    multi->SetModel(index);

    return multi;
  }
}
