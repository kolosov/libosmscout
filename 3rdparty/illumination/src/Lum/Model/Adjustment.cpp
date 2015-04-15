#include <Lum/Model/Adjustment.h>

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

#include <algorithm>

namespace Lum {
  namespace Model {

    Adjustment::Adjustment()
    : top(new ULong()),
      visible(new ULong()),
      total(new ULong()),
      stepSize(1)
    {
      // no code
    }

    void Adjustment::On(bool notify)
    {
      top->On(notify);
      visible->On(notify);
      total->On(notify);

      Model::On(notify);
    }

    void Adjustment::Off()
    {
      top->Off();
      visible->Off();
      total->Off();

      Model::Off();
    }

    /**
      As it is impossible to ensure that the relation between
      top, visible and total is not invalidated, this procedure
      provides a simple check.
      post: 0<top<=total and 0<visible OR result is FALSE.
     */
    bool Adjustment::IsValid() const
    {
      return !(top->IsNull() || visible->IsNull() || total->IsNull()
               || visible->GetUnsignedLong()==0
               || visible->GetUnsignedLong()>total->GetUnsignedLong()
               || top->GetUnsignedLong()<1);
    }

    unsigned long Adjustment::GetTop() const
    {
      if (top->IsNull()) {
        return 1;
      }
      else {
        return top->GetUnsignedLong();
      }
    }

    unsigned long Adjustment::GetBottom() const
    {
      return top->GetUnsignedLong()+visible->GetUnsignedLong()-1;
    }

    unsigned long Adjustment::GetVisible() const
    {
      return visible->GetUnsignedLong();
    }

    unsigned long Adjustment::GetTotal() const
    {
      return total->GetUnsignedLong();
    }

    bool Adjustment::IsCompletelyVisible() const
    {
      return GetVisible()==GetTotal();
    }

    /**
      Set a new value for top, visible and total.
      post: if visible>=1 then new value is accepted.
      post: top will be corrected to fit visible and total
     */
    void Adjustment::Set(unsigned long top, unsigned long visible, unsigned long total)
    {
      bool t,v,a;

      if (visible==0 || total==0) {
        SetInvalid();

        return;
      }

      if (visible>total) {
        visible=total;
      }

      if (visible==total) {
        top=1;
      }

      if (top+visible>total) {
        top=total-visible+1;
      }

      // To make sure, that all values are consistent if one of the three models
      // triggers it change, we first switch off the changed models, set all values
      // and then switch all changed models back on.

      a=this->total->IsNull() || this->total->GetUnsignedLong()!=total;
      v=this->visible->IsNull() ||this->visible->GetUnsignedLong()!=visible;
      t=this->top->IsNull() ||this->top->GetUnsignedLong()!=top;

      if (a) {
        this->total->Off();
        this->total->Set(total);
      }
      if (v) {
        this->visible->Off();
        this->visible->Set(visible);
      }
      if (t) {
        this->top->Off();
        this->top->Set(top);
      }

      if (a) {
        this->total->On();
      }
      if (v) {
        this->visible->On();
      }
      if (t) {
        this->top->On();
      }

      if (t || v || a) {
        Notify();
      }
    }

    void Adjustment::SetInvalid()
    {
      if (IsValid()) {
        // To make sure, that all values are consistent if one of the three models
        // triggers it change, we first switch off the changed models, set all values
        // and then switch all changed models back on.

        top->Off();
        visible->Off();
        total->Off();

        top->SetNull();
        visible->SetNull();
        total->SetNull();

        top->On();
        visible->On();
        total->On();

        Notify();
      }
    }

    /**
      Set a new value for top.
      post: if top>=1 & top<=total then new value is accepted.
     */
    void Adjustment::SetTop(unsigned long top)
    {
      assert(top>=1);
      assert(top<=total->GetUnsignedLong()-visible->GetUnsignedLong()+1);

      Set(top,GetVisible(),GetTotal());
    }

