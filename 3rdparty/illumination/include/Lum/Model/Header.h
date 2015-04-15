#ifndef LUM_MODEL_HEADER_H
#define LUM_MODEL_HEADER_H

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

#include <string>
#include <vector>

#include <Lum/Base/Object.h>
#include <Lum/Base/Size.h>

namespace Lum {
  namespace Model {

    /**
      Model representing information header information for example for table columns.
      The model holds information about a list of header entries /e.g. table columns).

      A header entry can have a label, a initial size and can be flaged to be flexible,
      which means that a table column can be automatically resized to fit existing space.
      This means that if the sum of the sizes of the columns is smaller than the size of
      the table the table is free to resize flaged columns to fill that space.

    */
    class LUMAPI Header : public Base::Model
    {
    public:
      /**
        If send, a column has been resized and the view should refresh that column.
      */
      class ResizedColumn : public Base::ResyncMsg
      {
      public:
        size_t column;
      };

      /**
        If send, the view is requested to fit the width of the
        column, so that all entries are completely visible.
      */
      class FitColumn : public Base::ResyncMsg
      {
      public:
        size_t column;
      };

    public:
      virtual size_t GetColumns() const = 0;

      virtual void SetColumnWidth(size_t column, size_t size) = 0;
      virtual size_t GetMinColumnWidth(size_t column) const = 0;
      virtual size_t GetColumnWidth(size_t column) const = 0;

      virtual std::wstring GetLabel(size_t column) const = 0;
      virtual bool CanColumnChangeSize(size_t column) const = 0;

      virtual void AddColumn(const std::wstring& label,
                             Base::Size::Mode mode, size_t size,
                             bool canChangeSize=false) = 0;

      virtual void SetSortColumn(size_t column) = 0;
      virtual void ClearSortColumn() = 0;
      virtual bool IsSortColumn(size_t column) const = 0;

      virtual void RequestFit(size_t column);
    };

#if defined(LUM_INSTANTIATE_TEMPLATES)
    LUM_EXPTEMPL template class LUMAPI Base::Reference<Header>;
#endif

    typedef Base::Reference<Header> HeaderRef;

    class LUMAPI HeaderImpl : public Header
    {
    private:
      struct Entry
      {
        std::wstring label;
        Base::Size   minSize;
        Base::Size   size;
        bool         canChangeSize;
      };

    private:
      std::vector<Entry> entries;
      size_t             sortColumn;

    public:
      HeaderImpl();

      size_t GetColumns() const;

      void SetColumnWidth(size_t column, size_t size);
      size_t GetMinColumnWidth(size_t column) const;
      size_t GetColumnWidth(size_t column) const;

      std::wstring GetLabel(size_t column) const;
      bool CanColumnChangeSize(size_t column) const;

      void AddColumn(const std::wstring& label,
                     Base::Size::Mode mode, size_t size,
                     bool canChangeSize=false);

      void SetSortColumn(size_t column);
      void ClearSortColumn();
      bool IsSortColumn(size_t column) const;
    };

    typedef Base::Reference<HeaderImpl> HeaderImplRef;
  }
}

#endif
