#ifndef LUM_DEF_INTERVAL_H
#define LUM_DEF_INTERVAL_H

/*
  This source is part of the Illumination library
  Copyright (C) 2011  Tim Teulings

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

#include <Lum/Private/ImportExport.h>

#include <Lum/Def/Property.h>

#include <Lum/Model/Number.h>

namespace Lum {

  namespace Def {
    /**
     */
    class LUMAPI Interval : public Property
    {
    private:
      Model::NumberRef start;
      Model::NumberRef end;

    public:
      Interval(const Desc& description,
               Model::Number* start,
               Model::Number* end);

      Property* Copy() const;

      Model::Number* GetStart() const;
      Model::Number* GetEnd() const;
    };
  }
}

#endif

