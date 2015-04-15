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

#include <Lum/Model/Table.h>

namespace Lum {
  namespace Model {

    Table::Table()
    {
      // no code
    }

    Table::~Table()
    {
      // no code
    }

    void Table::RedrawRow(size_t row)
    {
      RefreshRow msg;

      msg.row=row;

      Notify(msg);
    }

    void Table::RedrawCell(size_t column, size_t row)
    {
      RefreshCell msg;

      msg.row=row;
      msg.column=column;

      Notify(msg);
    }

    void Table::NotifyInsert(size_t row, size_t count)
    {
      InsertRow msg;

      msg.row=row;
      msg.count=count;

      Notify(msg);
    }

    void Table::NotifyDelete(size_t row, size_t count)
    {
      DeleteRow msg;

      msg.row=row;
      msg.count=count;

      Notify(msg);
    }

    /**
     You can assign a text to the model that might be displayed in the table if
     there is no entry in the table.
     */
    void Table::SetEmptyText(const std::wstring& text)
    {
      if (emptyText!=text) {
        emptyText=text;

        if (GetRows()==0) {
          Notify();
        }
      }
    }

    /**
      Returns the text earlier assign using SetEmptyText. This text might be displayed in the
      table if the table is empty.
     */
    std::wstring Table::GetEmptyText() const
    {
      return emptyText;
    }

    bool Table::Sort(size_t column, bool down)
    {
      // base class does not implement this functionality!
      return false;
    }
  }
}
