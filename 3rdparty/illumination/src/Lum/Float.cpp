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

#include <Lum/Float.h>

#include <cassert>
#include <cmath>

namespace Lum {

  Float::Float()
  : preferredColumns(0)
  {
    // no code
  }

  Float::~Float()
  {
    for (std::list<Object*>::iterator iter=objects.begin();
         iter!=objects.end();
         ++iter) {
      delete *iter;
    }
  }

  bool Float::RequiresKnobs() const
  {
    return false;
  }

  Float* Float::SetPreferredColumns(size_t columns)
  {
    preferredColumns=columns;

    return this;
  }

  Float* Float::Add(Lum::Object* object)
  {
    assert(object!=NULL);

    object->SetParent(this);
    objects.push_back(object);

    return this;
  }

  bool Float::VisitChildren(Visitor &visitor, bool onlyVisible)
  {
    for (std::list<Object*>::iterator iter=objects.begin();
         iter!=objects.end();
         ++iter) {
      if (!visitor.Visit(*iter)) {
        return false;
      }
    }

    return true;
  }

  void Float::CalcSize()
  {
    objectMinWidth=0;
    objectMinHeight=0;
    objectWidth=0;
    objectHeight=0;

    for (std::list<Object*>::iterator iter=objects.begin(); iter!=objects.end(); ++iter) {
      Object *object=(*iter);

      object->CalcSize();

      objectMinWidth=std::max(objectMinWidth,object->GetOMinWidth());
      objectMinHeight=std::max(objectMinHeight,object->GetOMinHeight());

      objectWidth=std::max(objectWidth,object->GetOWidth());
      objectHeight=std::max(objectHeight,object->GetOHeight());
    }

    minWidth+=objectMinWidth+
              2*OS::display->GetSpaceHorizontal(Lum::OS::Display::spaceInterObject);
    minHeight+=objectMinHeight+
               2*OS::display->GetSpaceVertical(Lum::OS::Display::spaceInterObject);

    unsigned long columns;
    unsigned long rows;

    if (preferredColumns>0) {
      columns=std::max((size_t)1u,std::min(objects.size(),preferredColumns));
    }
    else {
      columns=std::max(1l,lround(sqrt(objects.size())));
    }

    if (columns>0) {
      rows=objects.size()/columns;

      if (objects.size()%columns>0) {
        rows++;
      }
    }
    else {
      rows=0;
    }


    width=(columns+1)*OS::display->GetSpaceHorizontal(Lum::OS::Display::spaceInterObject)+
          columns*objectWidth;

    height=(rows+1)*OS::display->GetSpaceVertical(Lum::OS::Display::spaceInterObject)+
           rows*objectHeight;

    Scrollable::CalcSize();
  }

  void Float::Layout()
  {
    unsigned column,row;
    size_t   allWidth;
    size_t   allHeight;
    int      xOffset;
    int      yOffset;
    size_t   ow;

    xOffset=0;
    yOffset=0;
    ow=objectWidth;

    allWidth=(columns+1)*OS::display->GetSpaceHorizontal(Lum::OS::Display::spaceInterObject)+
             columns*objectWidth;
    allHeight=(rows+1)*OS::display->GetSpaceVertical(Lum::OS::Display::spaceInterObject)+
              rows*objectHeight;

    // Is there space to spend?
    if (allWidth<width && columns>0) {
      ow=objectWidth+(width-allWidth)/columns;

      for (std::list<Object*>::iterator iter=objects.begin(); iter!=objects.end(); ++iter) {
        Object *object=(*iter);

        object->ResizeWidth(ow);
      }

      ow=0;
      for (std::list<Object*>::iterator iter=objects.begin(); iter!=objects.end(); ++iter) {
        Object *object=(*iter);

        ow=std::max(ow,object->GetOWidth());
      }

      allWidth=(columns+1)*OS::display->GetSpaceHorizontal(Lum::OS::Display::spaceInterObject)+
               columns*ow;

      if (allWidth<width) {
        xOffset=(width-allWidth)/2;
      }
    }

    if (allHeight<height) {
      yOffset=(height-allHeight)/2;
    }
    else {
      yOffset=0;
    }

    column=0;
    row=0;
    for (std::list<Object*>::iterator iter=objects.begin(); iter!=objects.end(); ++iter) {
      Object *object=(*iter);

      object->Resize(ow,objectHeight);
      object->Move(this->x-hAdjustment->GetTop()+1+
                   xOffset+
                   OS::display->GetSpaceHorizontal(Lum::OS::Display::spaceInterObject)+
                   column*ow+
                   column*OS::display->GetSpaceHorizontal(Lum::OS::Display::spaceInterObject),
                   this->y-vAdjustment->GetTop()+1+
                   yOffset+
                   OS::display->GetSpaceVertical(Lum::OS::Display::spaceInterObject)+
                   row*objectHeight+
                   row*OS::display->GetSpaceVertical(Lum::OS::Display::spaceInterObject));

      column++;

      if (column==columns) {
        column=0;
        row++;
      }
    }

    hAdjustment->SetDimension(width,std::max(allWidth,allWidth));
    vAdjustment->SetDimension(height,std::max(height,allHeight));

    Scrollable::Layout();
  }

  void Float::GetDimension(size_t& width, size_t& height)
  {
    if (preferredColumns>0) {
      columns=std::max((size_t)1u,std::min(objects.size(),preferredColumns));
    }
    else {
      columns=std::max(1l,lround(sqrt(objects.size())));
    }

    if (columns>0) {
      rows=objects.size()/columns;

      if (objects.size()%columns>0) {
        rows++;
      }
    }
    else {
      rows=0;
    }

    if ((columns+1)*OS::display->GetSpaceHorizontal(Lum::OS::Display::spaceInterObject)+
        columns*objectWidth>width ||
        (rows+1)*OS::display->GetSpaceVertical(Lum::OS::Display::spaceInterObject)+
        rows*objectHeight>height) {
      // A square arrangement is not possible, now we try as much columns
      // as fit
      size_t columnWidth=objectWidth+
                         2*OS::display->GetSpaceHorizontal(Lum::OS::Display::spaceInterObject);

      columns=1;
      while (columnWidth+
             OS::display->GetSpaceHorizontal(Lum::OS::Display::spaceInterObject)+
             objectWidth<width) {
        columns++;
        columnWidth+=OS::display->GetSpaceHorizontal(Lum::OS::Display::spaceInterObject)+
                     objectWidth;
      }

      rows=objects.size()/columns;

      if (objects.size()%columns>0) {
        rows++;
      }
    }

    width=std::max(width,
                   (columns+1)*OS::display->GetSpaceHorizontal(Lum::OS::Display::spaceInterObject)+
                   columns*objectWidth);
    height=std::max(height,
                    (rows+1)*OS::display->GetSpaceVertical(Lum::OS::Display::spaceInterObject)+
                    rows*objectHeight);
  }

  void Float::Resync(Lum::Base::Model* model, const Lum::Base::ResyncMsg& msg)
  {
    if (model==hAdjustment->GetTopModel() ||
        model==vAdjustment->GetTopModel()) {
      SetRelayout();
      Redraw();
    }

    Scrollable::Resync(model,msg);
  }
}
