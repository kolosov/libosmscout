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

#include <Lum/Def/Props.h>

#include <Lum/Base/L10N.h>

#include <Lum/OS/Event.h>

namespace Lum {
  namespace Def {

    PropGroup::PropGroup(PropGroup* parent, const Desc& description)
    : parent(parent),
    description(description)
    {
      // no code
    }

    PropGroup::PropGroup()
    : parent(NULL)
    {
      // no code
    }

    PropGroup::PropGroup(const PropGroup& other)
    {
      parent=NULL;
      description=other.description;

      for (std::list<Property*>::const_iterator prop=other.props.begin();
           prop!=other.props.end();
           ++prop) {
        props.push_back((*prop)->Copy());
      }

      for (std::list<PropGroup*>::const_iterator group=other.groups.begin();
           group!=other.groups.end();
           ++group) {
        PropGroup *g=new PropGroup(*(*group));

        g->parent=this;

        groups.push_back(g);
      }
    }

    PropGroup::~PropGroup()
    {
      for (std::list<Property*>::iterator prop=props.begin();
           prop!=props.end();
           ++prop) {
        delete *prop;
      }

      for (std::list<PropGroup*>::const_iterator group=groups.begin();
           group!=groups.end();
           ++group) {
        delete *group;
      }
    }


    PropGroup* PropGroup::Boolean(const Def::Boolean& boolean)
    {
      props.push_back(boolean.Copy());

      return this;
    }

    PropGroup* PropGroup::Number(const Def::Number& number)
    {
      props.push_back(number.Copy());

      return this;
    }

    PropGroup* PropGroup::Interval(const Def::Interval& interval)
    {
      props.push_back(interval.Copy());

      return this;
    }

    PropGroup* PropGroup::OneOfMany(const Def::OneOfMany& oneOfMany)
    {
      props.push_back(oneOfMany.Copy());

      return this;
    }

    PropGroup* PropGroup::OneOfN(const Def::OneOfN& oneOfN)
    {
      props.push_back(oneOfN.Copy());

      return this;
    }

    PropGroup* PropGroup::Group(const Def::Desc& description)
    {
      PropGroup *group=new PropGroup(this,description);

      groups.push_back(group);

      return group;
    }

    PropGroup* PropGroup::Group(const std::wstring& name)
    {
      PropGroup *group=new PropGroup(this,Desc(name));

      groups.push_back(group);

      return group;
    }

    PropGroup* PropGroup::End()
    {
      return parent;
    }

    const Desc& PropGroup::GetDesc() const
    {
      return description;
    }

    const std::list<Property*>& PropGroup::GetProps() const
    {
      return props;
    }

    const std::list<PropGroup*>& PropGroup::GetGroups() const
    {
      return groups;
    }

    PropGroup* PropGroup::Create()
    {
      return new PropGroup();
    }
  }
}
