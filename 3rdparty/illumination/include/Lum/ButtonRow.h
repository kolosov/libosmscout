#ifndef LUM_BUTTONROW_H
#define LUM_BUTTONROW_H

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

#include <list>

#include <Lum/Base/Size.h>

#include <Lum/Object.h>

namespace Lum {
  /**
    A layout group enummerates a number of buttons horizontaly.
  */
  class LUMAPI ButtonRow : public Group
  {
  private:
    size_t             buttonWidth;
    size_t             spaces;
    std::list<Object*> optional;
    std::list<Object*> command;

  public:
    ButtonRow();
    ~ButtonRow();

    bool VisitChildren(Visitor &visitor, bool onlyVisible);

    ButtonRow* Add(Object* object);
    ButtonRow* AddOptional(Object* object);

    void CalcSize();
    void Layout();

    static ButtonRow* Create(bool horizontalFlex=false, bool verticalFlex=false);
    static ButtonRow* CreateOk(Model::Action* ok,
                               bool horizontalFlex=false, bool verticalFlex=false);
    static ButtonRow* CreateCancel(Model::Action* cancel,
                                   bool horizontalFlex=false, bool verticalFlex=false);
    static ButtonRow* CreateOkCancel(Model::Action* ok, Model::Action* cancel,
                                     bool horizontalFlex=false, bool verticalFlex=false);
    static ButtonRow* CreateClose(Model::Action* close,
                                  bool horizontalFlex=false, bool verticalFlex=false);
    static ButtonRow* CreateQuit(Model::Action* quit,
                                 bool horizontalFlex=false, bool verticalFlex=false);
  };
}

#endif
