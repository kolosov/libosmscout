#include <Lum/ButtonRow.h>

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

#include <Lum/Base/L10N.h>

#include <Lum/OS/Theme.h>

#include <Lum/Button.h>

namespace Lum {

  ButtonRow::ButtonRow()
  : buttonWidth(0),
    spaces(0)
  {
    // no code
  }

  ButtonRow::~ButtonRow()
  {
    std::list<Object*>::iterator iter;

    iter=command.begin();
    while (iter!=command.end()) {
      delete *iter;

      ++iter;
    }

    iter=optional.begin();
    while (iter!=optional.end()) {
      delete *iter;

      ++iter;
    }
  }

  bool ButtonRow::VisitChildren(Visitor &visitor, bool /*onlyVisible*/)
  {
    std::list<Object*>::iterator iter;

    iter=command.begin();
    while (iter!=command.end()) {
      if (!visitor.Visit(*iter)) {
        return false;
      }

      ++iter;
    }

    iter=optional.begin();
    while (iter!=optional.end()) {
      if (!visitor.Visit(*iter)) {
        return false;
      }

      ++iter;
    }

    return true;
  }

  /**
    Add an object to the button row.
  */
  ButtonRow* ButtonRow::Add(Object* object)
  {
    assert(object!=NULL);

    object->SetParent(this);
    command.push_back(object);

    return this;
  }

  /**
    Add an optional object to the button row. These objects will we aligne dot the
  other side of the button row to create a visible gap between the two object groups.
  */
  ButtonRow* ButtonRow::AddOptional(Object* object)
  {
    assert(object!=NULL);

    object->SetParent(this);
    optional.push_back(object);

    return this;
  }

  void ButtonRow::CalcSize()
  {
    std::list<Object*>::const_iterator iter;

    width=0;
    height=0;

    buttonWidth=OS::display->GetTheme()->GetMinimalButtonWidth();

    // Calculate bounds of command buttons
    iter=command.begin();
    while (iter!=command.end()) {
      (*iter)->CalcSize();

      height=std::max(height,(*iter)->GetOHeight());
      buttonWidth=std::max(buttonWidth,(*iter)->GetOWidth());

      ++iter;
    }

    // Calculate bounds of optional buttons
    iter=optional.begin();
    while (iter!=optional.end()) {
      (*iter)->CalcSize();

      height=std::max(height,(*iter)->GetOHeight());
      buttonWidth=std::max(buttonWidth,(*iter)->GetOWidth());

      ++iter;
    }

    // Resize command buttons
    iter=command.begin();
    while (iter!=command.end()) {
      (*iter)->Resize(buttonWidth,height);

      ++iter;
    }

    // Resize optional buttons
    iter=optional.begin();
    while (iter!=optional.end()) {
      (*iter)->Resize(buttonWidth,height);

      ++iter;
    }

    width=command.size()*buttonWidth+optional.size()*buttonWidth;

    if (command.size()>0 && optional.size()>0) {
      width+=OS::display->GetSpaceHorizontal(OS::Display::spaceInterObject);
    }

    if (command.size()>0) {
      width+=(command.size()-1)*OS::display->GetSpaceHorizontal(OS::Display::spaceInterObject);
    }
    if (optional.size()>0) {
      width+=(optional.size()-1)*OS::display->GetSpaceHorizontal(OS::Display::spaceInterObject);
    }

    minWidth=width;
    minHeight=height;

    Group::CalcSize();
  }

  void ButtonRow::Layout()
  {
    int pos;

    // If we have small displays we should make the button row fill its
    // size completely by resizing the buttons beyond their minimal sizes
    if (OS::display->GetSize()<=OS::Display::sizeSmall) {
      // TODO
    }

    pos=x;
    for (std::list<Object*>::const_iterator opt=optional.begin();
         opt!=optional.end();
         ++opt) {
      (*opt)->Move(pos,y+(height-(*opt)->GetOHeight())/2);

      pos+=(*opt)->GetOWidth()+OS::display->GetSpaceHorizontal(OS::Display::spaceInterObject);
    }

    pos=x+width-buttonWidth;
    // We should use const_reverse_iterator, but there is a bug in gcc <=4.0
    for (std::list<Object*>::reverse_iterator cmd=command.rbegin();
         cmd!=command.rend();
         ++cmd) {
      (*cmd)->Move(pos,y + (height-(*cmd)->GetOHeight()) / 2);
      pos-=(*cmd)->GetOWidth()+OS::display->GetSpaceHorizontal(OS::Display::spaceInterObject);
    }

    Group::Layout();
  }

  ButtonRow* ButtonRow::Create(bool horizontalFlex, bool verticalFlex)
  {
    ButtonRow *row;

    row=new ButtonRow();
    row->SetFlex(horizontalFlex,verticalFlex);

    return row;
  }

  ButtonRow* ButtonRow::CreateOk(Model::Action* ok,
                                 bool horizontalFlex, bool verticalFlex)
  {
    assert(ok!=NULL);

    ButtonRow *row;
    Button    *button;

    row=new ButtonRow();
    row->SetFlex(horizontalFlex,verticalFlex);

    button=Button::CreateOk(ok);
    button->SetFlex(true,true);
    row->Add(button);

    return row;
  }

  ButtonRow* ButtonRow::CreateCancel(Model::Action* cancel,
                                 bool horizontalFlex, bool verticalFlex)
  {
    assert(cancel!=NULL);

    ButtonRow *row;
    Button    *button;

    row=new ButtonRow();
    row->SetFlex(horizontalFlex,verticalFlex);

    button=Button::CreateCancel(cancel);
    button->SetFlex(true,true);
    row->Add(button);

    return row;
  }


  ButtonRow* ButtonRow::CreateOkCancel(Model::Action* ok,
                                       Model::Action* cancel,
                                 bool horizontalFlex, bool verticalFlex)
  {
    assert(ok!=NULL && cancel!=NULL);

    ButtonRow *row;
    Button    *button;

    row=new ButtonRow();
    row->SetFlex(horizontalFlex,verticalFlex);

    button=Button::CreateOk(ok);
    button->SetFlex(true,true);
    row->Add(button);

    button=Button::CreateCancel(cancel);
    button->SetFlex(true,true);
    row->Add(button);

    return row;
  }

  ButtonRow* ButtonRow::CreateClose(Model::Action* close,
                                 bool horizontalFlex, bool verticalFlex)
  {
    assert(close!=NULL);

    ButtonRow *row;
    Button    *button;

    row=new ButtonRow();
    row->SetFlex(horizontalFlex,verticalFlex);

    button=Button::CreateClose(close);
    button->SetFlex(true,true);
    row->Add(button);

    return row;
  }

  ButtonRow* ButtonRow::CreateQuit(Model::Action* quit,
                                   bool horizontalFlex, bool verticalFlex)
  {
    assert(quit!=NULL);

    ButtonRow *row;
    Button    *button;

    row=new ButtonRow();
    row->SetFlex(horizontalFlex,verticalFlex);

    button=Button::CreateQuit(quit);
    button->SetFlex(true,true);
    row->Add(button);

    return row;
  }

}
