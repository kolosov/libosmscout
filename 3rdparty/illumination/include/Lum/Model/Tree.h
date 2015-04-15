#ifndef LUM_MODEL_TREE_H
#define LUM_MODEL_TREE_H

/*
  This source is part of the Illumination library
  Copyright (C) 2005  Tim Teulings

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

#include <Lum/Base/Object.h>

#include <Lum/Object.h>

namespace Lum {
  namespace Model {

    /**
      Model representing tree like data structures.
    */
    class LUMAPI Tree : public Base::Model
    {
    public:
      /**
        An item has been collapsed. A possible viewer might change the current focus
        item, since the current focus item might gone invisible.
      */
      class LUMAPI ItemCollapsedMsg : public Base::ResyncMsg
      {
      };

      /**
        An item has been expanded.
      */
      class LUMAPI ItemExpandedMsg : public Base::ResyncMsg
      {
      };

      /**
        Abstract base class representing one item in a tree. Item visualisation will be done
        using an object (GetObject()!=NULL) or text (GetText()) in this order. Note that
        the abstract base class just defines the common interface for items in a tree, it does
        no hold real data nor does it have setter to set an object or a text string.

        You must overload it, add data storage and setter methods and implement getter method
        GetObject() or GetText().
      */
      class LUMAPI Item
      {
        friend class Tree;

      private:
        Tree         *model;        //! Backpointer to model
        Item         *parent;       //! Parent node or NULL
        Item         *next;         //! Pointer to next object in the same subtree
        Item         *previous;     //! Pointer to previous object in the same subtree
        Item         *first;        //! First child
        Item         *last;         //! Last child
        size_t       visible;       //! Number of visible children
        size_t       childrenCount; //! Number of children including sub children...
        bool         expanded;      //! Children are currently visible

      private:
        void UpdateChildrenCount(size_t count);
        void RecalcVisible();
        void SetParent(Item *item);
        void SetModel(Tree *model);

      protected:
        Item();

      public:
        virtual ~Item();

        bool HasChildren() const;
        bool IsExpanded() const;
        bool IsVisible() const;
        virtual Lum::Object* GetObject() const;
        virtual std::wstring GetText() const;
        Item* GetVisible() const;
        Item* GetFirstChild() const;
        Item* GetLastChild() const;
        Item* GetParent() const;
        Item* GetPrevious() const;
        Item* GetNext() const;
        size_t GetPos() const;
        void GetPosAndOffset(size_t& pos, size_t& offset) const;

        void Append(Item *item);
        void Prepend(Item *item);
        void Remove(Item *item);

        void Expand();
        void Collaps();
      };

      /**
        Concrete sub class of Item holding a simple string value
      */
      class LUMAPI TextItem : public Item
      {
      private:
        std::wstring text; //! Text assigned to item

      public:
        TextItem(const std::wstring& text=L"");

        void SetText(const std::wstring& text);
        std::wstring GetText() const;
      };

      /**
        Concrete sub class of Item holding a simple object value
      */
      class LUMAPI ObjectItem : public Item
      {
      private:
        Lum::Object *object; //! Object assigned to item

      public:
        ObjectItem(Lum::Object* object=NULL);

        void SetObject(Lum::Object* object);
        Lum::Object* GetObject() const;
      };

    private:
      Item *top;    //! Top element of tree

    private:
      void RecalcVisible();

    public:
      Tree();
      virtual ~Tree();

      void SetTop(Item* item);

      size_t GetVisible() const;
      Item* GetTop() const;
      bool GetVisibleItem(size_t pos, Item*& item, size_t& indent) const;
    };

    typedef Base::Reference<Tree> TreeRef;
  }
}

#endif

