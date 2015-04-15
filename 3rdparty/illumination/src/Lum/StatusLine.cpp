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

#include <Lum/StatusLine.h>

#include <Lum/Base/Util.h>

namespace Lum {

  StatusLine::StatusLine()
  {
    SetBackground(OS::display->GetFill(OS::Display::statusbarBackgroundFillIndex));
  }

  void StatusLine::CalcSize()
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
      height=std::max(minHeight,(*iter)->GetOMinHeight());

      ++iter;
    }

    OS::FrameRef iFrame=OS::display->GetFrame(OS::Display::statuscellFrameIndex);

    xRest=list.size()*(iFrame->leftBorder+iFrame->rightBorder)+
          list.size()*2*OS::display->GetSpaceHorizontal(OS::Display::spaceObjectBorder);

    if (list.size()>0) {
      xRest+=(list.size()-1)*OS::display->GetSpaceHorizontal(OS::Display::spaceIntraObject);
    }
    yRest=2*OS::display->GetSpaceVertical(OS::Display::spaceObjectBorder)+
          2*OS::display->GetSpaceVertical(OS::Display::spaceObjectBorder)+
          iFrame->topBorder+iFrame->bottomBorder;
    width+=xRest;
    minWidth+=xRest;
    height+=yRest;
    minHeight+=yRest;

    List::CalcSize();
  }

  void StatusLine::Layout()
  {
    size_t minSize;
    std::list<Object*>::iterator iter;

    minSize=0;
    iter=list.begin();
    while (iter!=list.end()) {
      minSize+=(*iter)->GetOWidth();
      ++iter;
    }

    while (true) {
      size_t fCount;

      fCount=0;

      iter=list.begin();
      while (iter!=list.end()) {
        if ((*iter)->CanResize(width-xRest>minSize,true)) {
          fCount++;
        }
        ++iter;
      }

      if (fCount==0) {
        break;
      }

      iter=list.begin();
      while (iter!=list.end()) {
        if ((*iter)->CanResize(width-xRest>minSize,true)) {
          size_t old;

          old=(*iter)->GetOWidth();
          (*iter)->Resize((*iter)->GetOWidth() + Base::UpDiv(width-xRest-minSize,fCount),
                          height-yRest);
          minSize+=(*iter)->GetOWidth()-old;
          fCount--;
        }
        ++iter;
      }

      if (minSize==width-xRest) {
        break;
      }
    }

    int          pos;
    OS::FrameRef iFrame=OS::display->GetFrame(OS::Display::statuscellFrameIndex);

    pos=x;

    iter=list.begin();
    while (iter!=list.end()) {
      pos+=iFrame->leftBorder+OS::display->GetSpaceHorizontal(OS::Display::spaceObjectBorder);

      (*iter)->Move(pos,this->y+(height-(*iter)->GetOHeight()) / 2);

      pos+=(*iter)->GetOWidth()+OS::display->GetSpaceHorizontal(OS::Display::spaceObjectBorder)+iFrame->rightBorder+
           OS::display->GetSpaceHorizontal(OS::Display::spaceIntraObject);

      ++iter;
    }

    List::Layout();
  }

  void StatusLine::Draw(OS::DrawInfo* draw,
                        int x, int y, size_t w, size_t h)
  {
    List::Draw(draw,x,y,w,h); /* We must call Draw of our superclass */

    if (!OIntersect(x,y,w,h)) {
      return;
    }

    /* --- */

    std::list<Object*>::iterator iter;
    int                          pos;

    pos=this->x;

    iter=list.begin();
    while (iter!=list.end()) {
      OS::FrameRef iFrame=OS::display->GetFrame(OS::Display::statuscellFrameIndex);

      iFrame->Draw(draw,
                   pos,
                   this->y+OS::display->GetSpaceVertical(OS::Display::spaceObjectBorder),
                   (*iter)->GetOWidth()+2*OS::display->GetSpaceHorizontal(OS::Display::spaceObjectBorder)+
                   iFrame->leftBorder+iFrame->rightBorder,
                   height-OS::display->GetSpaceVertical(OS::Display::spaceObjectBorder)*2);

      pos+=iFrame->leftBorder+OS::display->GetSpaceHorizontal(OS::Display::spaceObjectBorder)+
           (*iter)->GetOWidth()+OS::display->GetSpaceHorizontal(OS::Display::spaceObjectBorder)+
           iFrame->rightBorder+OS::display->GetSpaceHorizontal(OS::Display::spaceIntraObject);

      ++iter;
    }
  }
}
