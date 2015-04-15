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

#include <Lum/PosMarker.h>

#include <Lum/Base/Util.h>

#include <Lum/OS/Display.h>
#include <Lum/OS/Theme.h>

namespace Lum {

  PosMarker::PosMarker(bool vert)
  : corr(0),
    adjustment(NULL),
    vert(vert),
    lightColor(0.75,0.75,0.75,OS::Display::whiteColor),
    darkColor(0.25,0.25,0.25,OS::Display::blackColor)
  {
    // no code
  }

  PosMarker::~PosMarker()
  {
    // no code
  }

  /**
    Don't use it!
  */
  void PosMarker::SetOffset(int offset)
  {
    corr=offset;
  }

  bool PosMarker::SetModel(Base::Model* model)
  {
    adjustment=dynamic_cast<Model::Adjustment*>(model);

    Control::SetModel(adjustment);

    return adjustment.Valid();
  }

  void PosMarker::CalcSize()
  {
    if (vert) {
      width=10;
      height=5;
    }
    else {
      width=5;
      height=10;
    }

    minWidth=width;
    minHeight=height;

    Control::CalcSize();
  }

  void PosMarker::Draw(OS::DrawInfo* draw,
                       int x, int y, size_t w, size_t h)
  {
    Control::Draw(draw,x,y,w,h);

    if (!OIntersect(x,y,w,h)) {
      return;
    }

    /* --- */

    if (!adjustment.Valid() ||
        !adjustment->IsValid() ||
        adjustment->GetVisible()==adjustment->GetTotal()) {
      return;
    }

    size_t kSize,bSize;
    int    kStart;

    if (vert) {
      bSize=height;
    }
    else {
      bSize=width;
    }

    if (adjustment->GetTotal()+corr==0) {
      kSize=bSize;
      kStart=0;
    }
    else {
      kSize=Base::RoundDiv((bSize*adjustment->GetVisible()),adjustment->GetTotal()+corr);
      kStart=Base::RoundDiv(bSize*(adjustment->GetTop()+corr-1),adjustment->GetTotal()+corr);
    }

    if (vert) {
      if (kSize<10) {
        kSize=10;
      }
    }
    else {
      if (kSize<10) {
        kSize=10;
      }
    }

    if (kSize>bSize) {
      kSize=bSize;
    }

    if (kStart+kSize>bSize) {
      kStart=bSize-kSize;
    }

    if (OS::display->GetTheme()->IsLightTheme()) {
      draw->PushForeground(darkColor);
    }
    else {
      draw->PushForeground(lightColor);
    }

    if (vert) {
      draw->FillRectangle(this->x,this->y+kStart,width,kSize);
    }
    else {
      draw->FillRectangle(this->x+kStart,this->y,kSize,height);
    }
    draw->PopForeground();
  }

  void PosMarker::Resync(Base::Model* model, const Base::ResyncMsg& msg)
  {
    if (model==adjustment && visible) {
      Redraw();
    }
  }
}
