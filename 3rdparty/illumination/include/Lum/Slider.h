#ifndef LUM_SLIDER_H
#define LUM_SLIDER_H

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

#include <Lum/Base/Size.h>

#include <Lum/Model/Number.h>

#include <Lum/OS/Image.h>

#include <Lum/Object.h>
#include <Lum/Scale.h>

namespace Lum {

  class LUMAPI Slider : public Control
  {
  protected:
    OS::FrameRef     boxFrame;
    OS::ImageRef     knob;
    Scale            *scale;
    long             offset;
    long             corr;
    Model::NumberRef model;
    bool             selected;
    bool             useScale;
    int              knobX;
    int              knobY;
    size_t           knobWidth;
    size_t           knobHeight;

  protected:
    virtual void DrawKnob(OS::DrawInfo* draw) = 0;

  public:
    Slider();

    void SetScale(bool useScale);
    bool SetModel(Base::Model* model);

    void OnMoveStart();
    void OnMoveEnd();
    void Dec();
    void Inc();

    void Resync(Base::Model* model, const Base::ResyncMsg& msg);
  };

  class LUMAPI HSlider : public Slider
  {
  private:
    void HandleMouseMovement(int x, int y);
    void DrawKnob(OS::DrawInfo* draw);

  public:
    HSlider();

    bool HasBaseline() const;
    size_t GetBaseline() const;

    void CalcSize();
    void Layout();
    void Draw(OS::DrawInfo* draw,
              int x, int y, size_t w, size_t h);

    bool HandleKeyEvent(const OS::KeyEvent& event);
    bool HandleMouseEvent(const OS::MouseEvent& event);

    static Slider* Create(bool horizontalFlex=false, bool verticalFlex=false);
    static Slider* Create(Base::Model* model,
                          bool horizontalFlex=false, bool verticalFlex=false);
    static Slider* Create(Base::Model* model,
                          bool useScale,
                          bool horizontalFlex=false, bool verticalFlex=false);
  };

  class LUMAPI VSlider : public Slider
  {
  private:
    void HandleMouseMovement(int x, int y);
    void DrawKnob(OS::DrawInfo* draw);

  public:
    VSlider();

    void CalcSize();
    void Layout();
    void Draw(OS::DrawInfo* draw,
              int x, int y, size_t w, size_t h);

    bool HandleKeyEvent(const OS::KeyEvent& event);
    bool HandleMouseEvent(const OS::MouseEvent& event);

    static Slider* Create(bool horizontalFlex=false, bool verticalFlex=false);
    static Slider* Create(Base::Model* model,
                          bool horizontalFlex=false, bool verticalFlex=false);
    static Slider* Create(Base::Model* model,
                          bool useScale,
                          bool horizontalFlex=false, bool verticalFlex=false);
  };
}

#endif
