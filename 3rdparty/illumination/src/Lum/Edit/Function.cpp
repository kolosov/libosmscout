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

#include <Lum/Edit/Function.h>

namespace Lum {
  namespace Edit {

    static FunctionRepository*                 singletonStore;
    ::Lum::Base::Singleton<FunctionRepository> functionRepository(&singletonStore);

    FunctionRepository::FunctionRepository()
    {
      Register(CursorLeft,L"CursorLeft");
      Register(CursorRight,L"CursorRight");
      Register(CursorUp,L"CursorUp");
      Register(CursorDown,L"CursorDown");
      Register(CursorLeftSelection,L"CursorLeftSelection");
      Register(CursorRightSelection,L"CursorRightSelection");
      Register(CursorUpSelection,L"CursorUpSelection");
      Register(CursorDownSelection,L"CursorDownSelection");
      Register(CursorPageUp,L"CursorPageUp");
      Register(CursorPageDown,L"CursorPageDown");
      Register(CursorPageUpSelection,L"CursorPageUpSelection");
      Register(CursorPageDownSelection,L"CursorPageDownSelection");
      Register(CursorGotoStart,L"CursorGotoStart");
      Register(CursorGotoEnd,L"CursorGotoEnd");
      Register(CursorGotoSOL,L"CursorGotoSOL");
      Register(CursorGotoEOL,L"CursorGotoEOL");
      Register(CursorGotoSOLSelection,L"CursorGotoSOLSelection");
      Register(CursorGotoEOLSelection,L"CursorGotoEOLSelection");
      Register(CursorGotoFirstVisibleRow,L"CursorGotoFirstVisibleRow");
      Register(CursorGotoLastVisibleRow,L"CursorGotoLastVisibleRow");
      Register(DisplayRefresh,L"DisplayRefresh");
      Register(DisplayScrollUp,L"DisplayScrollUp");
      Register(DisplayScrollDown,L"DisplayScrollDown");
      Register(DisplayScrollLeft,L"DisplayScrollLeft");
      Register(DisplayScrollRight,L"DisplayScrollRight");
      Register(CursorDelete,L"CursorDelete");
      Register(CursorBackspace,L"CursorBackspace");
      Register(CursorDeleteLine,L"CursorDeleteLine");
      Register(CursorSplitLine,L"CursorSplitLine");
      Register(CursorInsertBuffer,L"CursorInsertBuffer");
      Register(TextSelectAll,L"TextSelectAll");
      Register(SelectionCopy,L"SelectionCopy");
      Register(SelectionDelete,L"SelectionDelete");
      Register(SelectionCut,L"SelectionCut");
      Register(SelectionClear,L"SelectionClear");
      Register(SelectionIndent,L"SelectionIndent");
      Register(SelectionOutdent,L"SelectionOutdent");
      Register(TextUndo,L"TextUndo");
      Register(TextRedo,L"TextRedo");
      Register(TextClear,L"TextClear");
    }

    void FunctionRepository::Register(Function function, const std::wstring& name)
    {
      map[name]=function;
    }

    Function FunctionRepository::Get(const std::wstring& name) const
    {
      std::map<std::wstring,Function>::const_iterator iter;

      iter=map.find(name);

      if (iter!=map.end()) {
        return iter->second;
      }
      else {
        return NULL;
      }
    }

    void CursorLeft(Edit& e)
    {
      e.ClearSelection();
      e.cursor->Goto(e.cursor->GetX()-1,e.cursor->GetY());
    }

    void CursorLeftSelection(Edit& e)
    {
      if (e.IsUnselected()) {
        e.SetStartMark(e.cursor);
      }
      if (e.cursor->Goto(e.cursor->GetX()-1,e.cursor->GetY())) {
        e.SetEndMark(e.cursor);
      }
    }

    void CursorRight(Edit& e)
    {
      e.ClearSelection();
      e.cursor->Goto(e.cursor->GetX()+1,e.cursor->GetY());
    }

    void CursorRightSelection(Edit& e)
    {
      if (e.IsUnselected()) {
        e.SetStartMark(e.cursor);
      }
      if (e.cursor->Goto(e.cursor->GetX()+1,e.cursor->GetY())) {
        e.SetEndMark(e.cursor);
      }
    }

