#ifndef LUM_BLENDER_H
#define LUM_BLENDER_H

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

#include <map>

#include <Lum/Base/Size.h>

#include <Lum/Model/Action.h>

#include <Lum/Object.h>

namespace Lum {

  /**
    Layout group to blend in dialog like mini windows above and below the
    main object by temporary shrinking the main object.
  */
  class LUMAPI Blender : public Group
  {
  private:
    std::map<std::wstring,Object*> topMap;
    std::map<std::wstring,Object*> bottomMap;
    Object                         *top;
    Object                         *main;
    Object                         *bottom;
    Model::ActionRef               closeTopAction;
    Model::ActionRef               closeBottomAction;

  public:
    Blender();
    virtual ~Blender();

    void AddTop(const std::wstring& name, Object* object);
    bool HasTop(const std::wstring& name) const;
    void SetTop(const std::wstring& name);
    void HideTop();

    void SetMain(Object* object);

    void AddBottom(const std::wstring& name, Object* object);
    bool HasBottom(const std::wstring& name) const;
    void SetBottom(const std::wstring& name);
    void HideBottom();

    void HideBoth();

    Model::Action* GetCloseTopAction() const;
    Model::Action* GetCloseBottomAction() const;

    bool VisitChildren(Visitor &visitor, bool onlyVisible);

    void CalcSize();
    void Layout();

    void Resync(Base::Model* model, const Base::ResyncMsg& msg);
  };
}

#endif