    /**
      Set a new value for visible and total.
      post: if visible>=1 then new value is accepted.
      post: top will be corrected to fit visible and total
     */
    void Adjustment::SetDimension(unsigned long visible, unsigned long total)
    {
      unsigned long top;

      if (visible==0 || total==0) {
        return;
      }

      if (IsValid()) {
        top=GetTop();
      }
      else {
        top=1;
      }

      if (visible>total) {
        visible=total;
      }

      if (visible==total) {
        top=1;
      }

      if (top+visible>total) {
        top=total-visible+1;
      }

      Set(top,visible,total);
    }

    Number* Adjustment::GetTopModel() const
    {
      return top.Get();
    }

    Number* Adjustment::GetVisibleModel() const
    {
      return visible.Get();
    }

    Number* Adjustment::GetTotalModel() const
    {
      return total.Get();
    }

    void Adjustment::IncTop()
    {
      IncTop(stepSize);
    }

    void Adjustment::IncTop(unsigned long stepSize)
    {
      assert(IsValid());

      if (top->GetUnsignedLong()+stepSize-1<=total->GetUnsignedLong()-visible->GetUnsignedLong()) {
        top->Add(stepSize);
        Notify();
      }
      else if (top->GetUnsignedLong()<=total->GetUnsignedLong()-visible->GetUnsignedLong()) {
        top->Set(total->GetUnsignedLong()-visible->GetUnsignedLong()+1);
        Notify();
      }
    }

    void Adjustment::DecTop()
    {
      DecTop(stepSize);
    }

    void Adjustment::DecTop(unsigned long stepSize)
    {
      assert(IsValid());

      if (top->GetUnsignedLong()>=1+stepSize) {
        top->Sub(stepSize);
        Notify();
      }
      else if (top->GetUnsignedLong()>1) {
        top->Set(1);
        Notify();
      }
    }

    void Adjustment::PageBack()
    {
      assert(IsValid());

      if (top->GetUnsignedLong()>visible->GetUnsignedLong()) {
        top->Sub(visible->GetUnsignedLong());
        Notify();
      }
      else if (top->GetUnsignedLong()!=1) {
        top->Set(1);
        Notify();
      }
    }

    void Adjustment::PageForward()
    {
      assert(IsValid());

      if (top->GetUnsignedLong()+visible->GetUnsignedLong()<total->GetUnsignedLong()-visible->GetUnsignedLong()+1) {
        top->Add(visible->GetUnsignedLong());
        Notify();
      }
      else if (top->GetUnsignedLong()!=total->GetUnsignedLong()-visible->GetUnsignedLong()+1) {
        top->Set(total->GetUnsignedLong()-visible->GetUnsignedLong()+1);
        Notify();
      }
    }

    void Adjustment::MakeVisible(unsigned long pos)
    {
      assert(IsValid());

      if (pos>=1 && pos<=total->GetUnsignedLong()) {
        if (pos<top->GetUnsignedLong()) {
          top->Set(pos);
          Notify();
        }
        else if (pos>top->GetUnsignedLong()+visible->GetUnsignedLong()-1) {
          if (pos<visible->GetUnsignedLong()) {
            top->Set(1);
          }
          else {
            top->Set(pos-visible->GetUnsignedLong()+1);
          }
          Notify();
        }
      }
    }

    void Adjustment::CenterOn(unsigned long pos)
    {
      assert(IsValid());

      if (pos<=visible->GetUnsignedLong()/2) {
        ShowFirstPage();
      }
      else if (pos>=total->GetUnsignedLong()-visible->GetUnsignedLong()/2+1) {
        ShowLastPage();
      }
      else {
        SetTop(pos-visible->GetUnsignedLong()/2);
      }
    }

    void Adjustment::ShowFirstPage()
    {
      assert(IsValid());

      SetTop(1);
    }
    void Adjustment::ShowLastPage()
    {
      assert(IsValid());

      SetTop(total->GetUnsignedLong()-visible->GetUnsignedLong()+1);
    }

    void Adjustment::SetStepSize(unsigned long stepSize)
    {
      this->stepSize=stepSize;
    }

    unsigned long Adjustment::GetStepSize() const
    {
      return stepSize;
    }

  }
}