    void CursorUp(Edit& e)
    {
      e.ClearSelection();
      e.cursor->GotoRow(e.cursor->GetY()-1);
    }

    void CursorUpSelection(Edit& e)
    {
      if (e.IsUnselected()) {
        e.SetStartMark(e.cursor);
      }
      if (e.cursor->GotoRow(e.cursor->GetY()-1)) {
        e.SetEndMark(e.cursor);
      }
    }

    void CursorDown(Edit& e)
    {
      e.ClearSelection();
      e.cursor->GotoRow(e.cursor->GetY()+1);
    }

    void CursorDownSelection(Edit& e)
    {
      if (e.IsUnselected()) {
        e.SetStartMark(e.cursor);
      }
      if (e.cursor->GotoRow(e.cursor->GetY()+1)) {
        e.SetEndMark(e.cursor);
      }
    }

    void CursorPageUp(Edit& e)
    {
      e.ClearSelection();
      if (e.GetFirstVisiblePos()==1) {
        e.cursor->Goto(e.cursor->GetX(),1);
      }
      else {
        size_t y;

        // Remember cursor position if cursor is visible
        y=e.cursor->GetY();

        e.Page(-(e.vSize-1));

        // Reposition cursor
        if (y<e.vSize-1) {
          y=1;
        }
        else {
          y-=e.vSize-1;
        }

        if (y<e.GetFirstVisiblePos()) {
          e.cursor->Goto(e.cursor->GetX(),e.GetFirstVisiblePos());
        }
        else if (y>e.GetLastVisiblePos()) {
          e.cursor->Goto(e.cursor->GetX(),e.GetLastVisiblePos());
        }
        else {
          e.cursor->Goto(e.cursor->GetX(),y);
        }
      }
    }

    void CursorPageUpSelection(Edit& e)
    {
      if (e.IsUnselected()) {
        e.SetStartMark(e.cursor);
      }

      if (e.GetFirstVisiblePos()==1) {
        e.cursor->Goto(e.cursor->GetX(),1);
      }
      else {
        size_t y;

        // Remember cursor position if cursor is visible
        y=e.cursor->GetY();

        e.Page(-(e.vSize-1));

        // Reposition cursor
        if (y<e.vSize-1) {
          y=1;
        }
        else {
          y-=e.vSize-1;
        }

        if (y<e.GetFirstVisiblePos()) {
          e.cursor->Goto(e.cursor->GetX(),e.GetFirstVisiblePos());
        }
        else if (y>e.GetLastVisiblePos()) {
          e.cursor->Goto(e.cursor->GetX(),e.GetLastVisiblePos());
        }
        else {
          e.cursor->Goto(e.cursor->GetX(),y);
        }
      }

      e.SetEndMark(e.cursor);
    }

    void CursorPageDown(Edit& e)
    {
      e.ClearSelection();
      if (e.GetLastVisiblePos()==e.model->GetLines()) {
        e.cursor->Goto(e.cursor->GetX(),e.model->GetLines());
      }
      else {
        size_t y;

        // Remember cursor position if cursor is visible
        y=e.cursor->GetY();

        e.Page(e.vSize-1);

        // Reposition cursor
        y=std::min(y+e.vSize-1,e.model->GetLines());

        if (y<e.GetFirstVisiblePos()) {
          e.cursor->Goto(e.cursor->GetX(),e.GetFirstVisiblePos());
        }
        else if (y>e.GetLastVisiblePos()) {
          e.cursor->Goto(e.cursor->GetX(),e.GetLastVisiblePos());
        }
        else {
          e.cursor->Goto(e.cursor->GetX(),y);
        }
      }
    }

    void CursorPageDownSelection(Edit& e)
    {
      if (e.IsUnselected()) {
        e.SetStartMark(e.cursor);
      }

      if (e.GetLastVisiblePos()==e.model->GetLines()) {
        e.cursor->Goto(e.cursor->GetX(),e.model->GetLines());
      }
      else {
        size_t y;

        // Remember cursor position if cursor is visible
        y=e.cursor->GetY();

        e.Page(e.vSize-1);

        // Reposition cursor
        y=std::min(y+e.vSize-1,e.model->GetLines());

        if (y<e.GetFirstVisiblePos()) {
          e.cursor->Goto(e.cursor->GetX(),e.GetFirstVisiblePos());
        }
        else if (y>e.GetLastVisiblePos()) {
          e.cursor->Goto(e.cursor->GetX(),e.GetLastVisiblePos());
        }
        else {
          e.cursor->Goto(e.cursor->GetX(),y);
        }
      }

      e.SetEndMark(e.cursor);
    }

