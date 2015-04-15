#ifndef LUM_EDIT_TEXT_H
#define LUM_EDIT_TEXT_H

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

#include <vector>

#include <Lum/Base/Model.h>
#include <Lum/Base/Status.h>

#include <Lum/Edit/Syntax.h>

namespace Lum {
  namespace Edit {

    class Cursor;
    typedef Cursor *CursorPtr;

    class Text;
    typedef Base::Reference<Text> TextRef;

    class Buffer;
    typedef Base::Reference<Buffer> BufferRef;

    enum EOLMode {
      EOL_DOS,
      EOL_Unix,
      EOL_Mac
    };

    /**
      for tempory storing text position
    */
    class Pos
    {
    public:
      size_t pos;
      size_t x;
      size_t y;
    };

    /**
      A Cursor
    */
    class LUMAPI Cursor
    {
      friend class Text;

    public:
      enum Flags
      {
        cursorFix      = 1 << 0, //< Cursor will keep its position even if some editing happens
        cursorHidden   = 1 << 1, //< This cursor is for internal use and should not be displayed
        cursorBookmark = 1 << 2  //< This is a bookmark
      };

    private:
      std::wstring  name;
      unsigned long flags; //< One or more of Flags.
      Text*         text;  //< Pointer to text object the cursor referes to.
      size_t        pos;   //< Absolute position in text [0..textLength]
      size_t        x;     //< Column in current line [1..max]
      size_t        y;     //< Riw in text [1..max]

    public:
      Cursor(const std::wstring& name, unsigned long flags);

      void LinkText(Text* text);
      void UnlinkText();

      std::wstring GetName() const;
      unsigned long GetFlags() const;

      wchar_t GetChar() const;
      wchar_t GetPreviousChar() const;
      size_t GetLineLength() const;
      std::wstring GetLine() const;
      size_t GetX() const;
      size_t GetY() const;
      size_t GetPos() const;

      void Goto(Cursor* cursor);
      bool GotoPos(size_t pos);
      bool Goto(size_t x, size_t y);
      bool GotoColumn(size_t column);
      bool GotoRow(size_t row);
      bool GotoNextOccurence(const std::wstring& string, bool caseMatch);
      bool Move(int offset);
      bool Back();
      bool Forward();
      bool Left();
      bool Right();
      bool Up();
      bool Down();
    };

    class LUMAPI Buffer : public Base::Referencable
    {
    private:
      std::wstring text;
      EOLMode      eolMode;

    public:
      Buffer();
      Buffer(const std::wstring& text,EOLMode eolMode);

      void SetText(const std::wstring& text, EOLMode eolMode);

      const std::wstring& GetText() const;
      const std::wstring GetText(EOLMode eolMode) const;
    };

    /**
      Stores syntax parsing information
     */
    class LUMAPI State
    {
    private:
      Text*                  text;     //! Back pointer to the text
      size_t                 row;      //! the current row
      std::vector<StateInfo> memory;   //! Stored StateInfo for the given line
      size_t                 highMark;
      Cursor                 *cursor;  //! The cursor the State uses for iterating over the text

    public:
      StateInfo              state;

    public:
      State(Text* text);
      void Init();
      void RememberState(size_t line, const StateInfo& state);
      void ForgetStateBehindLine(size_t line);
      StateInfo MoveToLine(size_t line);
    };

    /**
      The text.
    */
    class LUMAPI Text : public Base::Model
    {
      friend class Cursor;

    private:
      enum UndoMode
      {
        undoNormal,
        undoUndo,
        undoRedo
      };

    private:
      /**
        Baseclass for holding text manipulation information
       */
      class Undo
      {
      public:
        virtual ~Undo() {};
      };

      /**
        All information to undo a text insertion
       */
      class UndoInsert : public Undo
      {
      public:
        size_t pos;
        size_t count;

      public:
        UndoInsert(size_t pos, size_t count);
      };

      /**
        All information to undo a text deletion
       */
      class UndoDelete : public Undo
      {
      public:
        size_t       pos;
        std::wstring text;

      public:
        UndoDelete(size_t pos, const std::wstring& text);

      };

