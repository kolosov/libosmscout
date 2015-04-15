#ifndef LUM_MODEL_TABLE_H
#define LUM_MODEL_TABLE_H

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

#include <algorithm>
#include <list>
#include <vector>

#include <Lum/Base/Object.h>

#include <Lum/Object.h>

namespace Lum {
  namespace Model {

    /**
      Abstract baseclass for all table models. Its purpose is,
      to define an common interface for all tablemodels. The
      table object itself will only talk with the model through
      this interface.
    */
    class LUMAPI Table : public Base::Model
    {
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

      class LUMAPI InsertRow : public Base::ResyncMsg
      {
      public:
        int    row;
        size_t count;
      };

      class LUMAPI DeleteRow : public Base::ResyncMsg
      {
      public:
        int    row;
        size_t count;
      };

    private:
      std::wstring emptyText;

    public:
      Table();
      ~Table();

      void RedrawRow(size_t row);
      void RedrawCell(size_t column, size_t row);
      void NotifyInsert(size_t row, size_t count);
      void NotifyDelete(size_t row, size_t count);

      void SetEmptyText(const std::wstring& text);
      std::wstring GetEmptyText() const;

      virtual size_t GetRows() const = 0;

      virtual bool Sort(size_t column, bool ascending=true);
    };

#if defined(LUM_INSTANTIATE_TEMPLATES)
    LUM_EXPTEMPL template class LUMAPI Base::Reference<Table>;
#endif

    typedef Base::Reference<Table> TableRef;

    /**
      Abstract base class for TableModel implementations that wrap stdandard library
      containers.
     */
    template<typename Element, typename Container>
    class StdTableBase : public Table
    {
    public:
      typedef Container                                  ContainerType;
      typedef Element                                    ElementType;
      typedef typename ContainerType::iterator           Iterator;

    public:
      /**
        Optionally assignable function object for sorting of tables. If no Comp
        no sorting is possible.
       */
      class Comparator
      {
      public:
        inline virtual ~Comparator()
        {
          // no code
        }

        virtual bool operator()(const Element& a, const Element& b, size_t column, bool ascending) const = 0;
      };

    protected:
      /**
        Internal helper class.
       */
      class InternalComparator
      {
      private:
        Comparator *comparator;
        size_t     column;
        bool       ascending;

      public:
        InternalComparator(Comparator* comparator, size_t column, bool ascending)
        : comparator(comparator),
          column(column),
          ascending(ascending)
        {
          // no code
        }

        bool operator()(const Element& a, const Element& b) const
        {
          return comparator->operator()(a,b,column,ascending);
        }
      };

    private:
      mutable Iterator current;
      mutable size_t   currentPos;
      mutable bool     currentInvalid;
      Comparator       *comparator;

    private:
      void GotoEntry(size_t row) const
      {
        assert(row>=1 && row<=GetContainer().size());

        if (currentInvalid) {
          // current is mutable,but compiler chooses const version of GetContainer()
          current=const_cast<StdTableBase*>(this)->GetContainer().begin();
          currentPos=1;
        }

        if (currentPos<row) {
          while (currentPos<row) {
            ++current;
            ++currentPos;
          }
        }
        else if (currentPos>row) {
          while (currentPos>row) {
            --current;
            --currentPos;
          }
        }

        assert(current!=GetContainer().end());
      }

    protected:
      void InvalidateCurrent()
      {
        currentInvalid=true;
      }

    public:
      virtual Container& GetContainer() = 0;
      virtual const Container& GetContainer() const = 0;

      StdTableBase(Comparator* comparator=NULL)
       : currentPos(0),
         currentInvalid(true),
         comparator(comparator)
      {
        // no code
      }

      ~StdTableBase()
      {
        delete comparator;
      }

      size_t GetRows() const
      {
        return GetContainer().size();
      }

      virtual const Element& GetEntry(size_t row) const
      {
        GotoEntry(row);

        return *current;
      }

      /**
        You need to manually call RereshRow after changing the entry!
       */
      virtual Element& GetEntry(size_t row)
      {
        GotoEntry(row);

        return *current;
      }

      // Insert
      // SwapWithNext
      virtual Element Delete(size_t row)
      {
        GotoEntry(row);

        ElementType value=*current;

        GetContainer().erase(current);

        InvalidateCurrent();

        NotifyDelete(row,1);

        return value;
      }

      virtual void Prepend(const Element& entry)
      {
        GetContainer().insert(GetContainer().begin(),entry);
        currentPos++;
        NotifyInsert(1,1);
      }

      virtual void Append(const Element& entry)
      {
        GetContainer().push_back(entry);

        InvalidateCurrent();

        NotifyInsert(GetContainer().size(),1);
      }

      virtual void Clear()
      {
        InvalidateCurrent();

        GetContainer().clear();

        Notify();
      }

      Comparator* GetComparator() const
      {
        return comparator;
      }

      /**
        Sort by the given column up or down.
       */
      virtual bool Sort(size_t /*column*/, bool /*ascending=true*/)
      {
        return false;
      }
    };

    template<typename Element, typename Container>
    class StdTableBaseSorted : public StdTableBase<Element,Container>
    {
    public:
      StdTableBaseSorted(typename StdTableBase<Element, Container>::Comparator* comparator=NULL)
       : StdTableBase<Element,Container>(comparator)
      {
        // no code
      }

      bool Sort(size_t column, bool ascending=true)
      {
        if (this->GetComparator()!=NULL) {
          std::sort(this->GetContainer().begin(),
                    this->GetContainer().end(),
                    typename StdTableBase<Element, Container>::InternalComparator(this->GetComparator(),
                                                                                  column,ascending));
          this->InvalidateCurrent();

          this->Notify();

          return true;
        }
        else {
          return false;
        }
      }
    };