    void CursorGotoStart(Edit& e)
    {
      e.ClearSelection();
      e.cursor->GotoPos(0);
    }

    void CursorGotoEnd(Edit& e)
    {
      e.ClearSelection();
      if (e.cursor->GotoRow(e.model->GetLines())) {
        e.cursor->GotoColumn(e.cursor->GetLineLength()+1);
      }
    }

    void CursorGotoSOL(Edit& e)
    {
      e.ClearSelection();
      e.cursor->GotoColumn(1);
    }

    void CursorGotoSOLSelection(Edit& e)
    {
      if (e.IsUnselected()) {
        e.SetStartMark(e.cursor);
      }

      if (e.cursor->GotoColumn(1)) {
        e.SetEndMark(e.cursor);
      }
    }

    void CursorGotoEOL(Edit& e)
    {
      e.ClearSelection();
      e.cursor->GotoColumn(e.cursor->GetLineLength()+1);
    }

    void CursorGotoEOLSelection(Edit& e)
    {
      if (e.IsUnselected()) {
        e.SetStartMark(e.cursor);
      }

      if (e.cursor->GotoColumn(e.cursor->GetLineLength()+1)) {
        e.SetEndMark(e.cursor);
      }
    }

    void CursorGotoFirstVisibleRow(Edit& e)
    {
      e.ClearSelection();
      e.cursor->GotoRow(e.GetFirstVisiblePos());
    }

    void CursorGotoLastVisibleRow(Edit& e)
    {
      e.ClearSelection();
      e.cursor->GotoRow(e.GetLastVisiblePos());
    }

    void DisplayRefresh(Edit& e)
    {
      e.Redraw();
    }

    void DisplayScrollUp(Edit& e)
    {
      if (e.CanScrollUp()) {
        e.Page(-1);
      }
    }

    void DisplayScrollDown(Edit& e)
    {
      if (e.CanScrollDown()) {
        e.Page(1);
      }
    }

    void DisplayScrollLeft(Edit& e)
    {
      if (e.CanScrollLeft()) {
        e.ScrollSidewards(-1);
      }
    }

    void DisplayScrollRight(Edit& e)
    {
      if (e.CanScrollRight()) {
        e.ScrollSidewards(1);
      }
    }

    /*
      Editing
    */

    void CursorDelete(Edit& e)
    {
      if (!e.IsWriteable()) {
        e.DisplayBeep();
        return;
      }

      if (e.IsSelected()) {
        e.DeleteSelection();
      }
      else {
        e.tmpCursor->Goto(e.cursor);
        if (e.tmpCursor->Forward()) {
          if (!e.model->DeleteArea(e.cursor,e.tmpCursor)) {
            e.DisplayBeep();
          }
        }
        else {
          e.DisplayBeep();
        }
      }
    }

    void CursorBackspace(Edit& e)
    {
      if (!e.IsWriteable()) {
        e.DisplayBeep();
        return;
      }

      if (e.IsSelected()) {
        e.DeleteSelection();
      }
      else if (e.cursor->GetX()>e.cursor->GetLineLength()+1) {
        CursorLeft(e);
      }
      else {
        e.tmpCursor->Goto(e.cursor);
        if (e.tmpCursor->Back()) {
          if (!e.model->DeleteArea(e.tmpCursor,e.cursor)) {
            e.DisplayBeep();
          }
        }
        else {
          e.DisplayBeep();
        }
      }
    }

    void CursorDeleteLine(Edit& e)
    {
      if (!e.IsWriteable()) {
        e.DisplayBeep();
        return;
      }

      size_t oldX;

      e.ClearSelection();

      oldX=e.cursor->GetX();

      if (e.cursor->GotoColumn(1)) {
        e.tmpCursor->Goto(e.cursor);
        if (e.tmpCursor->Goto(1,e.cursor->GetY()+1)) {
          if (e.model->DeleteArea(e.cursor,e.tmpCursor)) {
            e.cursor->GotoColumn(oldX);
          }
          else {
            e.DisplayBeep();
          }
        }
      }
    }

