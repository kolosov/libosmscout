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

#include <Lum/Model/Header.h>

#include <cassert>

namespace Lum {
  namespace Model {

    /**
      Send FitColumn to view to request the view to resize
      the referenced \p column so that all entries are completely
      visible.
    */
    void Header::RequestFit(size_t column)
    {
      FitColumn msg;

      msg.column=column;
      Notify(msg);
    }

    HeaderImpl::HeaderImpl()
    : sortColumn((size_t)-1)
    {
      // no code
    }

    size_t HeaderImpl::GetColumns() const
    {
      return entries.size();
    }

    void HeaderImpl::SetColumnWidth(size_t column, size_t size)
    {
      ResizedColumn msg;

      assert(column<entries.size());

      if (size>=entries[column].minSize.GetSize()) {
        entries[column].size.SetSize(Base::Size::pixel,size);
      }
      else {
        entries[column].size.SetSize(Base::Size::pixel,entries[column].minSize.GetSize());
      }

      msg.column=column;
      Notify(msg);
    }

    size_t HeaderImpl::GetMinColumnWidth(size_t column) const
    {
      assert(column<entries.size());

      return entries[column].minSize.GetSize();
    }

    size_t HeaderImpl::GetColumnWidth(size_t column) const
    {
      assert(column<entries.size());

      return entries[column].size.GetSize();
    }

    std::wstring HeaderImpl::GetLabel(size_t column) const
    {
      assert(column<entries.size());

      return entries[column].label;
    }

    bool HeaderImpl::CanColumnChangeSize(size_t column) const
    {
      assert(column<entries.size());

      return entries[column].canChangeSize &&
             entries[column].size.GetSize()>=entries[column].minSize.GetSize();
    }

    void HeaderImpl::AddColumn(const std::wstring& label,
                               Base::Size::Mode mode, size_t size,
                               bool canChangeSize)
    {
      Entry entry;

      entry.label=label;
      entry.minSize.SetSize(mode,size);
      entry.size.SetSize(mode,size);
      entry.canChangeSize=canChangeSize;

      entries.push_back(entry);

      Notify();
    }

    void HeaderImpl::SetSortColumn(size_t column)
    {
      sortColumn=column;
    }

    void HeaderImpl::ClearSortColumn()
    {
      sortColumn=(size_t)-1;
    }

    bool HeaderImpl::IsSortColumn(size_t column) const
    {
      return sortColumn==column;
    }
  }
}
