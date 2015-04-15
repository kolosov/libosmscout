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

#include <Lum/Wizard.h>

#include <Lum/Base/L10N.h>

#include <Lum/Button.h>
#include <Lum/ButtonRow.h>
#include <Lum/Multi.h>
#include <Lum/Panel.h>
#include <Lum/Text.h>

#include <iostream>
namespace Lum {

  Wizard::Wizard()
  : backAction(new Lum::Model::Action()),
    page(new Lum::Model::Number(0ul))
  {
    Observe(backAction);
  }

  Wizard::~Wizard()
  {
    // no code
  }

  void Wizard::SetCancelAction(Lum::Model::Action* cancelAction)
  {
    this->cancelAction=cancelAction;
  }

  void Wizard::AddPage(const std::wstring& title,
                       Lum::Object* content,
                       Lum::Model::Action* continueAction,
                       Lum::Model::Action* helpAction)
  {
    assert(!inited);
    assert(content!=NULL);

    Page page;

    page.title=title;
    page.content=content;
    page.continueAction=continueAction!=NULL ? continueAction : new Lum::Model::Action();
    page.helpAction=helpAction;

    Observe(page.continueAction);

    pages.push_back(page);
  }

  void Wizard::CalcSize()
  {
    assert(pages.size()>0);

    Multi *multi;

    multi=new Multi();
    multi->SetFlex(true,true);
    multi->SetModel(page);

    for (size_t i=0; i<pages.size(); i++) {
      ButtonRow *row;
      Panel     *panel;
      Text      *title;

      panel=Lum::VPanel::Create(true,true);

      title=new Text(pages[i].title,
                     OS::Font::bold,
                     Text::left,
                     OS::display->GetFont(OS::Display::fontScaleCaption2));
      title->SetFlex(true,false);
      panel->Add(title);
      panel->AddSpace();
      panel->Add(pages[i].content);

      if (OS::display->GetTheme()->GetDialogActionPosition()==OS::Theme::actionPosHorizButtonRowBelow) {
        panel->AddSpace();

        row=ButtonRow::Create(true,false);

        if (pages[i].helpAction.Valid()) {
          row->AddOptional(Button::CreateHelp(pages[i].helpAction,true,true));
        }

        if (i>0) {
          row->Add(Button::Create(L"_Previous",backAction,true,true));
        }

        if (i==pages.size()-1) {
          row->Add(Button::Create(L"_Finish",pages[i].continueAction,true,true));
        }
        else {
          row->Add(Button::Create(L"_Next",pages[i].continueAction,true,true));
        }

        if (cancelAction.Valid()) {
          row->Add(Button::CreateCancel(cancelAction,true,true));
        }

        panel->Add(row);

        multi->Append(panel);
      }
      else if (OS::display->GetTheme()->GetDialogActionPosition()==OS::Theme::actionPosVertButtonRowRight) {
        HPanel *main;
        VPanel *buttons;

        main=HPanel::Create(true,true);
        main->Add(panel);
        main->AddSpace();

        buttons=VPanel::Create(false,true);
        buttons->AddSpace(true);

        if (pages[i].helpAction.Valid()) {
          buttons->Add(Button::CreateHelp(pages[i].helpAction,true,false));
        }

        if (i>0) {
          buttons->Add(Button::Create(L"_Previous",backAction,true,false));
        }

        if (i==pages.size()-1) {
          buttons->Add(Button::Create(L"_Finish",pages[i].continueAction,true,false));
        }
        else {
          buttons->Add(Button::Create(L"_Next",pages[i].continueAction,true,false));
        }

        if (cancelAction.Valid()) {
          buttons->Add(Button::CreateCancel(cancelAction,true,false));
        }

        main->Add(buttons);

        multi->Append(main);
      }
      else {
        assert(false);
      }
    }

    container=multi;

    Component::CalcSize();
  }

  void Wizard::Resync(Base::Model* model, const Base::ResyncMsg& msg)
  {
    if (model==backAction &&
        backAction->IsFinished()) {
      std::cout << "Dec page from " << page->GetUnsignedLong() << std::endl;
      page->Dec();
      std::cout << "Page now: " << page->GetUnsignedLong() << std::endl;
    }
    else if (pages.size()>0) {
      for (size_t i=0; i<pages.size()-1; i++) {
        if (model==pages[i].continueAction &&
            pages[i].continueAction->IsFinished()) {
          std::cout << "Inc page from " << page->GetUnsignedLong() << std::endl;
          page->Inc();
          if (page->IsNull()) {
            std::cout << "Page now: <null>" << std::endl;
          }
          else {
            std::cout << "Page now: " << page->GetUnsignedLong() << std::endl;
          }
        }
      }
    }

    Component::Resync(model,msg);
  }
}

