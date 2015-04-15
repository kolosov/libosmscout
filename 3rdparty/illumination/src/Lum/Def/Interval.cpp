/*
  This source is part of the Illumination library
  Copyright (C) 2010  Tim Teulings

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

#include <Lum/Def/Interval.h>

namespace Lum {
  namespace Def {

    Interval::Interval(const Desc& description,
                       Model::Number* start,
                       Model::Number* end)
    : Property(description),
      start(start),
      end(end)
    {
      assert(start!=NULL);
      assert(end!=NULL);
    }

    Property* Interval::Copy() const
    {
      return new Interval(GetDesc(),start,end);
    }

    Model::Number* Interval::GetStart() const
    {
      return start;
    }

    Model::Number* Interval::GetEnd() const
    {
      return end;
    }
  }
}
