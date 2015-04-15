#ifndef LUM_BOOLEANBUTTON_H
#define LUM_BOOLEANBUTTON_H

/*
  This source is part of the Illumination library
  Copyright (C) 2009  Tim Teulings

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

#include <Lum/State.h>

namespace Lum {

  /**
    A simple boolean button with a selected and an unselected state.
    Also know as checkbox.
  */
  class LUMAPI BooleanButton : public State
  {
  private:
    OS::ImageRef image;
    Object       *label;

  protected:
    void DrawContent(OS::DrawInfo* draw,
                     int x, int y, size_t w, size_t h);

  public:
    BooleanButton();
    ~BooleanButton();

    bool HasImplicitLabel() const;

    void SetLabel(Object *label);
    void SetLabel(const std::wstring& label);

    void CalcSize();
    void Layout();

    void PrepareForBackground(OS::DrawInfo* draw);

    static BooleanButton* Create(const std::wstring& label,
                          Base::Model* model=NULL,
                          bool horizontalFlex=false, bool verticalFlex=false);
  };
}

#endif
