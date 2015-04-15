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

#include <Lum/Panel.h>

#include <Lum/Base/Util.h>

#include <Lum/Space.h>

namespace Lum {

  Panel::Panel()
  {
    // no code
  }

  Panel* Panel::Add(Lum::Object* object)
  {
    List::Add(object);

    return this;
  }

  HPanel::HPanel()
  {
    // no code
  }

  HPanel* HPanel::AddSpace(bool flex)
  {
    Add(new HSpace(flex));

    return this;
  }

  void HPanel::CalcSize()
  {
    std::list<Object*>::iterator iter;

    width=0;
    height=0;
    minWidth=0;
    minHeight=0;

    iter=list.begin();
    while (iter!=list.end()) {
      (*iter)->CalcSize();

      width+=(*iter)->GetOWidth();
      height=std::max(height,(*iter)->GetOHeight());

      minWidth+=(*iter)->GetOMinWidth();
      minHeight=std::max(minHeight,(*iter)->GetOMinHeight());

      ++iter;
    }

    Panel::CalcSize();
  }

  VPanel* VPanel::AddSpace(bool flex)
  {
    Add(new VSpace(flex));

    return this;
  }

  void VPanel::CalcSize()
  {
    std::list<Object*>::iterator iter;

    width=0;
    height=0;
    minWidth=0;
    minHeight=0;

    iter=list.begin();
    while (iter!=list.end()) {
      (*iter)->CalcSize();

      width=std::max(width,(*iter)->GetOWidth());
      height+=(*iter)->GetOHeight();

      minWidth=std::max(minWidth,(*iter)->GetOMinWidth());
      minHeight+=(*iter)->GetOMinHeight();

      ++iter;
    }

    Panel::CalcSize();
  }

  void HPanel::Layout()
  {
    std::list<Object*>::iterator iter;
    size_t                       size;
    int                          pos;

    size=0;
    iter=list.begin();
    while (iter!=list.end()) {
      (*iter)->Resize((*iter)->GetOMinWidth(),height);
      size+=(*iter)->GetOMinWidth();

      ++iter;
    }

    while (true) {
      int count=0;
      // count number of resizable objects
      iter=list.begin();
      while (iter!=list.end()) {
        if ((*iter)->CanResize(width>=size,true)) {
          count++;
        }

        ++iter;
      }

      // we leave the loop if there are no objects (anymore) that can be resized
      if (count==0) {
        break;
      }

      iter=list.begin();
      while (iter!=list.end()) {
        if ((*iter)->CanResize(width>=size,true)) {
          int old;

          old=(*iter)->GetOWidth();
          (*iter)->ResizeWidth((*iter)->GetOWidth()+Base::UpDiv(width-size,count));
          size+=(*iter)->GetOWidth()-old;
          count--;
        }

        ++iter;
      }

      // no space left to delegate
      if (size==width) {
        break;
      }
    }

    pos=x;
    iter=list.begin();
    while (iter!=list.end()) {
      (*iter)->Move(pos,y+(height-(*iter)->GetOHeight()) / 2);
      pos+=(*iter)->GetOWidth();
      ++iter;
    }

    Panel::Layout();
  }

  VPanel::VPanel()
  {
    // no code
  }

  void VPanel::Layout()
  {
    std::list<Object*>::iterator iter;
    size_t                       size;
    int                          pos;

    size=0;
    iter=list.begin();
    while (iter!=list.end()) {
      (*iter)->Resize(width,(*iter)->GetOMinHeight());
      size+=(*iter)->GetOMinHeight();

      ++iter;
    }

    while (true) {
      int count=0;
      // count number of resizable objects
      iter=list.begin();
      while (iter!=list.end()) {
        if ((*iter)->CanResize(height>=size,false)) {
          count++;
        }

        ++iter;
      }

      // we leave the loop if there are no objects (anymore) that can be resized
      if (count==0) {
        break;
      }

      iter=list.begin();
      while (iter!=list.end()) {
        if ((*iter)->CanResize(height>=size,false)) {
          int old;


          old=(*iter)->GetOHeight();
          (*iter)->ResizeHeight((*iter)->GetOHeight()+Base::UpDiv(height-size,count));
          size+=(*iter)->GetOHeight()-old;
          count--;
        }

        ++iter;
      }

      // no space left to delegate
      if (size==height) {
        break;
      }
    }

    pos=y;
    iter=list.begin();
    while (iter!=list.end()) {
      (*iter)->Move(x+(width-(*iter)->GetOWidth()) / 2,pos);
      pos+=(*iter)->GetOHeight();
      ++iter;
    }

    Panel::Layout();
  }

  HPanel* HPanel::Create(bool horizontalFlex, bool verticalFlex)
  {
    HPanel *p;

    p=new HPanel();
    p->SetFlex(horizontalFlex,verticalFlex);

    return p;
  }

  VPanel* VPanel::Create(bool horizontalFlex, bool verticalFlex)
  {
    VPanel *p;

    p=new VPanel();
    p->SetFlex(horizontalFlex,verticalFlex);

    return p;
  }
}
