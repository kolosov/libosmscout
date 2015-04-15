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

#include <Lum/Def/MultiView.h>
#include <iostream>
namespace Lum {
  namespace Def {
    MultiView::View::View(size_t value,
             const Desc& description,
             Lum::Object* object)
     : value(value),
       description(description),
       menu(NULL),
       object(object)
    {
      assert(object!=NULL);
    }

    MultiView::View::View(const View& other)
    {
      if (this==&other) {
        return;
      }

      this->value=other.value;
      this->description=other.description;
      if (other.menu!=NULL) {
        this->menu=new Menu(*other.menu);
      }
      else {
        this->menu=NULL;
      }
      this->object=other.object;
    }

    MultiView::View::~View()
    {
      delete menu;
    }

    MultiView::View& MultiView::View::SetMenu(Menu* menu)
    {
      delete menu;

      this->menu=menu;

      return *this;
    }

    const Desc& MultiView::View::GetDesc() const
    {
      return description;
    }

    size_t MultiView::View::GetValue() const
    {
      return value;
    }

    Menu* MultiView::View::GetMenu() const
    {
      return menu;
    }

    Lum::Object* MultiView::View::GetObject() const
    {
      return object;
    }

    MultiView::MultiView(const Desc& description)
     : description(description),
       preMenu(NULL),
       postMenu(NULL)
    {
      // no code
    }

    MultiView::MultiView(const MultiView& other)
    {
      if (this==&other) {
        return;
      }

      this->description=other.description;
      if (other.preMenu!=NULL) {
        this->preMenu=new Menu(*other.preMenu);
      }
      else {
        this->preMenu=NULL;
      }
      if (other.postMenu!=NULL) {
        this->postMenu=new Menu(*other.postMenu);
      }
      else {
        this->postMenu=NULL;
      }
      this->value=other.value;
      this->views=other.views;
    }

    MultiView::~MultiView()
    {
      delete preMenu;
      delete postMenu;
    }

    MultiView& MultiView::SetMenu(Menu* pre, Menu* post)
    {
      delete preMenu;
      delete postMenu;

      this->preMenu=pre;
      this->postMenu=post;

      return *this;
    }

    MultiView& MultiView::SetValue(Model::Number *value)
    {
      this->value=value;

      return *this;
    }

    MultiView& MultiView::AddView(const View& view)
    {
      views.push_back(view);

      return *this;
    }

    Model::Number* MultiView::GetValue() const
    {
      return value;
    }

    const Desc& MultiView::GetDesc() const
    {
      return description;
    }

    Menu* MultiView::GetPreMenu() const
    {
      return preMenu;
    }

    Menu* MultiView::GetPostMenu() const
    {
      return postMenu;
    }

    const std::list<MultiView::View>& MultiView::GetViews() const
    {
      return views;
    }
  }
}

