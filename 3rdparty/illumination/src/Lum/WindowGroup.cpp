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

#include <Lum/WindowGroup.h>

#include <algorithm>

#include <Lum/OS/Theme.h>

namespace Lum {

  WindowGroup::WindowGroup()
  : men(NULL),
    main(NULL),
    tool(NULL),
    status(NULL),
    mainSeamless(false)
  {
    // no code
  }

  WindowGroup::~WindowGroup()
  {
    delete men;
    delete main;
    delete tool;
    delete status;
  }

  void WindowGroup::SetMenuStrip(MenuStrip* menuStrip)
  {
    delete this->men;

    this->men=menuStrip;
    if (men!=NULL) {
      men->SetParent(this);
    }

    if (visible) {
      if (this->men!=NULL) {
        this->men->CalcSize();
        this->men->SetWindow(GetWindow());    
        this->men->ResizeWidth(width);
      }

      Redraw();
    }
  }

  void WindowGroup::SetToolbar(Object* object)
  {
    delete this->tool;

    this->tool=object;
    if (tool!=NULL) {
      tool->SetParent(this);
    }

    if (visible) {
      Redraw();
    }
  }

  void WindowGroup::SetMain(Object* object, bool seamless)
  {
    delete this->main;

    this->main=object;
    if (main!=NULL) {
      main->SetParent(this);
      mainSeamless=seamless;
    }

    if (visible) {
      Redraw();
    }
  }

  void WindowGroup::SetStatusLine(Object* object)
  {
    delete this->status;

    this->status=object;
    if (status!=NULL) {
      status->SetParent(this);
    }

    if (visible) {
      Redraw();
    }
  }

  bool WindowGroup::VisitChildren(Visitor &visitor, bool /*onlyVisible*/)
  {
    if (men!=NULL &&
        !visitor.Visit(men)) {
      return false;
    }

    if (main!=NULL &&
        !visitor.Visit(main)) {
      return false;
    }

    if (tool!=NULL &&
        !visitor.Visit(tool)) {
      return false;
    }

    if (status!=NULL &&
        !visitor.Visit(status)) {
      return false;
    }

    return true;
  }

  void WindowGroup::CalcSize()
  {
    width=0;
    height=0;
    minWidth=0;
    minHeight=0;

    if (men!=NULL) {
      men->CalcSize();
    }

    if (tool!=NULL) {
      tool->CalcSize();
    }

    if (status!=NULL) {
      status->CalcSize();
    }

    if (main!=NULL) {
      main->CalcSize();
    }

    // width

    if (men!=NULL) {
      width=std::max(width,men->GetOWidth());
      minWidth=std::max(minWidth,men->GetOMinWidth());
    }

    if (tool!=NULL) {
      width=std::max(width,tool->GetOWidth()+2*OS::display->GetSpaceHorizontal(OS::Display::spaceWindowBorder));
      minWidth=std::max(minWidth,tool->GetOMinWidth()+2*OS::display->GetSpaceHorizontal(OS::Display::spaceWindowBorder));
    }

    if (main!=NULL) {
      if (mainSeamless) {
        width=std::max(width,main->GetOWidth());
        minWidth=std::max(minWidth,main->GetOMinWidth());
      }
      else {
        width=std::max(width,main->GetOWidth()+2*OS::display->GetSpaceHorizontal(OS::Display::spaceWindowBorder));
        minWidth=std::max(minWidth,main->GetOMinWidth()+2*OS::display->GetSpaceHorizontal(OS::Display::spaceWindowBorder));
      }
      SetFlex(main->HorizontalFlex(),main->VerticalFlex());
    }

    if (status!=NULL) {
      width=std::max(width,status->GetOWidth());
      minWidth=std::max(minWidth,status->GetOMinWidth());
    }

    // height

    if (men!=NULL) {
      height=men->GetOHeight();
      minHeight=men->GetOMinHeight();
    }

    if (tool!=NULL) {
      height+=tool->GetOHeight();
      minHeight+=tool->GetOMinHeight();
    }

    if (status!=NULL) {
      height+=status->GetOHeight();
      minHeight+=status->GetOMinHeight();
    }

    if (main!=NULL) {
      height+=main->GetOHeight();
      minHeight+=main->GetOMinHeight();

      if (!mainSeamless) {
        if (men==NULL && tool==NULL) {
          height+=OS::display->GetSpaceVertical(OS::Display::spaceWindowBorder);
          minHeight+=OS::display->GetSpaceVertical(OS::Display::spaceWindowBorder);
        }
        else {
          height+=OS::display->GetSpaceVertical(OS::Display::spaceInterObject);
          minHeight+=OS::display->GetSpaceVertical(OS::Display::spaceInterObject);
        }
      }

      if (status==NULL) {
        height+=OS::display->GetSpaceVertical(OS::Display::spaceWindowBorder);
        minHeight+=OS::display->GetSpaceVertical(OS::Display::spaceWindowBorder);
      }
      else {
        height+=OS::display->GetSpaceVertical(OS::Display::spaceInterObject);
        minHeight+=OS::display->GetSpaceVertical(OS::Display::spaceInterObject);
      }
    }

    Group::CalcSize();
  }

  void WindowGroup::Layout()
  {
    int    yPos=y;
    size_t height=this->height;

    if (men!=NULL) {
      men->ResizeWidth(width);
      men->Move(x+(width-men->GetOWidth())/2,yPos);

      yPos+=men->GetOHeight();
      height-=men->GetOHeight();
    }

    if (tool!=NULL) {
      tool->ResizeWidth(width);
      if (OS::display->GetTheme()->GetToolbarPosition()== OS::Theme::toolbarTop) {
        tool->Move(x+(width-tool->GetOWidth())/2,yPos);

        yPos+=tool->GetOHeight();
        height-=tool->GetOHeight();
      }
      else {
        tool->Move(x+(width-tool->GetOWidth())/2,this->height-tool->GetOHeight()-
                   (status!=NULL ? status->GetOHeight() : 0));
        height-=tool->GetOHeight();
      }
    }

    if (status!=NULL) {
      status->ResizeWidth(width);
      status->Move(x+(width-status->GetOWidth())/2,this->height-status->GetOHeight());
      height-=status->GetOHeight();
    }

    if (main!=NULL) {
      if (mainSeamless) {
        main->ResizeWidth(width);
      }
      else {
        main->ResizeWidth(width-2*OS::display->GetSpaceHorizontal(OS::Display::spaceWindowBorder));
      }

      if (!mainSeamless) {
        if (men==NULL && tool==NULL) {
          yPos+=OS::display->GetSpaceVertical(OS::Display::spaceWindowBorder);
          height-=OS::display->GetSpaceVertical(OS::Display::spaceWindowBorder);
        }
        else {
          yPos+=OS::display->GetSpaceVertical(OS::Display::spaceInterObject);
          height-=OS::display->GetSpaceVertical(OS::Display::spaceInterObject);
        }
      }

      if (status==NULL) {
        height-=OS::display->GetSpaceVertical(OS::Display::spaceWindowBorder);
      }
      else {
        height-=OS::display->GetSpaceVertical(OS::Display::spaceInterObject);
      }

      main->ResizeHeight(height);

      main->Move(x+(width-main->GetOWidth())/2,yPos+(height-main->GetOHeight())/2);
    }

    Group::Layout();
  }
}
