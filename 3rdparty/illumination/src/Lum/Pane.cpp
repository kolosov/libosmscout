/*
  This source is part of the Illumination library
  Copyright (C) 2008  Tim Teulings

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

#include <Lum/Pane.h>

#include <Lum/Image.h>

#include <Lum/Base/Util.h>

namespace Lum {

  Pane::Pane(Direction direction)
   : direction(direction)
  {
    // no code
  }

  void Pane::Add(Object* object)
  {
    if (list.size()>0) {
      Image *image;

      image=new Image();
      image->SetFlex(true,true);
      if (direction==horizontal) {
        image->SetImage(OS::display->GetImage(OS::Display::vpaneHandleImageIndex));
      }
      else {
        image->SetImage(OS::display->GetImage(OS::Display::hpaneHandleImageIndex));
      }
      image->SetMinWidth(Base::Size::pixel,3);
      image->SetMinHeight(Base::Size::pixel,3);

      List::Add(image);
    }

    List::Add(object);
  }

  void Pane::CalcSize()
  {
    width=0;
    height=0;
    minWidth=0;
    minHeight=0;

    for (std::list<Object*>::iterator iter=list.begin(); iter!=list.end(); ++iter) {
      (*iter)->CalcSize();

      if (direction==horizontal) {
        width+=(*iter)->GetOWidth();
        height=std::max(height,(*iter)->GetOHeight());

        minWidth+=(*iter)->GetOMinWidth();
        minHeight=std::max(minHeight,(*iter)->GetOMinHeight());
      }
      else {
        width=std::max(width,(*iter)->GetOWidth());
        height+=(*iter)->GetOHeight();

        minWidth=std::max(minWidth,(*iter)->GetOMinWidth());
        minHeight+=(*iter)->GetOMinHeight();
      }
    }

    List::CalcSize();
  }

  void Pane::Layout()
  {
    int    pos;
    size_t size;

    if (direction==horizontal) {
      pos=x;
    }
    else {
      pos=y;
    }

    size=0;
    for (std::list<Object*>::iterator iter=list.begin(); iter!=list.end(); ++iter) {

      if (direction==horizontal) {
        if ((*iter)!=list.back()) {
          (*iter)->MoveResize(pos,y+(height-(*iter)->GetOHeight())/2,(*iter)->GetOWidth(),height);
        }
        else {
          (*iter)->MoveResize(pos,y+(height-(*iter)->GetOHeight())/2,width-size,height);
        }
        size+=(*iter)->GetOWidth();
        pos+=(*iter)->GetOWidth();
      }
      else {
        if ((*iter)!=list.back()) {
          (*iter)->MoveResize(x+(width-(*iter)->GetOWidth())/2,pos,width,(*iter)->GetOHeight());
        }
        else {
          (*iter)->MoveResize(x+(width-(*iter)->GetOWidth())/2,pos,width,height-size);
        }
        size+=(*iter)->GetOHeight();
        pos+=(*iter)->GetOHeight();
      }
    }
  }

  void Pane::HandleMouseMoveEvent(const OS::MouseEvent& event)
  {
    if (!PointIsIn(event)) {
      return;
    }

    std::list<Object*>::iterator first=list.begin(),second;
    size_t                       current=pos-1;
    int                          delta;

    // Handle object before mover
    while (current>0) {
      ++first;
      --current;
    }

    // Handle object before mover
    second=first;
    // Goto mover
    ++second;
    // Skip mover
    ++second;

    if (direction==horizontal) {
      delta=event.x-offset-(*first)->GetOX()-(*first)->GetOWidth()+1;

      if ((*first)->GetOWidth()+delta>=(*first)->GetOMinWidth() &&
          (*first)->GetOWidth()+delta<=(*first)->GetOMaxWidth() &&
          (*second)->GetOWidth()-delta>=(*second)->GetOMinWidth() &&
          (*second)->GetOWidth()-delta<=(*second)->GetOMaxWidth()) {
        (*first)->ResizeWidth((*first)->GetOWidth()+delta);
        (*second)->ResizeWidth((*first)->GetOWidth()-delta);
        (*second)->Move((*second)->GetOX()-delta,(*second)->GetOY());
        Redraw();
      }
    }
    else {
      delta=event.y-offset-(*first)->GetOY()-(*first)->GetOHeight()+1;

      if ((*first)->GetOHeight()+delta>=(*first)->GetOMinHeight() &&
          (*first)->GetOHeight()+delta<=(*first)->GetOMaxHeight() &&
          (*second)->GetOHeight()-delta>=(*second)->GetOMinHeight() &&
          (*second)->GetOHeight()-delta<=(*second)->GetOMaxHeight()) {
        (*first)->ResizeHeight((*first)->GetOHeight()+delta);
        (*second)->ResizeHeight((*first)->GetOHeight()-delta);
        (*second)->Move((*second)->GetOX(),(*second)->GetOY()-delta);
        Redraw();
      }
    }
  }


  bool Pane::HandleMouseEvent(const OS::MouseEvent& event)
  {
    if (!visible) {
      return false;
    }

    if (event.type==OS::MouseEvent::down && PointIsIn(event) && event.button==OS::MouseEvent::button1) {

      pos=0;
      for (std::list<Object*>::iterator iter=list.begin(); iter!=list.end(); ++iter) {
        if ((*iter)->PointIsIn(event) && (pos % 2==1)) {
          if (direction==horizontal) {
            offset=event.x-(*iter)->GetOX();
          }
          else {
            offset=event.y-(*iter)->GetOY();
          }
          return true;
        }

        pos++;
      }
    }
    else if (event.IsGrabEnd()) {
      HandleMouseMoveEvent(event);

      return false;
    }
    else if (event.type==OS::MouseEvent::move && event.IsGrabed()) {
      if (!PointIsIn(event)) {
        return true;
      }

      HandleMouseMoveEvent(event);

      return true;
    }

    return false;
  }

}

