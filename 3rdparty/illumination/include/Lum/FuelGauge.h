#ifndef LUM_FUELGAUGE_H
#define LUM_FUELGAUGE_H

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

#include <Lum/Object.h>

#include <Lum/Model/Number.h>

namespace Lum {

  /**
    An object that shows some kind of percentual level meter.

    The object expects an instance of Model::DoubleModel as model.
  */
  class LUMAPI FuelGauge : public Control
  {
  private:
    Model::NumberRef model;
    OS::FontRef      font;

  public:
    FuelGauge();

    bool SetModel(Base::Model* model);

    void CalcSize();
    void Layout();

    void Draw(OS::DrawInfo* draw,
              int x, int y, size_t w, size_t h);

    void Resync(Base::Model* model, const Base::ResyncMsg& msg);

    static FuelGauge* Create(bool horizontalFlex=false, bool verticalFlex=false);
    static FuelGauge* Create(Base::Model* model, bool horizontalFlex=false, bool verticalFlex=false);
  };
}

#endif
