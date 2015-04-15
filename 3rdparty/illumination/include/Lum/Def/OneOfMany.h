#ifndef LUM_DEF_ONEOFMANY_H
#define LUM_DEF_ONEOFMANY_H

/*
  This source is part of the Illumination library
  Copyright (C) 2009  Tim Teulings

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

#include <string>

#include <Lum/Private/ImportExport.h>

#include <Lum/Def/Property.h>

#include <Lum/Model/Header.h>
#include <Lum/Model/Number.h>
#include <Lum/Model/Table.h>

#include <Lum/TableView.h>

namespace Lum {
  namespace Def {
    /**
      Define a one of n choice (index), where the n is dynamic (list model).

      In difference to OneOfN the number and value of the choices is dynamic and
      not definition time static. neOfN will be realized by using radio menues, radio
      buttons or combo boxes. OneOfMany will always fall back to use a table for
      value selection.
     */
    class LUMAPI OneOfMany : public Property
    {
    private:
      Model::NumberRef    value;
      Model::TableRef     choices;
      Model::HeaderRef    choicesHeader;
      TableCellPainterRef tablePainter;

    public:
      OneOfMany(const Desc& description,
                Model::Number* value,
                Model::Table* choices,
                TableCellPainter* tablePainter=NULL);

      Property* Copy() const;

      OneOfMany& SetChoicesHeader(Model::Header* choicesHeader);

      Model::Number* GetValue() const;
      Model::Table* GetChoices() const;
      Model::Header* GetChoicesHeader() const;
      TableCellPainter* GetTableCellPainter() const;
    };
  }
}

#endif

