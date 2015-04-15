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

#include <Lum/TimeLine.h>

#include <math.h>

#include <iostream>
namespace Lum {

  TimeLine::TimeLine()
  : top(100),bottom(0),horizInt(0),vertInt(0),nextPos(0),state(false)
  {
    SetBackground(OS::display->GetFill(OS::Display::graphBackgroundFillIndex));

    points.resize(1);
    points[0].resize(1);
    points[0][0]=0;

    colors.resize(1);
    colors[0]=OS::display->GetColor(OS::Display::blackColor);
  }

  int TimeLine::TransformY(double value, size_t height)
  {
    return (int)floor(height-1-(value-bottom)*(height-1)/(top-bottom));
  }
  void TimeLine::ResizePoints()
  {
    size_t size;

    if (inited) {
      size=width;

      if (horizInt>0) {
        size-=2*OS::display->GetSpaceHorizontal(OS::Display::spaceObjectBorder);
      }
    }
    else {
      size=1;
    }

    if (!model.Valid()) {
      return;
    }

    // Adapt to possible channel count changes
    if (points.size()!=model->GetChannels()) {
      size_t oldSize=points.size();

      points.resize(model->GetChannels());
      colors.resize(model->GetChannels());

      for (size_t y=oldSize; y<model->GetChannels(); y++) {
        colors[y]=OS::display->GetColor(OS::Display::blackColor);
        points[y].resize(size);
      }
    }

    if (points[0].size()<size) {
      // increase size of value array, if smaller than visible area
      for (size_t y=0; y<points.size(); y++) {
        points[y].resize(size);
      }
    }
    else if (points[0].size()>size) {
      // decrease size of value array, if bigger than visible area
      size_t diff=points[0].size()-size;

      for (size_t y=0; y<points.size(); y++) {
        std::vector<double>::iterator start,stop;

        start=points[y].begin();
        stop=start+diff;
        points[y].erase(start,stop);
      }

      if (diff<=nextPos) {
        nextPos-=diff;
      }
      else {
        nextPos=0;
      }
    }
  }

  void TimeLine::CalculateBounds(int &xo, int &yo, size_t &wo, size_t &ho)
  {
    xo=x;
    yo=y;
    wo=width;
    ho=height;

    if (vertInt>0) {
      yo+=OS::display->GetSpaceVertical(OS::Display::spaceObjectBorder);
      ho-=2*OS::display->GetSpaceVertical(OS::Display::spaceObjectBorder);
    }

    if (horizInt>0) {
      xo+=OS::display->GetSpaceHorizontal(OS::Display::spaceObjectBorder);
      wo-=2*OS::display->GetSpaceHorizontal(OS::Display::spaceObjectBorder);
    }
  }

  void TimeLine::SetRange(double bottom, double top)
  {
    if (this->bottom!=bottom || this->top!=top) {
      this->bottom=bottom;
      this->top=top;

      if (visible) {
        Redraw();
      }
    }
  }

  double TimeLine::GetBottom() const
  {
    return bottom;
  }

  double TimeLine::GetTop() const
  {
    return top;
  }

  /**
    The the interval at which horizontal and vertical
    scaling lines will be displayed.
  */
  void TimeLine::SetInterval(size_t horiz, size_t vert)
  {
    if (horizInt!=horiz || vertInt!=vert) {
      horizInt=horiz;
      vertInt=vert;

      if (visible) {
        Redraw();
      }
    }
  }

  void TimeLine::SetColor(size_t channel, OS::Color color)
  {
    colors[channel]=color;

    if (visible) {
      Redraw();
    }
  }

  bool TimeLine::SetModel(Base::Model* model)
  {
    this->model=dynamic_cast<Model::DoubleDataStream*>(model);

    Control::SetModel(this->model);

    return this->model.Valid();
  }

  void TimeLine::Clear()
  {
    nextPos=0;

    if (visible) {
      Redraw();
    }
  }

  void TimeLine::CalcSize()
  {
    width=3*OS::display->GetSpaceHorizontal(OS::Display::spaceObjectBorder);
    height=3*OS::display->GetSpaceVertical(OS::Display::spaceObjectBorder);

    minWidth=width;
    minHeight=height;

    Control::CalcSize();
  }

