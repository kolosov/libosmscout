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

#include <Lum/Model/Tree.h>

namespace Lum {
  namespace Model {

    Tree::Item::Item()
    : model(NULL),
      parent(NULL),
      next(NULL),previous(NULL),first(NULL),last(NULL),
      visible(0),childrenCount(0),
      expanded(true)
    {
    }

    Tree::Item::~Item()
    {
      Item *item;
      Item *next;

      item=first;
      while (item!=NULL) {
        next=item->next;
        delete item;

        item=next;
      }
    }

    void Tree::Item::UpdateChildrenCount(size_t count)
    {
      childrenCount+=count;
      if (parent!=NULL) {
        parent->UpdateChildrenCount(count);
      }
    }

    void Tree::Item::RecalcVisible()
    {
      visible=0;

      if (expanded) {
        Item *item;

        item=first;
        while (item!=NULL) {
          visible+=item->visible+1; // All visible children + entry itself

          item=item->next;
        }
      }

      if (parent!=NULL) {
        parent->RecalcVisible();
      }
    }

    void Tree::Item::SetParent(Item *item)
    {
      parent=item;
    }

    void Tree::Item::SetModel(Tree *model)
    {
      this->model=model;

      for (Item* item=first; item!=NULL; item=item->next) {
        item->SetModel(model);
      }
    }

    bool Tree::Item::HasChildren() const
    {
      return first!=NULL;
    }

    bool Tree::Item::IsExpanded() const
    {
      return expanded;
    }

    bool Tree::Item::IsVisible() const
    {
      return GetVisible()==this;
    }

    Object* Tree::Item::GetObject() const
    {
      return NULL;
    }

    std::wstring Tree::Item::GetText() const
    {
      return L"";
    }

    Tree::Item* Tree::Item::GetVisible() const
    {
      const Item *item;
      const Item *visible;

      item=this;
      visible=this;
      while (item->parent!=NULL) {
        if (item->parent->visible==0) {
          visible=item->parent;
        }
        item=item->parent;
      }

      return const_cast<Item*>(visible);
    }

    Tree::Item* Tree::Item::GetFirstChild() const
    {
      return first;
    }

    Tree::Item* Tree::Item::GetLastChild() const
    {
      return last;
    }

    Tree::Item* Tree::Item::GetParent() const
    {
      return parent;
    }

    Tree::Item* Tree::Item::GetPrevious() const
    {
      return previous;
    }

    Tree::Item* Tree::Item::GetNext() const
    {
      return next;
    }

    size_t Tree::Item::GetPos() const
    {
      size_t pos,offset;

      GetPosAndOffset(pos,offset);

      return pos;
    }

    void Tree::Item::GetPosAndOffset(size_t& pos, size_t& offset) const
    {
      const Item* item;

      pos=1;
      offset=0;
      item=this;
      while (item!=NULL) {
        // calculation of position in current sub tree
        while (item->previous!=NULL) {
          ++pos;
          item=item->previous;
          if (item->expanded) {
            pos+=item->visible;
          }
        }

        // step up one hierachie
        item=item->parent;
        if (item!=NULL) {
          ++offset;
          ++pos;
        }
      }
    }

    void Tree::Item::Append(Item *item)
    {
      item->SetModel(model);
      item->SetParent(this);
      item->next=NULL;
      item->previous=last;

      if (last==NULL) {
        first=item;
      }
      else {
        last->next=item;
      }
      last=item;

      UpdateChildrenCount(childrenCount+1);
      RecalcVisible();

      if (model!=NULL) {
        model->Notify(); // TODO: Better notify!?
      }
    }

    void Tree::Item::Prepend(Item *item)
    {
      item->SetModel(model);
      item->SetParent(this);
      item->previous=NULL;
      item->next=first;

      if (last!=NULL) {
        first->last=item;
      }
      else {
        last=item;
      }
      first=item;

      UpdateChildrenCount(childrenCount+1);
      RecalcVisible();

      if (model!=NULL) {
        model->Notify(); // TODO: Better notify!?
      }
    }

    void Tree::Item::Remove(Item *item)
    {
      if (item==first) {
        first=item->next;
        if (first!=NULL) {
          first->previous=NULL;
        }
      }
      else if (item==last) {
        if (item->previous!=NULL) {
          item->previous->next=NULL;
        }
        last=item->previous;
      }
      else {
        Item* tmp;

        tmp=first;
        while (tmp!=NULL && tmp->next!=item) {
          tmp=tmp->next;
        }

        assert(tmp!=NULL); // item is not a child of this node!

        tmp->next=item->next;
        if (tmp->next!=NULL) {
          tmp->previous=tmp;
        }
      }

      delete item;

      UpdateChildrenCount(childrenCount-1);
      RecalcVisible();

      if (model!=NULL) {
        model->Notify(); // TODO: Better notify!?
      }
    }

    void Tree::Item::Expand()
    {
      expanded=true;
      RecalcVisible();

      if (model!=NULL) {
        ItemExpandedMsg msg;

        model->Notify(msg);
      }
    }

    void Tree::Item::Collaps()
    {
      expanded=false;
      RecalcVisible();

      if (model!=NULL) {
        ItemCollapsedMsg msg;

        model->Notify(msg);
      }
    }

    Tree::TextItem::TextItem(const std::wstring& text)
    : text(text)
    {
      // no code
    }

    void Tree::TextItem::SetText(const std::wstring& text)
    {
      this->text=text;
    }

    std::wstring Tree::TextItem::GetText() const
    {
      return text;
    }

    Tree::ObjectItem::ObjectItem(Lum::Object* object)
    : object(object)
    {
      // no code
    }

    void Tree::ObjectItem::SetObject(Lum::Object* object)
    {
      this->object=object;
    }

    Lum::Object* Tree::ObjectItem::GetObject() const
    {
      return object;
    }

    Tree::Tree()
    : top(NULL)
    {
      // no code
    }

    Tree::~Tree()
    {
      delete top;
    }

    void Tree::SetTop(Item* item)
    {
      delete top;

      top=item;
      top->SetModel(this);
    }

    size_t Tree::GetVisible() const
    {
      if (top!=NULL) {
        return top->visible+1;
      }
      else {
        return 0;
      }
    }

    Tree::Item* Tree::GetTop() const
    {
      return top;
    }

    bool Tree::GetVisibleItem(size_t pos, Item*& item, size_t& indent) const
    {
      size_t help;

      item=top;
      indent=0;
      help=1;
      while (help!=pos && item!=NULL) {
        if (help+item->visible<pos) {
          help+=item->visible+1;
          item=item->next;
        }
        else {
          item=item->first;
          ++indent;
          ++help;
        }
      }

      return item!=NULL;
    }
  }
}