    void CursorSplitLine(Edit& e)
    {
      if (!e.IsWriteable()) {
        e.DisplayBeep();
        return;
      }

      e.DeleteSelection();

      if (e.model->Insert(e.cursor,e.model->GetEOLToken())) {
        e.MakeCursorVisible(e.cursor);
      }
      else {
        e.DisplayBeep();
      }
    }

    void CursorInsertBuffer(Edit& e)
    {
      if (!e.IsWriteable()) {
        e.DisplayBeep();
        return;
      }

      e.DeleteSelection();

      std::wstring text;

      text=::Lum::OS::display->GetClipboard();

      if (!text.empty()) {
        e.model->Insert(e.cursor,text);
      }
      else {
        e.DisplayBeep();
      }
    }

    void SelectionCopy(Edit& e)
    {
      BufferRef    buffer;
      std::wstring text;

      if (e.IsSelected()) {
        buffer=e.model->CopyArea(e.GetStartMark(),e.GetEndMark());

        if (buffer.Valid()) {
          text=buffer->GetText();
          if (!text.empty()) {
            if (!::Lum::OS::display->SetClipboard(text)) {
              e.DisplayBeep();
            }
          }
        }
      }
      else {
        e.DisplayBeep();
      }
    }

    void SelectionDelete(Edit& e)
    {
      e.DeleteSelection();
    }

    void SelectionCut(Edit& e)
    {
      SelectionCopy(e);
      SelectionDelete(e);
    }

    void TextSelectAll(Edit& e)
    {
      e.tmpCursor->Goto(e.cursor);
      e.tmpCursor->GotoPos(0);
      e.SetStartMark(e.tmpCursor);

      e.tmpCursor->Goto(e.cursor);
      e.tmpCursor->GotoPos(e.model->GetSize()-1);
      e.SetEndMark(e.tmpCursor);
    }

    void SelectionClear(Edit& e)
    {
      e.ClearSelection();
    }

    void SelectionIndent(Edit& e)
    {
      if (e.IsSelected()) {
        CursorPtr markA,markB;

        markA=e.GetStartMark();
        markB=e.GetEndMark();
        e.tmpCursor->Goto(markB);

        if (e.tmpCursor->Back()) {
          size_t lastLine=e.tmpCursor->GetY();

          e.tmpCursor->Goto(markA);
          if (e.tmpCursor->GotoColumn(1)) {
            while (e.tmpCursor->GetY()<=lastLine) {
              e.model->Insert(e.tmpCursor,L' ');
              e.tmpCursor->Goto(1,e.tmpCursor->GetY()+1);
            }
          }
        }
      }
      else {
        e.DisplayBeep();
      }
    }

    void SelectionOutdent(Edit& e)
    {
      if (e.IsSelected()) {
        CursorPtr markA,markB;

        markA=e.GetStartMark();
        markB=e.GetEndMark();
        e.tmpCursor->Goto(markB);

        if (e.tmpCursor->Back()) {
          size_t lastLine=e.tmpCursor->GetY();

          e.tmpCursor->Goto(markA);
          if (e.tmpCursor->GotoColumn(1)) {
            while (e.tmpCursor->GetY()<=lastLine) {
              if (e.tmpCursor->GetChar()==L' ') { // TODO: Check for whitespace instead
                e.tmp2Cursor->Goto(e.tmpCursor);
                e.tmp2Cursor->Forward();
                e.model->DeleteArea(e.tmpCursor,e.tmp2Cursor);
              }
              e.tmpCursor->Goto(1,e.tmpCursor->GetY()+1);
            }
          }
        }
      }
      else {
        e.DisplayBeep();
      }
    }

    void TextUndo(Edit& e)
    {
      if (!e.model->UndoStep(e.cursor,e.tmpCursor)) {
        e.DisplayBeep();
      }
    }

    void TextRedo(Edit& e)
    {
      if (!e.model->RedoStep(e.cursor,e.tmpCursor)) {
        e.DisplayBeep();
      }
    }

    void TextClear(Edit& e)
    {
      e.model->Clear();
    }
  }
}