  void TimeLine::Draw(OS::DrawInfo* draw,
                      int x, int y, size_t w, size_t h)
  {
    Control::Draw(draw,x,y,w,h);

    if (!OIntersect(x,y,w,h)) {
      return;
    }

    int          xo,yo;
    size_t       wo,ho;

    ResizePoints();

    draw->PushClip(x,y,w,h);

    CalculateBounds(xo,yo,wo,ho);

    draw->PushForeground(OS::display->GetColor(OS::Display::graphScaleColor));
    // Vertical scale
    if (horizInt>0) {
      int pos=(int)((bottom / horizInt-1)*horizInt);
      while (pos<=top) {
        // Left
        draw->DrawLine(this->x,
                       yo+TransformY(pos,ho),
                       this->x+OS::display->GetSpaceHorizontal(OS::Display::spaceObjectBorder)-1,
                       yo+TransformY(pos,ho));
        //Right
        draw->DrawLine(xo+wo,
                       yo+TransformY(pos,ho),
                       this->x+width-1,
                       yo+TransformY(pos,ho));
        pos+=horizInt;
      }
    }

    // Horizontal scale
    if (vertInt>0) {
      for (size_t pos=0; pos<points[0].size(); pos++) {
        if (pos % vertInt==0) {
          // Top
          draw->DrawLine(xo+pos,this->y,
                         xo+pos,this->y+OS::display->GetSpaceVertical(OS::Display::spaceObjectBorder)-1);
          // Bottom
          draw->DrawLine(xo+pos,yo+ho,
                         xo+pos,this->y+height-1);
        }
      }
    }
    draw->PopForeground();

    draw->PushClip(xo,yo,wo,ho);

    for (size_t pos2=0; pos2<points.size(); pos2++) {
      draw->PushForeground(colors[pos2]);
      for (size_t pos=0; pos<nextPos; pos++) {
        if (pos==0) {
          draw->DrawPoint(xo,yo+TransformY(points[pos2][0]-bottom,ho));
        }
        else {
          draw->DrawLine(xo+pos-1,yo+TransformY(points[pos2][pos-1],ho),
                         xo+pos,  yo+TransformY(points[pos2][pos],ho));
        }
      }
      draw->PopForeground();
    }

    draw->PopClip();

    draw->PopClip();
  }

  void TimeLine::Resync(Base::Model* model, const Base::ResyncMsg& /*msg*/)
  {
    if (state==this->model->IsNull()) {
      state=!this->model->IsNull();

      if (!state) {
        Redraw();
        return;
      }
      else {
        Redraw();
      }
    }

    ResizePoints();

    if (!model->IsNull()) {
      if (nextPos>=points[0].size()) {
        size_t size=points[0].size();
        for (size_t y=0; y<points.size(); y++) {
          points[y].erase(points[y].begin());
          points[y].resize(size);
        }
        nextPos=size-1;
      }

      for (size_t y=0; y<points.size(); y++) {
        points[y][nextPos]=this->model->Get(y);
      }

      Redraw();

      nextPos++;
    }
  }

  TimeLine* TimeLine::Create(bool horizontalFlex, bool verticalFlex)
  {
    TimeLine *t;

    t=new TimeLine();
    t->SetFlex(horizontalFlex,verticalFlex);

    return t;
  }

  TimeLine* TimeLine::Create(Base::Model* model, bool horizontalFlex, bool verticalFlex)
  {
    TimeLine *t;

    t=new TimeLine();
    t->SetFlex(horizontalFlex,verticalFlex);
    t->SetModel(model);

    return t;
  }

  TimeLine* TimeLine::Create(Base::Model* model, double bottom, double top,
                             bool horizontalFlex, bool verticalFlex)
  {
    TimeLine *t;

    t=new TimeLine();
    t->SetFlex(horizontalFlex,verticalFlex);
    t->SetModel(model);
    t->SetRange(bottom,top);

    return t;
  }
}
