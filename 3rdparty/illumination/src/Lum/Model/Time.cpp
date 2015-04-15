#include <Lum/Model/Time.h>

/*
  This source is part of the Illumination library
  Copyright (C) 2005  Tim Teulings

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

#include <assert.h>

namespace Lum {
  namespace Model {

    bool Time::SetToNow()
    {
      Base::Time now;

      return SetTime(now.GetHour(),now.GetMinute(),now.GetSecond());
    }

    bool Time::SetTime(unsigned char hour, unsigned char minute, unsigned char second)
    {
      if (!value.isNull &&
          value.value.GetHour()==hour &&
          value.value.GetMinute()==minute &&
          value.value.GetSecond()==second) {
        return false;
      }

      value.value.Set(hour,minute,second);
      value.isNull=false;

      Notify();

      return true;
    }

    void Time::IncHour()
    {
      assert(!value.isNull);

      value.value.IncHour();
    }

    void Time::DecHour()
    {
      assert(!value.isNull);

      value.value.DecHour();
    }

    void Time::IncMinute()
    {
      assert(!value.isNull);

      value.value.IncMinute();
    }

    void Time::DecMinute()
    {
      assert(!value.isNull);

      value.value.DecMinute();
    }

    void Time::IncSecond()
    {
      assert(!value.isNull);

      value.value.IncSecond();
    }

    void Time::DecSecond()
    {
      assert(!value.isNull);

      value.value.DecSecond();
    }
  }
}
