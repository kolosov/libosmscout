#ifndef LUM_WIZRAD_H
#define LUM_WIZARD_H

/*
  This source is part of the Illumination library
  Copyright (C) 2008  Tim Teulings

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

#include <vector>

#include <Lum/Model/Number.h>

#include <Lum/Object.h>

namespace Lum {

  /**
    Layout group to blend in dialog like mini windows above and below the
    main object by temporary shrinking the main object.
  */
  class LUMAPI Wizard : public Component
  {
  private:
    class Page
    {
    public:
      std::wstring          title;
      Lum::Object*          content;
      Lum::Model::ActionRef continueAction;
      Lum::Model::ActionRef helpAction;
    };

  private:
    std::vector<Page>     pages;

    Lum::Model::ActionRef backAction;
    Lum::Model::ActionRef cancelAction;
    Lum::Model::NumberRef page;

  public:
    Wizard();
    virtual ~Wizard();

    void SetCancelAction(Lum::Model::Action* cancelAction);

    void AddPage(const std::wstring& title,
                 Lum::Object* content,
                 Lum::Model::Action* continueAction=NULL,
                 Lum::Model::Action* helpAction=NULL);

    void CalcSize();

    void Resync(Base::Model* model, const Base::ResyncMsg& msg);
  };
}

#endif

