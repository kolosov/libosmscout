#ifndef LUM_MODEL_CALENDAR_H
#define LUM_MODEL_CALENDAR_H

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

#include <Lum/Base/DateTime.h>

#include <Lum/Model/Simple.h>

namespace Lum {
  namespace Model {

    class LUMAPI Calendar : public Simple<Base::Calendar>
    {
    public:
      Calendar() : Simple<Base::Calendar>() {}
      Calendar(const Base::Calendar& value) : Simple<Base::Calendar>(value) {}

      bool SetToCurrent();
      bool SetDate(int dayOfMonth, int month, int year);
    };

    typedef Base::Reference<Calendar> CalendarRef;
  }
}

#endif
