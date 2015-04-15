#ifndef LUM_STATE_H
#define LUM_STATE_H

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

#include <Lum/Base/Object.h>

#include <Lum/Model/Boolean.h>
#include <Lum/Model/Number.h>

#include <Lum/OS/DrawInfo.h>
#include <Lum/OS/Image.h>
#include <Lum/OS/Event.h>
#include <Lum/OS/Theme.h>

#include <Lum/Object.h>

namespace Lum {

  /**
    Implementation of a bool state gadgets like Bool and Radio buttons.
  */
  class LUMAPI State : public Control
  {
  protected:
    Model::BooleanRef boolModel;
    Model::NumberRef  numberModel;
    unsigned long     index,orgState;
    bool              active;

  private:
    bool IsIn(const OS::MouseEvent& event) const;

  protected:
    virtual void DrawContent(OS::DrawInfo* draw,
                             int x, int y, size_t w, size_t h) = 0;

  public:
    State(bool active=true);
    ~State();

    void SetIndex(int index);

    bool SetModel(Base::Model* model);

    bool HandleMouseEvent(const OS::MouseEvent& event);
    bool HandleKeyEvent(const OS::KeyEvent& event);

    void PrepareForBackground(OS::DrawInfo* draw);
    void Draw(OS::DrawInfo* draw,
              int x, int y, size_t w, size_t h);

    void Resync(Base::Model* model, const Base::ResyncMsg& msg);
  };

  class LUMAPI ImageState : public State
  {
  protected:
    OS::ImageRef image;

  protected:
    void DrawContent(OS::DrawInfo* draw,
                     int x, int y, size_t w, size_t h);

  public:
    ImageState(OS::Image* image, bool active=true);

    void CalcSize();
  };
}

#endif
