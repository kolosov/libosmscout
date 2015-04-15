#ifndef LUM_EDIT_FUNCTION_H
#define LUM_EDIT_FUNCTION_H

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

#include <Lum/Base/Singleton.h>

#include <Lum/Edit/Edit.h>

namespace Lum {
  namespace Edit {

    /*
     Navigation
    */

    extern LUMAPI void CursorLeft(Edit& e);
    extern LUMAPI void CursorRight(Edit& e);
    extern LUMAPI void CursorUp(Edit& e);
    extern LUMAPI void CursorDown(Edit& e);

    extern LUMAPI void CursorLeftSelection(Edit& e);
    extern LUMAPI void CursorRightSelection(Edit& e);
    extern LUMAPI void CursorUpSelection(Edit& e);
    extern LUMAPI void CursorDownSelection(Edit& e);

    extern LUMAPI void CursorPageUp(Edit& e);
    extern LUMAPI void CursorPageDown(Edit& e);

    extern LUMAPI void CursorPageUpSelection(Edit& e);
    extern LUMAPI void CursorPageDownSelection(Edit& e);

    extern LUMAPI void CursorGotoStart(Edit& e);
    extern LUMAPI void CursorGotoEnd(Edit& e);

    extern LUMAPI void CursorGotoSOL(Edit& e);
    extern LUMAPI void CursorGotoEOL(Edit& e);

    extern LUMAPI void CursorGotoSOLSelection(Edit& e);
    extern LUMAPI void CursorGotoEOLSelection(Edit& e);

    extern LUMAPI void CursorGotoFirstVisibleRow(Edit& e);
    extern LUMAPI void CursorGotoLastVisibleRow(Edit& e);

    extern LUMAPI void DisplayRefresh(Edit& e);

    extern LUMAPI void DisplayScrollUp(Edit& e);
    extern LUMAPI void DisplayScrollDown(Edit& e);
    extern LUMAPI void DisplayScrollLeft(Edit& e);
    extern LUMAPI void DisplayScrollRight(Edit& e);

    /*
      Basic editing
    */

    extern LUMAPI void CursorDelete(Edit& e);
    extern LUMAPI void CursorBackspace(Edit& e);
    extern LUMAPI void CursorDeleteLine(Edit& e);
    extern LUMAPI void CursorSplitLine(Edit& e);
    extern LUMAPI void CursorInsertBuffer(Edit& e);

    extern LUMAPI void TextSelectAll(Edit& e);
    extern LUMAPI void SelectionCopy(Edit& e);
    extern LUMAPI void SelectionDelete(Edit& e);
    extern LUMAPI void SelectionCut(Edit& e);
    extern LUMAPI void SelectionClear(Edit& e);
    extern LUMAPI void SelectionIndent(Edit& e);
    extern LUMAPI void SelectionOutdent(Edit& e);

    // Undo & Redo
    extern LUMAPI void TextUndo(Edit& e);
    extern LUMAPI void TextRedo(Edit& e);

    extern LUMAPI void TextClear(Edit& e);

    class LUMAPI FunctionRepository
    {
    private:
      std::map<std::wstring,Function> map;

    public:
      FunctionRepository();

      void Register(Function function, const std::wstring& name);
      Function Get(const std::wstring& name) const;
    };

    extern LUMAPI ::Lum::Base::Singleton<FunctionRepository> functionRepository;
  }
}

#endif
