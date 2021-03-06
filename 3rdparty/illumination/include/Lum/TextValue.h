#ifndef LUM_TEXTVALUE_H
#define LUM_TEXTVALUE_H

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

#include <Lum/Model/String.h>

#include <Lum/Object.h>

namespace Lum {

  class LUMAPI TextValue : public Control
  {
  public:
    enum Alignment {
      left,    /** Text will be displayed left aligned. */
      right,   /** Text will displayed right aligned. */
      centered /** Text will be displayed centered. */
    };

  private:
    Model::StringRef model;
    Lum::OS::FontRef font;
    Alignment        alignment;
    unsigned long    style;

  public:
    TextValue();
    virtual ~TextValue();

    bool HasBaseline() const;
    size_t GetBaseline() const;

    bool SetModel(Base::Model* model);

    void SetFont(OS::Font *font);
    void SetStyle(unsigned long style);
    void SetAlignment(Alignment alignment);

    void CalcSize();
    void Draw(OS::DrawInfo* draw,
              int x, int y, size_t w, size_t h);

    void Resync(Base::Model* model, const Base::ResyncMsg& msg);

    static TextValue* Create(bool horizontalFlex=false, bool verticalFlex=false);
    static TextValue* Create(Base::Model* model,
                             bool horizontalFlex=false, bool verticalFlex=false);
    static TextValue* Create(Base::Model* model,
                             Alignment alignment,
                             bool horizontalFlex=false, bool verticalFlex=false);
  };
}

#endif