      /**
       Groups a number of undo changes
       */
      class UndoGroup
      {
      public:
        std::list<Undo*> entries;
        bool             changed;

      public:
        ~UndoGroup();
      };

    public:
      class RedrawMsg : public Base::ResyncMsg
      {
      public:
        size_t from;
        size_t to;
      };

      class LUMAPI DeleteMsg : public Base::ResyncMsg
      {
      public:
        size_t from;
        size_t count;
      };

      class LUMAPI InsertMsg : public Base::ResyncMsg
      {
      public:
        size_t from;
        size_t count;
      };

      class LUMAPI CursorMsg : public Base::ResyncMsg
      {
      public:
        Cursor *cursor;
        Pos    oldPos;
      };

      class LUMAPI PropertyChangedMsg : public Base::ResyncMsg
      {
      };

      class LUMAPI TextChangedMsg : public Base::ResyncMsg
      {
      };

      class LUMAPI FilenameChangedMsg : public Base::ResyncMsg
      {
      };

    private:
      std::list <Cursor*>  cursors;

      UndoMode             undoMode;
      std::list<UndoGroup> undoStack;
      std::list<UndoGroup> redoStack;

      bool                 changed;
      bool                 readOnly;
      EOLMode              eolMode;

      std::wstring         fileName;

      wchar_t              *buffer;
      size_t               bufferSize;
      size_t               fillSize;
      size_t               gapPos;
      size_t               lines;

    public:
      State                state;
      Syntax               *syntax;

    private:
      void ReserveBuffer(size_t size);
      void SetBuffer(const std::wstring& text);
      void SetBuffer(const wchar_t *text, size_t length);
      void MoveGap(size_t pos, size_t length);
      void InsertGap(size_t pos, const std::wstring& text);
      void InsertGap(size_t pos, const wchar_t* text, size_t length);
      size_t GetChars() const;

      void DetectEOLMode();
      void CountText(const std::wstring& text, size_t &firstX, size_t &lastX, size_t &lines);
      void CountLines();
      void ResetCursors();
      void PushUndo(Undo* undo);
      void PushRedo(Undo* undo);
      void NewRedoGroup();

    public:
      Text();
      ~Text();

      void NotifyRedraw(size_t a, size_t b);
      void NotifyDelete(size_t from, size_t count);
      void NotifyInsert(size_t from, size_t count);
      void NotifyPropertyChanged();
      void NotifyTextChanged();
      void NotifyFilenameChanged();

      // getters
      std::wstring GetFileName() const;
      bool IsReadOnly() const;
      bool HasChanged() const;
      size_t GetLines() const;
      size_t GetSize() const;
      EOLMode GetEOLMode() const;
      std::wstring GetEOLToken() const;


      // setters
      void SetReadOnly(bool readOnly);
      void SetChanged(bool changed);
      void SetFileName(const std::wstring& fileName/* syntax : S.Syntax*/);
      void SetSyntax(Syntax* syntax);

      // Cursors
      Cursor* CreateCursor(const std::wstring& name, unsigned long flags);
      Cursor* GetCursor(const std::wstring& name) const;
      Cursor* GetOrCreateCursor(const std::wstring& name, unsigned long flags);
      void FreeCursor(Cursor* cursor);

      void NewUndoGroup();

      // Text
      void Clear();
      void SetText(const wchar_t* text, size_t length);
      void SetText(const std::wstring& text);
      std::wstring GetText() const;

      Base::Status Load(const std::wstring& fileName);
      Base::Status Save(const std::wstring& fileName, bool stripTrailingSpace);

      bool UndoStep(Cursor* cursor, Cursor* cursor2);
      bool RedoStep(Cursor* cursor, Cursor* cursor2);

      wchar_t GetChar(size_t pos) const;
      bool IsEOL(size_t pos) const;
      bool Insert(Cursor* cursor, const std::wstring& string);
      bool Insert(Cursor* cursor, wchar_t character, size_t count=1);
      bool Insert(Cursor* cursor, BufferRef buffer);
      bool DeleteArea(Cursor* markA, Cursor* markB);
      BufferRef CopyArea(Cursor* markA, Cursor* markB);
    };
  }
}

#endif
