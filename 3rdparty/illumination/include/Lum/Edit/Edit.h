#ifndef LUM_EDIT_EDIT_H
#define LUM_EDIT_EDIT_H

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

  You should have received a copy of the GNU Lesser General16 Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/

#include <map>
#include <vector>

#include <Lum/Object.h>

#include <Lum/Base/Object.h>

#include <Lum/Edit/Style.h>
#include <Lum/Edit/Syntax.h>
#include <Lum/Edit/Text.h>

#include <Lum/Model/Action.h>
#include <Lum/Model/Number.h>

#include <Lum/OS/Bitmap.h>
#include <Lum/OS/Font.h>

namespace Lum {
  namespace Edit {

    class LUMAPI LineInfo
    {
    public:
      enum Decoration {
        decoCursor = 1 << 0, //! Draw a cursor bar
        decoBlock  = 2
      };

    public:
      std::wstring               orig;     //! The original text.
      std::wstring               text;     //! The converted and to be displayed  text (style, character exchange...)
      StateInfo                  state;    //! Teh starting state for this line.
      std::vector<size_t>        pos;      //! Line position of each character in 'text'.
      std::vector<Style>         style;    //! Style of each character in 'text'.
      std::vector<Token*>        token;    //! The corresponding parsing token for each character in text.
      std::vector<bool>          start;    //! Start of a token.
      std::vector<size_t>        width;    //! Width of the character.
      std::vector<unsigned long> decoration; //! Some optional decoration

    private:
      void Expand(size_t size);

    public:
      void SetArea(size_t x1, size_t x2, const Style& style);
      void SetBackground(size_t x1, size_t x2,
                         const Style& normalStyle,
                         const Style& backgroundStyle);
      void SetDecoration(size_t x1, unsigned long decoration);
    };

    class Edit;
    typedef void (*Function)(Edit& e);

    class LUMAPI Config : public Lum::Base::Referencable
    {
    private:
      typedef std::map<std::wstring,Function> FunctionMap;

    private:
      std::wstring name;
      bool         createBackup;
      bool         remTrailSpace;
      bool         showEOL;
      bool         showTab;
      bool         highlightTab;
      bool         showSpace;
      bool         showCurrentLine;
      FunctionMap  keys;
      Syntax       *syntax;

    public:
      Config(const std::wstring& name);

      virtual void Inherit(Config* config);

      std::wstring GetName() const;

      void SetSyntax(Syntax* syntax);
      Syntax* GetSyntax() const;

      void SetCreateBackup(bool create);
      bool GetCreateBackup() const;

      void SetRemTrailingSpace(bool rem);

      bool ShowEOL() const;
      void SetShowEOL(bool show);

      bool ShowTab() const;
      void SetShowTab(bool show);

      bool HighlightTab() const;
      void SetHighlightTab(bool highlight);

      bool ShowSpace() const;
      void SetShowSpace(bool show);

      bool ShowCurrentLine() const;
      void SetShowCurrentLine(bool show);

      Function GetFunctionForKey(const std::wstring& key) const;
      bool RegisterKey(const std::wstring& name, Function function);

      static Config* GetDefaultConfig();
    };

    typedef Lum::Base::Reference<Config> ConfigRef;

    class LUMAPI EditMsg : public Base::ResyncMsg
    {
    public:
      Edit *edit;

    public:
      EditMsg(Edit* edit);
    };

    class LUMAPI Edit : public Scrollable
    {
    private:
      class Line
      {
      public:
        StateInfo state;
        bool      drawn;
      };

    private:
      ConfigRef               config;

      // Prefs
      size_t                  hScrollOffset;
      bool                    requiresKnob;

      // low level text display stuff
      size_t                  spaceWidth;
      Cursor                  *startLine;
      Cursor                  *workCursor;

      OS::FontRef             font;
      OS::Bitmap              *drawBuffer;

      std::vector<Line>       lines;

      // refresh tracking
      std::list<size_t>       refreshLines;   //! List of lines (line in model) that needs refreshing
      bool                    refreshMove;    //! Refresh will just move text, without redrawing individual lines
      size_t                  refreshLastTop; //! top position of last draw
      bool                    refreshFull;    //! Refresh will be a complete resync and redraw

