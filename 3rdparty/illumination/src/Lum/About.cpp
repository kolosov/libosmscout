/*
  This source is part of the Illumination library
  Copyright (C) 2007  Tim Teulings

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

#include <Lum/About.h>

#include <Lum/Base/L10N.h>

#include <Lum/Label.h>
#include <Lum/ScrolledObject.h>
#include <Lum/Panel.h>
#include <Lum/Space.h>
#include <Lum/Tab.h>
#include <Lum/Text.h>
#include <Lum/View.h>

namespace Lum {

  About::About()
   : illuminationAction(new Model::Action())
  {
    Observe(illuminationAction);
  }

  About::~About()
  {
    // no code
  }

  void About::SetAppInfo(const Def::AppInfo& info)
  {
    this->info=info;
  }

  void About::CalcSize()
  {
    Panel *vPanel;
    Text  *text;
    View  *view;

    vPanel=VPanel::Create(true,false);

    text=new Text(info.GetProgram()+L" "+info.GetVersion(),
                  OS::Font::normal,
                  Text::centered,
                  OS::display->GetFont(OS::Display::fontScaleCaption1));
    text->SetFlex(true,false);
    vPanel->Add(text);

    vPanel->AddSpace();

    text=new Text(info.GetDescription(),
                  OS::Font::italic|OS::Font::bold,Text::centered);
    text->SetFlex(true,false);
    vPanel->Add(text);

    vPanel->AddSpace();

    vPanel->Add(Label::Create(true,false)
                ->AddLabel(_l(L"ABOUT_AUTHOR",L"Author:"),new Text(info.GetAuthor()))
                ->AddLabel(_l(L"ABOUT_CONTACT",L"Contact:"),new Text(info.GetContact()))
                ->AddLabel(_l(L"ABOUT_COPYRIGHT",L"Copyright:"),new Text(info.GetCopyright()))
                ->AddLabel(_l(L"ABOUT_LICENCE",L"License:"),new Text(info.GetLicense())));

    vPanel->AddSpace(Space::sizeHuge);

    text=new Text();
    text->SetFont(OS::display->GetFont(OS::Display::fontScaleFootnote));
    text->SetAlignment(Text::centered);
    text->AddText(_l(L"ABOUT_SLOGAN",
                     L"This program was written using Illumination,\n"
                     L"the C++ GUI framework.\n"
                     L"Visit http://illumination.sourceforge.net\n"
                     L"for more information!"));
    vPanel->Add(text);

    view=View::Create(ScrolledObject::Create(vPanel,true,true)
                      ->SetScroll(true,true),
                      true,true);

    container=view;

    Component::CalcSize();
  }
}

