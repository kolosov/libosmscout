/*
  This source is part of the Illumination library
  Copyright (C) 2009 Tim Teulings

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

#include <Lum/Def/Desc.h>

#include <cassert>

namespace Lum {
  namespace Def {
    Desc::Desc()
     : hasLabel(false),
       //hasStockImage(false),
       hasFileImage(false),
       hasShortcut(false)
    {
      // no code
    }

    Desc::Desc(const std::wstring& label)
     : hasLabel(true),
       label(label),
       //hasStockImage(false),
       hasFileImage(false),
       hasShortcut(false)
    {
      assert(label.length()>0);
    }

    Desc& Desc::SetLabel(const std::wstring& label)
    {
      assert(label.length()>0);

      this->label=label;
      hasLabel=true;

      return *this;
    }

    bool Desc::HasLabel() const
    {
      return hasLabel;
    }

    std::wstring Desc::GetLabel() const
    {
      assert(hasLabel);

      return label;
    }
    /*
    Desc& Desc::SetStockImage(const OS::Theme::StockImage& stockImage)
    {
      this->stockImage=stockImage;
      hasStockImage=true;

      return *this;
    }

    bool Desc::HasStockImage() const
    {
      return hasStockImage;
    }

    OS::Theme::StockImage Desc::GetStockImage() const
    {
      assert(hasStockImage);

      return stockImage;
    }*/

    Desc& Desc::SetFileImage(const std::wstring& baseName, const std::wstring& suffix)
    {
      assert(baseName.length()>0);
      assert(suffix.length()>0);

      this->fileImageBaseName=baseName;
      this->fileImageSuffix=suffix;
      hasFileImage=true;

      return *this;
    }

    bool Desc::HasFileImage() const
    {
      return hasFileImage;
    }

    std::wstring Desc::GetFileImageBaseName() const
    {
      assert(hasFileImage);

      return fileImageBaseName;
    }

    std::wstring Desc::GetFileImageSuffix() const
    {
      assert(hasFileImage);

      return fileImageSuffix;
    }

    Desc& Desc::SetShortcut(unsigned long qualifier, const std::wstring& key)
    {
      assert(key.length()>0);

      this->shortcutQualifier=qualifier;
      this->shortcutKey=key;
      hasShortcut=true;

      return *this;
    }

    bool Desc::HasShortcut() const
    {
      return hasShortcut;
    }

    unsigned long Desc::GetShortcutQualifier() const
    {
      assert(hasShortcut);

      return shortcutQualifier;
    }

    std::wstring Desc::GetShortcutKey() const
    {
      assert(hasShortcut);

      return shortcutKey;
    }
  }
}