      Model::ActionRef        actionPropertyChanged;
      Model::ActionRef        actionFunctionExecuted;
      Model::ActionRef        actionResolveConfig;

    public:
      // The text
      TextRef                 model;

      ColorShemeRef           colorSheme;

      // Text status
      bool                    stableSelection;
      bool                    readWrite; //! allow write operations

      // window dimensions
      size_t                  vSize;

      Cursor                  *cursor;
      Cursor                  *markA;
      Cursor                  *markB;
      Cursor                  *tmpCursor;
      Cursor                  *tmp2Cursor;
      Cursor                  *tmp3Cursor;
      Cursor                  *blockCursor;

      bool                    showBlockCursor;

    private:
      // RefreshTracking
      bool CanRefreshRedraw() const;
      bool CanRefreshMove() const;
    
      void ResizeLines();

      void GetLine(size_t y, Cursor* &cursor) const;
      void ExtractText(Cursor*& cursor,LineInfo& info) const;
      void Colorize(size_t yPos,LineInfo& info);

      bool CoordsToTextPos(int cX, int cY, size_t &x, size_t &y);

      void SetDirtyVisibleRange(size_t start, size_t size);
      void SetDirty(size_t line);

      void MoveCursor(size_t ox, size_t oy, size_t nx, size_t ny);

      void PrintChar(OS::DrawInfo* draw,
                     LineInfo& info,
                     size_t pos, size_t x);
      void PrintLine(OS::DrawInfo* draw, LineInfo& info,
                     size_t pos, size_t line);

      void Update(OS::DrawInfo* draw);
      void UpdateDimensions();

      void ClearSelectionInternal();

    public:
      Edit();
      ~Edit();

      bool CanExecuteAction(const Base::Datatype& type,
                            Base::DataExchangeObject::ExchangeAction action) const;
      bool ExecuteAction(const Base::Datatype& type,
                         Base::Data& data,
                         Base::DataExchangeObject::ExchangeAction action);
      bool PostprocessAction(Base::DataExchangeObject::ExchangeAction action);

      Config* GetConfig() const;
      void SetConfig(Config* config);
      void SetColorSheme(const ColorShemeRef& colorSheme);
      void SetFont(OS::Font* font);

      void DisplayBeep();

      void SetRequiresKnobs(bool requiresKnob);

      bool RequiresKnobs() const;
      bool IsWriteable() const;

      bool IsVisible(size_t y) const;
      size_t GetFirstVisiblePos() const;
      size_t GetLastVisiblePos() const;

      size_t TextToDisplayPos(size_t y) const;
      size_t DisplayToTextPos(size_t y) const;

      bool TextPosToCoords(size_t x, size_t y, int &cX, int &cY);

      void GetLineInfo(Cursor*& cursor, LineInfo& info);

      void SetPropertyChangedAction(Model::Action* action);
      void SetFunctionExecutedAction(Model::Action* action);
      void SetResolveConfigAction(Model::Action* action);

      void TriggerResolveConfigAction();
      void TriggerPropertyChangedAction();
      void TriggerFunctionExecutedAction();

      bool CanScrollUp() const;
      bool CanScrollDown();
      bool CanScrollLeft() const;
      bool CanScrollRight() const;

      void Page(long lines);
      void ScrollSidewards(long pixel);
      void MakeCursorVisible(Cursor* cursor);

      bool IsUnselected() const;
      bool IsSelected() const;
      bool IsMarkSet() const;
      Cursor* GetStartMark() const;
      Cursor* GetEndMark() const;
      void SetStartMark(Cursor* cursor);
      void SetEndMark(Cursor* cursor);
      void ClearSelection();
      void DeleteSelection();

      void ShowBlock();
      void HideBlock();

      bool SetModel(Base::Model* model);

      void CalcSize();
      void GetDimension(size_t& width, size_t& height);
      void Layout();
      void Draw(OS::DrawInfo* draw,
                int x, int y, size_t w, size_t h);

      void DrawFocus();
      void HideFocus();

      bool HandleMouseEvent(const OS::MouseEvent& event);
      bool HandleKeyEvent(const OS::KeyEvent& event);
      void Resync(Base::Model* model, const Base::ResyncMsg& msg);

      void ExecuteFunction(Function function);
      bool ExecuteFunctionKey(const std::wstring& key);
      void ExecutePost();
    };
  }
}

#endif
