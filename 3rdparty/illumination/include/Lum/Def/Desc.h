#ifndef LUM_DEF_DESCRIPTION_H
#define LUM_DEF_DESCRIPTION_H

/*
  This source is part of the Illumination library
  Copyright (C) 2009  Tim Teulings

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

#include <string>

#include <Lum/Private/ImportExport.h>

//#include <Lum/OS/Theme.h>

namespace Lum {

  namespace Def {
    /**
     * Definition of a description. Description values are for example used for actions,
     * choice definitions used for single select tables, comboboxes and similar.
     */
    class LUMAPI Desc
    {
    private:
      bool                  hasLabel;
      std::wstring          label;

      //bool                  hasStockImage;
      //OS::Theme::StockImage stockImage;

      bool                  hasFileImage;
      std::wstring          fileImageBaseName;
      std::wstring          fileImageSuffix;

      bool                  hasShortcut;
      unsigned long         shortcutQualifier;
      std::wstring          shortcutKey;

    public:
      Desc();
      Desc(const std::wstring& label);

      Desc& SetLabel(const std::wstring& label);
      bool HasLabel() const;
      std::wstring GetLabel() const;

      /*
      Desc& SetStockImage(const OS::Theme::StockImage& stockImage);
      bool HasStockImage() const;
      OS::Theme::StockImage GetStockImage() const;*/

      Desc& SetFileImage(const std::wstring& baseName, const std::wstring& suffix);
      bool HasFileImage() const;
      std::wstring GetFileImageBaseName() const;
      std::wstring GetFileImageSuffix() const;

      Desc& SetShortcut(unsigned long qualifier, const std::wstring& key);
      bool HasShortcut() const;
      unsigned long GetShortcutQualifier() const;
      std::wstring GetShortcutKey() const;
    };
  }
}

#endif

