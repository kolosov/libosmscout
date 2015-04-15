#ifndef LUM_MODEL_ADJUSTMENT_H
#define LUM_MODEL_ADJUSTMENT_H

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

#include <Lum/Base/Model.h>

#include <Lum/Model/Number.h>

namespace Lum {
  namespace Model {

    class LUMAPI Adjustment : public Base::Model
    {
    private:
      ULongRef      top;
      ULongRef      visible;
      ULongRef      total;
      unsigned long stepSize;

    public:
      Adjustment();

      void On(bool notify=true);
      void Off();

      bool IsValid() const;
      unsigned long GetTop() const;
      unsigned long GetBottom() const;
      unsigned long GetVisible() const;
      unsigned long GetTotal() const;
      bool IsCompletelyVisible() const;

      void Set(unsigned long top, unsigned long visible, unsigned long total);
      void SetInvalid();
      void SetTop(unsigned long top);
      void SetDimension(unsigned long visible, unsigned long total);

      Number* GetTopModel() const;
      Number* GetVisibleModel() const;
      Number* GetTotalModel() const;

      void IncTop();
      void IncTop(unsigned long stepSize);
      void DecTop();
      void DecTop(unsigned long stepSize);
      void PageBack();
      void PageForward();

      void MakeVisible(unsigned long pos);
      void CenterOn(unsigned long pos);
      void ShowFirstPage();
      void ShowLastPage();

      void SetStepSize(unsigned long stepSize);
      unsigned long GetStepSize() const;
    };

#if defined(LUM_INSTANTIATE_TEMPLATES)
    LUM_EXPTEMPL template class LUMAPI Base::Reference<Adjustment>;
#endif

    typedef Base::Reference<Adjustment> AdjustmentRef;
  }
}

#endif
