#include <Lum/Model/Selection.h>

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

#include <cassert>

namespace Lum {
  namespace Model {

    Selection::Selection(Type type)
    : type(type)
    {
      // no code
    }

    void Selection::SendCellRefresh(size_t column, size_t row)
    {
      RefreshCell msg;

      msg.column=column;
      msg.row=row;

      Notify(msg);
    }

    void Selection::SendRowRefresh(size_t line)
    {
      RefreshRow msg;

      msg.row=line;

      Notify(msg);
    }

    void Selection::SendSelected()
    {
      Selected msg;

      Notify(msg);
    }

    bool Selection::HasSelectionChanged(Base::Model* model, const Base::ResyncMsg& msg) const
    {
      return model==this &&
             dynamic_cast<const Selected*>(&msg)!=NULL;
    }

    NoneSelection::NoneSelection()
    : Selection(lineBased)
    {
      // no code
    }

    bool NoneSelection::HasSelection() const
    {
      return false;
    }

    bool NoneSelection::IsCellSelected(size_t column, size_t row) const
    {
      assert(column>0 && row>0);

      return false;
    }

    bool NoneSelection::IsLineSelected(size_t line) const
    {
      assert(line>0);

      return false;
    }

    void NoneSelection::Clear()
    {
      // no code
    }

    void NoneSelection::OnLineDeleted(size_t /*line*/)
    {
      // no code
    }

    void NoneSelection::OnClear()
    {
      // no code
    }

    void NoneSelection::SelectCell(size_t /*column*/, size_t /*row*/)
    {
      // no code
    }


    SingleLineSelection::SingleLineSelection()
    : Selection(lineBased),line(0)
    {
      // no code
    }

    bool SingleLineSelection::HasSelection() const
    {
      return line!=0;
    }

    bool SingleLineSelection::IsCellSelected(size_t column, size_t row) const
    {
      assert(column>0 && row>0);

      return line==row;
    }

    bool SingleLineSelection::IsLineSelected(size_t line) const
    {
      assert(line>0);

      return this->line==line;
    }

    void SingleLineSelection::Clear()
    {
      if (line>0) {
        size_t old;

        old=line;
        line=0;

        SendRowRefresh(old);
      }
    }

    void SingleLineSelection::OnLineDeleted(size_t line)
    {
      if (line<this->line) {
        this->line--;
      }
      else if (this->line==line) {
        // TODO: Select next line if available
        this->line=0;
      }
    }

    void SingleLineSelection::OnClear()
    {
      line=0;
    }

    void SingleLineSelection::SelectLine(size_t line)
    {
      assert(line>0);

      if (this->line!=line) {
        size_t old;

        old=this->line;
        this->line=line;
        if (old>0) {
          SendRowRefresh(old);
        }
        SendRowRefresh(this->line);
        SendSelected();
      }
    }

    void SingleLineSelection::SelectCell(size_t /*column*/, size_t row)
    {
      SelectLine(row);
    }

    size_t SingleLineSelection::GetLine() const
    {
      return line;
    }

    SingleCellSelection::SingleCellSelection()
    : Selection(cellBased), row(0),column(0)
    {
      // no code
    }

    bool SingleCellSelection::HasSelection() const
    {
      return row!=0 && column!=0;
    }
    bool SingleCellSelection::IsCellSelected(size_t column, size_t row) const
    {
      assert(row>0 && column>0);

      return this->column==column && this->row==row;
    }

    bool SingleCellSelection::IsLineSelected(size_t line) const
    {
      assert(line>0);

      return false;
    }

    void SingleCellSelection::Clear()
    {
      if (column>0 && row>0) {
        size_t oldColumn,oldRow;

        oldColumn=column;
        oldRow=row;

        column=0;
        row=0;

        SendCellRefresh(oldColumn,oldRow);
      }
    }

    void SingleCellSelection::OnLineDeleted(size_t line)
    {
      if (line<row) {
        row--;
      }
      else if (line==row) {
        column=0;
        row=0;
      }
    }

    void SingleCellSelection::OnClear()
    {
      column=0;
      row=0;
    }

    void SingleCellSelection::SelectCell(size_t column, size_t row)
    {
      assert(column>0 && row>0);

      if (this->column!=column || this->row!=row) {
        size_t oldColumn,oldRow;

        oldColumn=this->column;
        oldRow=this->row;

        this->column=column;
        this->row=row;

        SendCellRefresh(oldColumn,oldRow);
        SendCellRefresh(column,row);
        SendSelected();
      }
    }

    size_t SingleCellSelection::GetRow() const
    {
      return row;
    }

    size_t SingleCellSelection::GetColumn() const
    {
      return column;
    }
  }
}