    template <typename Element>
    class StdTableBaseSorted<Element,std::list<Element> > : public StdTableBase<Element,std::list<Element> >
    {
    public:
      StdTableBaseSorted(typename StdTableBase<Element,std::list<Element> >::Comparator* comparator=NULL)
       : StdTableBase<Element,std::list<Element> >(comparator)
      {
        // no code
      }

      bool Sort(size_t column, bool ascending=true)
      {
        if (this->GetComparator()!=NULL) {
          this->GetContainer().sort(typename StdTableBase<Element,std::list<Element> >::InternalComparator(this->GetComparator(),
                                                                                                 column,ascending));
          this->InvalidateCurrent();

          this->Notify();

          return true;
        }
        else {
          return false;
        }
      }
    };

    /**
      TableModel implementation that wraps an instancance of the given container of type
      Container holding elements of type Element.
     */
    template<typename Element, typename Container = std::list<Element> >
    class StdTable : public StdTableBaseSorted<Element,Container>
    {

    private:
      mutable typename StdTableBase<Element, Container>::ContainerType container;

    public:
      StdTable(typename StdTableBase<Element, Container>::Comparator* comparator=NULL)
       : StdTableBaseSorted<Element,Container>(comparator)
      {
        // no code
      }

      Container& GetContainer()
      {
        return container;
      }

      const Container& GetContainer() const
      {
        return container;
      }
    };

    /**
      TableModel implementation that wraps an instancance of the given container of type
      Container holding elements of type Element. Elements are of type pointer.
      Deleting an element using Delete() or Clear() will call the delete operator on
      this element.
     */
    template<typename Element, typename Container = std::list<Element> >
    class StdPtrTable : public StdTableBaseSorted<Element,Container>
    {

    private:
      mutable typename StdTableBase<Element, Container>::ContainerType container;

    public:
      StdPtrTable(typename StdTableBase<Element, Container>::Comparator* comparator=NULL)
       : StdTableBaseSorted<Element,Container>(comparator)
      {
        // no code
      }

      Container& GetContainer()
      {
        return container;
      }

      const Container& GetContainer() const
      {
        return container;
      }

      Element Delete(size_t row)
      {
        Element value=StdTableBase<Element,Container>::Delete(row);

        delete value;

        return value;
      }

      void Clear()
      {
        for (size_t i=1; i<=this->GetRows(); i++) {
          delete this->GetEntry(i);
        }

        StdTableBase<Element,Container>::Clear();
      }
    };

    /**
      TableModel implementation that wraps an reference to an instancance the given
      container of type Container holding elements of type Element.

      The reference to the container instance is passed in the constructor.
     */
    template<typename Element, typename Container = std::list<Element> >
    class StdRefTable : public StdTableBaseSorted<Element,Container>
    {

    private:
      typename StdTableBase<Element, Container>::ContainerType& container;

    public:
      StdRefTable(typename StdTableBase<Element, Container>::ContainerType& container,
                  typename StdTableBase<Element, Container>::Comparator* comparator=NULL)
       : StdTableBaseSorted<Element,Container>(comparator),
         container(container)
      {
        // no code
      }

      Container& GetContainer()
      {
        return container;
      }

      const Container& GetContainer() const
      {
        return container;
      }
    };

    /**
      Predefined table model implementation based on StdTable for a list of strings.
     */
    class LUMAPI StringTable : public StdTable<std::wstring, std::list<std::wstring> >
    {
    private:
      class StringComparator : public Comparator
      {
      public:
        bool operator()(const std::wstring& a,
                        const std::wstring& b,
                        size_t /*column*/,
                        bool ascending) const
        {
          if (ascending) {
            return a<b;
          }
          else {
            return a>b;
          }
        }
      };

    public:
      StringTable()
      : StdTable<std::wstring, std::list<std::wstring> >(new StringComparator())
      {
        // no code
      }
    };

    typedef Base::Reference<StringTable> StringTableRef;

    /**
      Predefined table model implementation based on StdRefTable for a list of strings.
     */
    class LUMAPI StringRefTable : public StdRefTable<std::wstring,std::list<std::wstring> >
    {
    private:
      class StringComparator : public Comparator
      {
      public:
        bool operator()(const std::wstring& a,
                        const std::wstring& b,
                        size_t /*column*/,
                        bool ascending) const
        {
          if (ascending) {
            return a<b;
          }
          else {
            return a>b;
          }
        }
      };

    public:
      StringRefTable(std::list<std::wstring>& container)
      : StdRefTable<std::wstring, std::list<std::wstring> >(container,new StringComparator())
      {
        // no code
      }
    };

    typedef Base::Reference<StringRefTable> StringRefTableRef;

    /**
      Predefined table model implementation based on StdTable for a list of arrays of strings
      (two dimensional dynamic array of strings).
     */
    class LUMAPI StringMatrix : public StdTable<std::vector<std::wstring>, std::list<std::vector<std::wstring> > >
    {
    private:
      class StringComparator : public Comparator
      {
      public:
        bool operator()(const std::vector<std::wstring>& a,
                        const std::vector<std::wstring>& b,
                        size_t column,
                        bool ascending) const
        {
          if (ascending) {
            return a[column-1]<b[column-1];
          }
          else {
            return a[column-1]>b[column-1];
          }
        }
      };

    public:
      StringMatrix()
      : StdTable<std::vector<std::wstring>, std::list<std::vector<std::wstring> > >(NULL)
      {
        // no code
      }
    };

    typedef Base::Reference<StringMatrix> StringMatrixRef;
  }
}

#endif
