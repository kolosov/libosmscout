#ifndef LUM_TIMELINE_H
#define LUM_TIMELINE_H

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

#include <vector>

#include <Lum/Model/DataStream.h>

#include <Lum/OS/Display.h>

#include <Lum/Object.h>

namespace Lum {

  /**
    An time based diagram object to display a continious
    number of values in relation to a time line by using a two
    dimensional graph, where the values is represented by the
    y-axis and the time by the x-axis. The new values are scrolled
    in from right to left.

    More then one value can be shown. Different colors can be used
    for different values.
  */
  class LUMAPI TimeLine : public Control
  {
  private:
    double                            top;      //< top value to be shown
    double                            bottom;   //< bottom value to be shown
    size_t                            horizInt; //< Intervall for horizontal markers
    size_t                            vertInt;  //< Intervall for vertical markers
    size_t                            nextPos;
    std::vector<std::vector<double> > points;
    std::vector<OS::Color>            colors;
    bool                              state;
    Model::DoubleDataStreamRef        model;

  private:
    void ResizePoints();
    void CalculateBounds(int &xo, int &yo, size_t &wo, size_t &ho);
    int TransformY(double value, size_t height);

  public:
    TimeLine();

    // setter
    void SetRange(double bottom, double top);
    void SetInterval(size_t horiz, size_t vert);
    void SetColor(size_t channel, OS::Color color);
    bool SetModel(Base::Model* model);

    void Clear();
    double GetBottom() const;
    double GetTop() const;

    void CalcSize();
    void Draw(OS::DrawInfo* draw,
              int x, int y, size_t w, size_t h);
    void Resync(Base::Model* model, const Base::ResyncMsg& msg);

    static TimeLine* Create(bool horizontalFlex=false, bool verticalFlex=false);
    static TimeLine* Create(Base::Model* model, bool horizontalFlex=false, bool verticalFlex=false);
    static TimeLine* Create(Base::Model* model, double bottom, double top, bool horizontalFlex=false, bool verticalFlex=false);
  };
}

#endif
