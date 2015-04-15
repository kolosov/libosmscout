#ifndef LUM_MODEL_SELECTION_H
#define LUM_MODEL_SELECTION_H

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

#include <Lum/Base/Model.h>

namespace Lum {
  namespace Model {

    class LUMAPI Selection : public Base::Model
    {
    public:
      enum Type
      {
        lineBased,
        cellBased
      };

    public:
      class LUMAPI RefreshCell : public Base::ResyncMsg
      {
      public:
        int column;
        int row;
      };

      class LUMAPI RefreshRow : public Base::ResyncMsg
      {
      public:
        int row;
      };

      class LUMAPI Selected : public Base::ResyncMsg
      {
      };

    protected:
      Type type;

      void SendCellRefresh(size_t column, size_t row);
      void SendRowRefresh(size_t line);
      void SendSelected();

    public:
      Selection(Type selection);

      virtual bool HasSelection() const = 0;
      virtual bool IsCellSelected(size_t column, size_t row) const = 0;
      virtual bool IsLineSelected(size_t line) const = 0;
      virtual void Clear() = 0;
      virtual void OnLineDeleted(size_t line) = 0;
      virtual void OnClear() = 0;
      virtual void SelectCell(size_t column, size_t row) = 0;

      bool HasSelectionChanged(Base::Model* model, const Base::ResyncMsg& msg) const;
    };

#if defined(LUM_INSTANTIATE_TEMPLATES)
    LUM_EXPTEMPL template class LUMAPI Base::Reference<Selection>;
#endif

    typedef Base::Reference<Selection> SelectionRef;

    class LUMAPI NoneSelection : public Selection
    {
    public:
      NoneSelection();

      bool HasSelection() const;
      bool IsCellSelected(size_t column, size_t row) const;
      bool IsLineSelected(size_t line) const;
      void Clear();
      void OnLineDeleted(size_t line);
      void OnClear();
      void SelectCell(size_t column, size_t row);
    };

    typedef Base::Reference<NoneSelection> NoneSelectionRef;

    class LUMAPI SingleLineSelection : public Selection
    {
    private:
      size_t line;

    public:
      SingleLineSelection();

      bool HasSelection() const;
      bool IsCellSelected(size_t column, size_t row) const;
      bool IsLineSelected(size_t line) const;
      void Clear();
      void OnLineDeleted(size_t line);
      void OnClear();
      void SelectCell(size_t column, size_t row);
      void SelectLine(size_t line);

      size_t GetLine() const;
    };

    typedef Base::Reference<SingleLineSelection> SingleLineSelectionRef;

    class LUMAPI SingleCellSelection : public Selection
    {
    private:
      size_t row;
      size_t column;

    public:
      SingleCellSelection();

      bool HasSelection() const;
      bool IsCellSelected(size_t column, size_t row) const;
      bool IsLineSelected(size_t line) const;
      void Clear();
      void OnLineDeleted(size_t line);
      void OnClear();
      void SelectCell(size_t column, size_t row);

      size_t GetRow() const;
      size_t GetColumn() const;
    };

    typedef Base::Reference<SingleCellSelection> SingleCellSelectionRef;
  }
}

#endif
