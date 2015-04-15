#ifndef LUM_RGBCONTROL_H
#define LUM_RGBCONTROL_H

/*
  This source is part of the Illumination library
  Copyright (C) 2007  Tim Teulings

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

#include <Lum/Model/Color.h>
#include <Lum/Model/Number.h>

#include <Lum/Object.h>

namespace Lum {

  /**
    Layout group to blend in dialog like mini windows above and below the
    main object by temporary shrinking the main object.
  */
  class LUMAPI RGBControl : public ControlComponent
  {
  private:
    Model::ULongRef r;
    Model::ULongRef g;
    Model::ULongRef b;
    Model::ColorRef model;

  public:
    RGBControl();
    virtual ~RGBControl();

    bool SetModel(Base::Model* model);

    void CalcSize();

    void Resync(Base::Model* model, const Base::ResyncMsg& msg);
  };
}

#endif

