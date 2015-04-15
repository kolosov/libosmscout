#ifndef LUM_DEF_PROPS_H
#define LUM_DEF_PROPS_H

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

#include <list>

#include <Lum/Def/Boolean.h>
#include <Lum/Def/Desc.h>
#include <Lum/Def/Interval.h>
#include <Lum/Def/Number.h>
#include <Lum/Def/OneOfMany.h>
#include <Lum/Def/OneOfN.h>

namespace Lum {
  namespace Def {

    class LUMAPI PropGroup
    {
    private:
      PropGroup             *parent;
      Desc                  description;
      std::list<PropGroup*> groups;
      std::list<Property*>  props;

    private:
      PropGroup(PropGroup* parent,
                const Desc& description);

    public:
      PropGroup();
      PropGroup(const PropGroup& other);
      ~PropGroup();

      // Base

      PropGroup* Boolean(const Def::Boolean& boolean);
      PropGroup* Number(const Def::Number& number);
      PropGroup* Interval(const Def::Interval& interval);
      PropGroup* OneOfN(const Def::OneOfN& oneOfN);
      PropGroup* OneOfMany(const Def::OneOfMany& oneOfMany);

      PropGroup* Group(const Desc& description);
      PropGroup* Group(const std::wstring& name);
      PropGroup* End();

      const std::list<Property*>& GetProps() const;
      const std::list<PropGroup*>& GetGroups() const;

      const Desc& GetDesc() const;

      static PropGroup* Create();
    };
  }
}

#endif
