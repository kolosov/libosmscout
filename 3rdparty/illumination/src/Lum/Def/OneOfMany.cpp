/*
  This source is part of the Illumination library
  Copyright (C) 200  Tim Teulings

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

#include <Lum/Def/OneOfMany.h>

namespace Lum {
  namespace Def {
    OneOfMany::OneOfMany(const Desc& description,
                         Model::Number* value,
                         Model::Table* choices,
                         TableCellPainter* tablePainter)
     : Property(description),
       value(value),
       choices(choices),
       tablePainter(tablePainter)
    {
      assert(value!=NULL);
      assert(choices!=NULL);

      choicesHeader=new Model::HeaderImpl();
      choicesHeader->AddColumn(L"",Lum::Base::Size::pixel,0);
    }

    Property* OneOfMany::Copy() const
    {
      OneOfMany *result=new OneOfMany(GetDesc(),
                                      value,
                                      choices,
                                      tablePainter);

      result->SetChoicesHeader(choicesHeader);

      return result;
    }

    OneOfMany& OneOfMany::SetChoicesHeader(Model::Header* choicesHeader)
    {
      assert(choicesHeader!=NULL);

      this->choicesHeader=choicesHeader;

      return *this;
    }

    Model::Number* OneOfMany::GetValue() const
    {
      return value;
    }

    Model::Table* OneOfMany::GetChoices() const
    {
      return choices;
    }

    Model::Header* OneOfMany::GetChoicesHeader() const
    {
      return choicesHeader;
    }

    TableCellPainter* OneOfMany::GetTableCellPainter() const
    {
      return tablePainter.Get();
    }
  }
}

