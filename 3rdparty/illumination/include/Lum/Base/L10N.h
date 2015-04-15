#ifndef LUM_BASE_L10N_H
#define LUM_BASE_L10N_H

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

#include <string>

#include <Lum/Private/ImportExport.h>

namespace Lum {
  namespace Base {

    class L10N
    {
    public:
      enum Default {
        menuProject,
        menuProjectQuit,

        menuEdit,
        menuEditSettings,

        menuHelp,
        menuHelpHelp,
        menuHelpAbout,

        dlgButtonAbout,
        dlgButtonCancel,
        dlgButtonHelp,
        dlgButtonOk,
        dlgButtonClose,
        dlgButtonQuit
      };

    private:
      static std::wstring locale;
      static std::wstring fallbackLocale;
      static std::wstring appName;

    public:
      static LUMAPI void SetLocale(const std::wstring& locale);
      static LUMAPI void SetFallbackLocale(const std::wstring& locale);

      static LUMAPI std::wstring GetLocalizedText(const std::wstring& key,
                                                  const std::wstring& defaultValue);
      static LUMAPI std::wstring GetLocalizedLibText(const std::wstring& key,
                                                     const std::wstring& defaultValue);
      static LUMAPI std::wstring GetLocalizedDefaultText(Default id);

      static LUMAPI void Substitute(std::wstring& text,
                                    const std::wstring& pattern,
                                    const std::wstring& value);
    };
  }
}

#define _(key,defaultValue) Lum::Base::L10N::GetLocalizedText(key,defaultValue)
#define _l(key,defaultValue) Lum::Base::L10N::GetLocalizedLibText(key,defaultValue)
#define _ld(id) Lum::Base::L10N::GetLocalizedDefaultText(Lum::Base::L10N::id)

#endif

