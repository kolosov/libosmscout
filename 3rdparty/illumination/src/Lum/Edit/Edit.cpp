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

#include <Lum/Edit/Edit.h>

#include <cassert>
#include <cstdlib>
#include <limits>

#include <Lum/Base/Util.h>

#include <Lum/Edit/Function.h>

#include <Lum/OS/Driver.h>

namespace Lum {
  namespace Edit {

    /**
      Expand LineInfo to also cover the new position.

      Currently only used when cursor is behind end of line.
    */
    void LineInfo::Expand(size_t size)
    {
      assert(pos.size()>0);

      if (size>pos[pos.size()-1]) {
        size_t oldSize,newSize,y;

        oldSize=pos.size();
        newSize=oldSize+(size-pos[oldSize-1]);

        pos.resize(newSize);
        y=pos[oldSize-1]+1;
        for (size_t x=oldSize; x<newSize; x++) {
          pos[x]=y;
          y++;
        }

        oldSize=text.size();
        text.resize(newSize);
        for (size_t x=oldSize; x<newSize; x++) {
          text[x]=L' '; // TODO
        }

        oldSize=style.size();
        style.resize(newSize,style[oldSize-1]);

        token.resize(newSize,NULL);
        start.resize(newSize,false);
        width.resize(newSize,0);
        decoration.resize(newSize,0);
      }
    }

    void LineInfo::SetArea(size_t x1, size_t x2, const Style& style)
    {
      size_t x;

      Expand(x2);

      /*
        Find start position x1 in text.
      */
      x=x1-1; // Optimisation: x1 will be at x or later but never before
      while (x<this->style.size() && pos[x]<x1) {
        x++;
      }

      while (x<this->style.size() && pos[x]<=x2) {
        this->style[x]=style;

        x++;
      }
    }

    void LineInfo::SetBackground(size_t x1, size_t x2,
                                 const Style& normalStyle,
                                 const Style& backgroundStyle)
    {
      size_t x;

      Expand(x2);

      /*
        Find start position x1 in text.
      */
      x=x1-1; // Optimisation: x1 will be at x or later but never before
      while (x<this->style.size() && pos[x]<x1) {
        x++;
      }

      while (x<this->style.size() && pos[x]<=x2) {
        if (this->style[x].bg==normalStyle.bg) {
          this->style[x].bg=backgroundStyle.bg;
        }

        x++;
      }
    }

    void LineInfo::SetDecoration(size_t x1, unsigned long decoration)
    {
      size_t x;

      Expand(x1);

      /*
        Find start position x1 in text.
      */
      x=x1-1; // Optimisation: x1 will be at x or later but never before
      while (x<this->style.size() && pos[x]<x1) {
        x++;
      }

      this->decoration[x]|=decoration;
    }

    Config::Config(const std::wstring& name)
    : name(name),
      createBackup(false),
      remTrailSpace(false),
      showEOL(false),
      showTab(false),
      highlightTab(false),
      showSpace(false),
      showCurrentLine(false),
      syntax(syntaxRepository->Get(L"Standard"))
    {
      // no code
    }

    std::wstring Config::GetName() const
    {
      return name;
    }

    /**
     Copies/inherits all settinsg from config - besides the name.
    */
    void Config::Inherit(Config* config)
    {
      createBackup=config->createBackup;
      remTrailSpace=config->remTrailSpace;
      showEOL=config->showEOL;
      showTab=config->showTab;
      highlightTab=config->highlightTab;
      showSpace=config->showSpace;
      showCurrentLine=config->showCurrentLine;
      keys=config->keys;
      syntax=config->syntax;
    }

    /**
      Call this method to register a new key combination an connect it
      to an existing function.

      TODO: Change to void f()
    */
    bool Config::RegisterKey(const std::wstring& name, Function function)
    {
      keys[name]=function;
      return true;
    }

    Function Config::GetFunctionForKey(const std::wstring& key) const
    {
      FunctionMap::const_iterator iter;

      iter=keys.find(key);
      if (iter!=keys.end()) {
        return iter->second;
      }
      else {
        return NULL;
      }
    }

    Syntax* Config::GetSyntax() const
    {
      return syntax;
    }

    bool Config::GetCreateBackup() const
    {
      return createBackup;
    }

    bool Config::ShowEOL() const
    {
      return showEOL;
    }

    void Config::SetShowEOL(bool show)
    {
      showEOL=show;
    }

    bool Config::ShowTab() const
    {
      return showTab;
    }

    void Config::SetShowTab(bool show)
    {
      showTab=show;
    }

    bool Config::HighlightTab() const
    {
      return highlightTab;
    }

    void Config::SetHighlightTab(bool highlight)
    {
      highlightTab=highlight;
    }

    bool Config::ShowSpace() const
    {
      return showSpace;
    }

    void Config::SetShowSpace(bool show)
    {
      showSpace=show;
    }

    bool Config::ShowCurrentLine() const
    {
      return showCurrentLine;
    }

    void Config::SetShowCurrentLine(bool show)
    {
      showCurrentLine=show;
    }

    void Config::SetSyntax(Syntax* syntax)
    {
      this->syntax=syntax;
    }

    void Config::SetCreateBackup(bool create)
    {
      createBackup=create;
    }

    void Config::SetRemTrailingSpace(bool rem)
    {
      remTrailSpace=rem;
    }

    Config* Config::GetDefaultConfig()
    {
      Config* config;

      config=new Config(L"Default");
      config->RegisterKey(L"Left",CursorLeft);
      config->RegisterKey(L"Right",CursorRight);
      config->RegisterKey(L"Up",CursorUp);
      config->RegisterKey(L"Down",CursorDown);

      config->RegisterKey(L"S+Left",CursorLeftSelection);
      config->RegisterKey(L"S+Right",CursorRightSelection);
      config->RegisterKey(L"S+Up",CursorUpSelection);
      config->RegisterKey(L"S+Down",CursorDownSelection);

      config->RegisterKey(L"C+Left",DisplayScrollLeft);
      config->RegisterKey(L"C+Right",DisplayScrollRight);
      config->RegisterKey(L"C+Up",DisplayScrollUp);
      config->RegisterKey(L"C+Down",DisplayScrollDown);

      config->RegisterKey(L"Prior",CursorPageUp);
      config->RegisterKey(L"Next",CursorPageDown);

      config->RegisterKey(L"S+Prior",CursorPageUpSelection);
      config->RegisterKey(L"S+Next",CursorPageDownSelection);

      config->RegisterKey(L"C+Prior",CursorGotoStart);
      config->RegisterKey(L"C+Next",CursorGotoEnd);

      config->RegisterKey(L"Home",CursorGotoSOL);
      config->RegisterKey(L"End",CursorGotoEOL);

      config->RegisterKey(L"S+Home",CursorGotoSOLSelection);
      config->RegisterKey(L"S+End",CursorGotoEOLSelection);

      config->RegisterKey(L"C+Home",CursorGotoFirstVisibleRow);
      config->RegisterKey(L"C+End",CursorGotoLastVisibleRow);

      config->RegisterKey(L"MouseButton4",DisplayScrollUp);
      config->RegisterKey(L"MouseButton5",DisplayScrollDown);
      config->RegisterKey(L"MouseButton4-doubleclick",DisplayScrollUp);
      config->RegisterKey(L"MouseButton5-doubleclick",DisplayScrollDown);
      config->RegisterKey(L"MouseButton4-trippleclick",DisplayScrollUp);
      config->RegisterKey(L"MouseButton5-trippleclick",DisplayScrollDown);

      config->RegisterKey(L"Delete",CursorDelete);
      config->RegisterKey(L"BackSpace",CursorBackspace);
      config->RegisterKey(L"Return",CursorSplitLine);

      config->RegisterKey(L"C+y",CursorDeleteLine);

      config->RegisterKey(L"A+BackSpace",TextUndo);
      config->RegisterKey(L"SA+BackSpace",TextRedo);

      config->RegisterKey(L"Escape",SelectionClear);

      return config;
    }

    EditMsg::EditMsg(Edit* edit)
    : edit(edit)
    {
      assert(edit!=NULL);
    }

    Edit::Edit()
    : config(NULL),
      hScrollOffset(3),
      requiresKnob(true),
      font(OS::display->GetFont(OS::Display::fontTypeFixed)),
      drawBuffer(NULL),
      //font(OS::display->GetFont(OS::Display::fontTypeProportional)),
      actionPropertyChanged(new Model::Action),
      actionFunctionExecuted(new Model::Action),
      actionResolveConfig(new Model::Action),
      colorSheme(NULL),
      stableSelection(true),
      readWrite(true),
      vSize(0),
      showBlockCursor(false)
    {
      SetBackground(OS::display->GetFill(OS::Display::listboxBackgroundFillIndex));

      SetClipFromParent(true);
      SetClipBackgroundToBorder(true);
      
      SetCanFocus(true);
      RequestFocus();
      SetRequestsKeyboard(true);

      colorSheme=ColorSheme::GetDefaultColorSheme();
    }

    Edit::~Edit()
    {
      delete drawBuffer;
    }

    bool Edit::CanRefreshRedraw() const
    {
      if (refreshFull) {
        return false;
      }
      
      return true;
    }

    bool Edit::CanRefreshMove() const
    {
      if (refreshFull) {
        return false;
      }
      
      return true;
    }

    bool Edit::CanExecuteAction(const Base::Datatype& type,
                                Base::DataExchangeObject::ExchangeAction action) const
    {
      if (!type.Matches(Lum::Base::Datatype::text)) {
        return false;
      }

      switch (action) {
      case actionMove:
        return model.Valid() && !model->IsReadOnly() && IsSelected();
        break;
      case actionCopy:
        return model.Valid() && IsSelected();
        break;
      case actionInsert:
        return model.Valid() && !model->IsReadOnly();
        break;
      default:
        return false;
      }
    }

    bool Edit::ExecuteAction(const Base::Datatype& type,
                             Base::Data& data,
                             Base::DataExchangeObject::ExchangeAction action)
    {
      std::wstring text;
      BufferRef    buffer;

      if (!CanExecuteAction(type,action)) {
        return false;
      }


      switch (action) {
      case actionInsert:
        text=data.GetText();

        if (text.length()==0) {
          return false;
        }
        model->NewUndoGroup();
        DeleteSelection();
        model->Insert(cursor,text);
        MakeCursorVisible(cursor);
        ExecutePost();
        return true;
      case actionCopy:
        buffer=model->CopyArea(GetStartMark(),GetEndMark());

        if (!buffer.Valid()) {
          return false;
        }

        data.SetText(buffer->GetText());
        return true;
      case actionMove:
        buffer=model->CopyArea(GetStartMark(),GetEndMark());

        if (!buffer.Valid()) {
          return false;
        }

        data.SetText(buffer->GetText());
      return true;
      default:
        return false;
      }
    }

    bool Edit::PostprocessAction(Base::DataExchangeObject::ExchangeAction action)
    {
      if (!model.Valid() || action!=actionMove || !IsSelected()) {
        return false;
      }

      DeleteSelection();

      return true;
    }

    Config* Edit::GetConfig() const
    {
      return config.Get();
    }

    void Edit::SetConfig(Config* config)
    {
      this->config=config;
      if (model.Valid()) {
        model->SetSyntax(config->GetSyntax());
      }

      refreshFull=true;
      Redraw();
    }

    void Edit::SetColorSheme(const ColorShemeRef& colorSheme)
    {
      this->colorSheme=colorSheme;
    }

    void Edit::SetFont(OS::Font* font)
    {
      assert(!inited); // TODO: ake it always setable!

      this->font=font;
    }

    void Edit::CalcSize()
    {
      spaceWidth=font->StringWidth(L" ",OS::Font::normal);

      minWidth=5*font->StringWidth(L"m",OS::Font::normal);
      minHeight=3*font->height;

      width=minWidth;
      height=minHeight;

      Scrollable::CalcSize();
    }

    void Edit::SetRequiresKnobs(bool requiresKnob)
    {
      this->requiresKnob=requiresKnob;
    }

    bool Edit::RequiresKnobs() const
    {
      return requiresKnob;
    }

    bool Edit::IsWriteable() const
    {
      return readWrite;
    }

    void Edit::DisplayBeep()
    {
      OS::display->Beep();
    }

    void Edit::ResizeLines()
    {
      lines.resize(vSize);

      for (size_t x=0; x<vSize; x++) {
        lines[x].drawn=false;
        lines[x].state.Reset();
      }
    }

    void Edit::GetLine(size_t y, Cursor* &cursor) const
    {
      cursor->Goto(startLine); // Move cursor to first displayed line
      cursor->Goto(1,y);
    }

    void Edit::GetLineInfo(Cursor*& cursor, LineInfo& info)
    {
      ExtractText(cursor,info);
      Colorize(cursor->GetY(),info);
    }

    /**
      Return first visible row relative to the start of the document ([1..text.GetLines()]).
    */
    size_t Edit::GetFirstVisiblePos() const
    {
      return startLine->GetY();
    }

    /**
      Return last visible row relative to the start of the document ([1..text.GetLines()]).
    */
    size_t Edit::GetLastVisiblePos() const
    {
      return std::min(startLine->GetY()+height/font->height-1,model->GetLines());
    }

    /**
      Return s true, if the given line in the document ([1..text.GetLines()]) is visible.
    */
    bool Edit::IsVisible(size_t y) const
    {
      return y>=GetFirstVisiblePos() && y<=GetLastVisiblePos();
    }

    /**
      Returns the display coordinate ([1..vSize]) of the given textline ([1..text.GetLines()]).
    */
    size_t Edit::TextToDisplayPos(size_t y) const
    {
      assert(IsVisible(y));

      return y-GetFirstVisiblePos()+1;
    }

    /**
      Returns the text coordinate ([1..text.GetLines()]) of the given display line ([1..vSize]).
    */
    size_t Edit::DisplayToTextPos(size_t y) const
    {
      assert(y>=1 && y<=vSize);

      return GetFirstVisiblePos()+y-1;
    }

    bool Edit::CoordsToTextPos(int cX, int cY, size_t &x, size_t &y)
    {
      LineInfo info;
      size_t   i;
      int      xPos;

      cX=Base::RoundRange(cX,this->x,this->x+(int)width-1)-this->x;

      y=GetFirstVisiblePos()+(cY-this->y)/font->height;

      if (!IsVisible(y)) {
        return false;
      }

      /*
        Since the drawing of the line is not 1:1 similar to the internal
        array of chars (a character can be wider than one char), we let the
        parser fill the draw bufffer and related structs for this line and
        look there for the X coordinate of that screen position.
      */
      GetLine(y,workCursor);
      GetLineInfo(workCursor,info);

      xPos=-(hAdjustment->GetTop()-1);
      i=0;
      x=0;
      while (i<info.width.size() && xPos<cX) {
        xPos+=info.width[i];
        x=info.pos[i];
        i++;
      }

      if (xPos<cX) {
        x+=(cX-xPos)/spaceWidth;

        if ((cX-xPos)%spaceWidth>0) {
          x++;
        }
      }

      return true;
    }

    /**
      Returns the pixel position on the display for the given text position.
    */

    bool Edit::TextPosToCoords(size_t x, size_t y, int &cX, int &cY)
    {
      LineInfo info;
      size_t   i,pos;

      if (y<GetFirstVisiblePos() || y>GetLastVisiblePos()) {
        return false;
      }

      cY=this->y+(y-GetFirstVisiblePos()+1)*font->height;

      GetLine(y,workCursor);
      GetLineInfo(workCursor,info);

      i=0;
      pos=0;
      cX=this->x-(hAdjustment->GetTop()-1);
      while (i<info.pos.size() && info.pos[i]<x) {
        pos=info.pos[i];
        cX+=info.width[i];
        i++;
      }

      if (pos<x) {
        cX+=(x-pos)*spaceWidth;
      }

      if (cX<this->x && cX>=this->x+(int)this->width) {
        return false;
      }

      return true;
    }

    bool Edit::CanScrollUp() const
    {
      return GetFirstVisiblePos()>1;
    }

    bool Edit::CanScrollDown()
    {
      return GetLastVisiblePos()<model->GetLines();
    }

    void Edit::SetPropertyChangedAction(Model::Action* action)
    {
      actionPropertyChanged=action;
    }

    void Edit::SetFunctionExecutedAction(Model::Action* action)
    {
      actionFunctionExecuted=action;
    }

    void Edit::SetResolveConfigAction(Model::Action* action)
    {
      action->Off();
      actionResolveConfig=action;
      action->On(false);
    }

    void Edit::TriggerPropertyChangedAction()
    {
      if (!actionPropertyChanged.Valid()) {
        return;
      }

      EditMsg editMsg(this);

      actionPropertyChanged->Trigger(editMsg);
    }

    void Edit::TriggerFunctionExecutedAction()
    {
      if (!actionFunctionExecuted.Valid()) {
        return;
      }

      EditMsg editMsg(this);

      actionFunctionExecuted->Trigger(editMsg);
    }

    void Edit::TriggerResolveConfigAction()
    {
      if (!actionResolveConfig.Valid()) {
        return;
      }

      EditMsg editMsg(this);

      actionResolveConfig->Trigger(editMsg);
    }

    /**
      Marks the given display area ([1...vSize]) as "needs to be refreshed".
    */
    void Edit::SetDirtyVisibleRange(size_t start, size_t size)
    {
      for (size_t x=0; (start-1+x)<lines.size() && x<size; x++) {
        lines[start-1+x].drawn=false;
      }
    }

    /**
      Marks the given range of lines ([1..text.GetLines()]) as "needs to be refreshed".
    */
    void Edit::SetDirty(size_t line)
    {
      if (line>GetLastVisiblePos() || line<GetFirstVisiblePos()) {
        return;
      }


      lines[line-GetFirstVisiblePos()].drawn=false;
    }

    void Edit::MoveCursor(size_t ox, size_t oy, size_t nx, size_t ny)
    {
      if (CanRefreshRedraw()) {       
        if (oy==ny) {
          refreshLines.push_back(oy);
        }
        else {
          refreshLines.push_back(oy);
          refreshLines.push_back(ny);
        }
        Redraw();
      }
      else {
        refreshFull=true;
      }
    }

    void Edit::ExtractText(Cursor*& cursor,LineInfo& info) const
    {
      size_t x;
      size_t origLength;
      bool   eol=false;

      info.orig=cursor->GetLine();
      origLength=info.orig.length();

      // Cutting of potential EOL markers
      if (origLength>0 && info.orig[origLength-1]==L'\n') {
        eol=true;
        --origLength;
      }

      info.text.clear();
      info.text.reserve(origLength+1);
      info.pos.clear();
      info.pos.reserve(origLength+1);

      /*
        Character sequence substitution
      */

      x=0;
      while (x<origLength) {
        wchar_t character=info.orig[x];

        if (character==L' ') {
          if (config->ShowSpace()) {
            info.text.append(1,L'\u00b7');
          }
          else {
            info.text.append(1,L' ');
          }
          info.pos.push_back(x+1);
        }
        else if (character==L'\t' && !config->ShowTab()) {
          size_t spaces;
          size_t tabSize=8;

          spaces=(((info.text.size()+1) / tabSize)+1)*tabSize-info.text.size();

          while (spaces>0) {
            info.text.append(1,' ');
            info.pos.push_back(x+1);
            spaces--;
          }
        }
        else if (character>=0 && character<=31) {
          // hide unprintable characters
          info.text.append(1,'\\');
          info.pos.push_back(x+1);
          if (character/16<10) {
            info.text.append(1,(wchar_t)('0' + character/16));
          }
          else {
            info.text.append(1,(wchar_t)('a' + character/16 - 10));
          }
          info.pos.push_back(x+1);

          if (character%16<10) {
            info.text.append(1,(wchar_t)('0' + character%16));
          }
          else {
            info.text.append(1,(wchar_t)('a' + character%16 - 10));
          }
          info.pos.push_back(x+1);
        }
        else {
          info.text.append(1,character);
          info.pos.push_back(x+1);
        }

        x++;
      }

      // Add an EOL-marker is line was terminated by such and displaying is requested
      if (eol && config->ShowEOL()) {
        info.text.append(1,L'\u00b6');
        info.pos.push_back(x+1);
      }
      else {
        info.text.append(1,L' ');
        info.pos.push_back(x+1);
      }

      info.style.clear();
      info.style.resize(info.text.size(),colorSheme->styles[ColorSheme::normalStyle]);

      info.token.clear();
      info.token.resize(info.pos.size(),NULL);
      info.start.clear();
      info.start.resize(info.pos.size(),false);
      info.width.clear();
      info.width.resize(info.pos.size());
      info.decoration.clear();
      info.decoration.resize(info.pos.size(),0);
    }

    void Edit::Colorize(size_t yPos,LineInfo& info)
    {
      Cursor*       markA,*markB;
      size_t        a,x,length;
      unsigned long style;

      /*
        Syntax hilighting
      */

      info.state=model->state.MoveToLine(yPos);
      style=model->syntax->GetStyle(info.state);

      x=0;
      a=0;
      while (x<info.orig.length()) {
        Token* token;

        token=model->syntax->ParseToken(info.orig,x,length,info.state);
        if (token!=NULL) {
          style=token->GetStyle();
        }

        info.style[a]=colorSheme->styles[style];
        info.token[a]=token;
        info.start[a]=true;
        a++;

        while (a<info.pos.size() && info.pos[a]<=x+length) {
          info.style[a]=colorSheme->styles[style];
          info.token[a]=token;
          info.start[a]=false;
          a++;
        }

        x+=length;
      }

      /*
        Coloring of end of line
      */

      while (a<info.style.size()) {
        info.style[a]=colorSheme->styles[style];
        info.token[a]=NULL;
        info.start[a]=false;

        a++;
      }

      /*
        Coloring tabs
      */

      if (config->HighlightTab()) {
        a=0;
        while (a<info.pos.size()) {
          if (info.pos[a]<info.orig.size() && info.orig[info.pos[a]-1]==L'\t') {
            info.style[a]=colorSheme->styles[ColorSheme::specialStyle];
          }

          a++;
        }
      }

      /*
        Calling the external colorizers
      */

      std::list<Colorizer>::iterator colorizer;

      colorizer=colorSheme->colorizer.begin();
      while (colorizer!=colorSheme->colorizer.end()) {
        (*colorizer)(*this,info);

        ++colorizer;
      }

      /*
        Coloring of current selection block.
      */

      if (IsSelected()) {
        markA=GetStartMark();
        markB=GetEndMark();

        if (markA->GetY()<yPos && yPos<markB->GetY()) {
          info.SetArea(1,info.orig.length(),colorSheme->styles[ColorSheme::selectionStyle]);
        }
        else if (yPos==markA->GetY() && markA->GetY()==markB->GetY()) {
          info.SetArea(markA->GetX(),markB->GetX()-1,colorSheme->styles[ColorSheme::selectionStyle]);
        }
        else if(yPos==markA->GetY()) {
          info.SetArea(markA->GetX(),info.orig.length(),colorSheme->styles[ColorSheme::selectionStyle]);
        }
        else if (yPos==markB->GetY()) {
          info.SetArea(1,markB->GetX()-1,colorSheme->styles[ColorSheme::selectionStyle]);
        }
      }

      /*
        Coloring of secondary visible cursors
      */

      /*
      cursor:=e.model.firstCursor;
      WHILE cursor#NIL DO
        IF (T.cursorBookmark IN cursor.flags) & (cursor.GetY()=yPos) THEN
          i.SetArea(cursor.GetX(),cursor.GetX(),S.BookmarkStyle);
        END;
        cursor:=cursor.next;
      END;
      */

      /*
        Block cursor
      */

      if (showBlockCursor && blockCursor->GetY()==yPos) {
        info.SetDecoration(blockCursor->GetX(),LineInfo::decoBlock);
      }

      /*
        Drawing of cursor
      */
      if (cursor->GetY()==yPos) {
        info.SetDecoration(cursor->GetX(),LineInfo::decoCursor);
      }

      /*
        Coloring current line (optional)
       */
      if (config->ShowCurrentLine() && cursor->GetY()==yPos) {
        info.SetBackground(1,info.pos[info.pos.size()-1],
                           colorSheme->styles[ColorSheme::normalStyle],
                           colorSheme->styles[ColorSheme::currentLineStyle]);
      }

      std::wstring textPart(L" ");

      for (size_t x=0; x<info.text.length(); x++) {
        textPart[0]=info.text[x];

        info.width[x]=font->StringWidth(textPart,info.style[x].style);
      }
    }

    /**
      Print a character at the given y position (display relative) and
      x position (position in line).

      The resulting character may consist of more than one character if colorizing
      evaluates to multiple characters.
    */

    void Edit::PrintChar(OS::DrawInfo* draw,
                         LineInfo& info,
                         size_t pos, size_t x)
    {
      size_t start,end,xPos;

      xPos=0;

      start=0;
      while (start<info.pos.size() && info.pos[start]!=x) {
        xPos+=info.width[start];
        start++;
      }

      if (start<info.pos.size()) { // We have to display a character
        size_t width=info.width[start];

        end=start;
        while (end+1<info.pos.size() && info.pos[end+1]==x) {
          end++;
          width+=info.width[end];
        }

        // TODO: Check if visible

        std::wstring textPart;

        textPart=info.text.substr(start,end-start+1);

        draw->PushForeground(info.style[start].bg);
        draw->FillRectangle(this->x+xPos-(hAdjustment->GetTop()-1),
                            this->y+(pos-1)*font->height,
                            width,font->height);
        draw->PopForeground();

        draw->PushForeground(info.style[start].fg);
        draw->PushFont(font,info.style[start].style);
        draw->DrawString(this->x+xPos-(hAdjustment->GetTop()-1),
                         this->y+(pos-1)*font->height+font->ascent,
                         textPart);
        draw->PopFont();
        draw->PopForeground();

        if (info.decoration[start] & LineInfo::decoCursor) {
          draw->PushForeground(colorSheme->styles[ColorSheme::normalStyle].fg);
          draw->DrawLine(this->x+xPos-(hAdjustment->GetTop()-1),
                         this->y+(pos-1)*font->height,
                         this->x+xPos-(hAdjustment->GetTop()-1),
                         this->y+(pos-1)*font->height+font->height-1);
          draw->PopForeground();
        }

        if (info.decoration[start] & LineInfo::decoBlock) {
          draw->PushForeground(colorSheme->styles[ColorSheme::normalStyle].fg);
          draw->DrawLine(this->x+xPos-(hAdjustment->GetTop()-1),
                         this->y+(pos-1)*font->height+font->height-1,
                         this->x+xPos-(hAdjustment->GetTop()-1)+info.width[start]-1,
                         this->y+(pos-1)*font->height+font->height-1);
          draw->PopForeground();
        }
      }
      else {
        xPos+=(x-info.pos[info.pos.size()-1]-1)*spaceWidth;

        draw->PushForeground(colorSheme->styles[ColorSheme::normalStyle].bg);
        draw->FillRectangle(this->x+xPos-(hAdjustment->GetTop()-1),
                            this->y+(pos-1)*font->height,
                            spaceWidth,font->height);
        draw->PopForeground();
      }
    }

    /**
      Print the given line at the given display position.
    */
    void Edit::PrintLine(OS::DrawInfo* draw, LineInfo& info, size_t pos, size_t line)
    {
      size_t       p;
      int          xPos;
      std::wstring textPart;

      // Draw areas of equal display parameter
      p=0;
      xPos=-(hAdjustment->GetTop()-1);
      while (p<info.style.size()) {
        size_t width=0;
        size_t a=p;

        width=info.width[p];

        while (a+1<info.style.size() &&
               info.style[p].fg==info.style[a+1].fg &&
               info.style[p].bg==info.style[a+1].bg &&
               info.style[p].style==info.style[a+1].style) {
          a++;
          width+=info.width[a];
        }

        std::wstring textPart=info.text.substr(p,a-p+1);

        // TODO: Check if visible

        draw->PushForeground(info.style[p].bg);

        draw->FillRectangle(this->x+xPos,
                            this->y+(pos-1)*font->height,
                            width,
                            font->height);
        draw->PopForeground();

        draw->PushForeground(info.style[p].fg);
        draw->PushFont(font,info.style[p].style);
        draw->DrawString(this->x+xPos,this->y+(pos-1)*font->height+font->ascent,
                         textPart);
        draw->PopFont();
        draw->PopForeground();

        xPos+=width;
        p=a+1;
      }

      // Draw potential decorations

      xPos=-(hAdjustment->GetTop()-1);
      for (size_t x=0; x<info.text.length(); x++) {
        if (info.decoration[x] & LineInfo::decoCursor) {
          draw->PushForeground(colorSheme->styles[ColorSheme::normalStyle].fg);
          draw->DrawLine(this->x+xPos,this->y+(pos-1)*font->height,
                         this->x+xPos,this->y+(pos-1)*font->height+font->height-1);
          draw->PopForeground();
        }
        if (info.decoration[x] & LineInfo::decoBlock) {
          draw->PushForeground(colorSheme->styles[ColorSheme::normalStyle].fg);
          draw->DrawLine(this->x+xPos,this->y+(pos-1)*font->height+font->height-1,
                         this->x+xPos+info.width[x]-1,this->y+(pos-1)*font->height+font->height-1);
          draw->PopForeground();
        }
        xPos+=info.width[x];
      }

      // Draw area behind last text

      if (xPos<=0) {
        if (config->ShowCurrentLine() && cursor->GetY()==line) {
          draw->PushForeground(colorSheme->styles[ColorSheme::currentLineStyle].bg);
        }
        else {
          draw->PushForeground(colorSheme->styles[ColorSheme::normalStyle].bg);
        }
        draw->FillRectangle(this->x,this->y+(pos-1)*font->height,
                            width,font->height);
        draw->PopForeground();
      }
      else if (xPos<(int)width) {
        if (config->ShowCurrentLine() && cursor->GetY()==line) {
          draw->PushForeground(colorSheme->styles[ColorSheme::currentLineStyle].bg);
        }
        else {
          draw->PushForeground(colorSheme->styles[ColorSheme::normalStyle].bg);
        }
        draw->FillRectangle(this->x+xPos,this->y+(pos-1)*font->height,
                            width-xPos,font->height);
        draw->PopForeground();
      }
    }

    void Edit::UpdateDimensions()
    {
      if (model.Valid() &&
          model->GetLines()>0) {
        vAdjustment->SetDimension(height/font->height,
                                  model->GetLines());

        hAdjustment->SetDimension(width,32000);
        
      }
      else {
        hAdjustment->SetInvalid();
        vAdjustment->SetInvalid();
      }
    }

    /**
      Refresh each to be refreshed line.
    */
    void Edit::Update(OS::DrawInfo* draw)
    {
      if (!visible || !model.Valid()) {
        return;
      }

#if 0
        if (moveRegion) {
          draw->CopyArea(this->x,                                   /* sX     */
                         this->y+(mrStart-1)*font->height,          /* sY     */
                         width,                                     /* width  */
                         (mrEnd-mrStart+1)*font->height,            /* height */
                         this->x,                                   /* dX     */
                         this->y+(mrStart-1+mrCount)*font->height); /* dY     */
          moveRegion=false;
        }
#endif
      draw->PushClip(this->x,this->y,this->width,this->height);
      draw->PushFont(font);

      size_t x,y;
      bool   error;

      x=0;
      y=startLine->GetY();
      GetLine(y,workCursor);
      error=false;
      while (x<lines.size() && !error) {
        LineInfo info;

        if (!lines[x].drawn) {
          GetLineInfo(workCursor,info);
          PrintLine(draw,info,x+1,y);
          lines[x].drawn=true;

          // If the state of this line has changed, we mark the next line as
          // "to be drawn", too.
          if (lines[x].state!=info.state) {
            lines[x].state=info.state;
            if (x<lines.size()-1) {
              lines[x+1].drawn=false;
            }
          }
        }

        x++;
        y++;

        error=!workCursor->Down();
      }

      draw->PopFont();
      draw->PopClip();
    }

    bool Edit::CanScrollLeft() const
    {
      return hAdjustment->GetTop()>1;
    }

    bool Edit::CanScrollRight() const
    {
      return true;
    }

    void Edit::Page(long lines)
    {
      assert(lines!=0);

      if (lines<0) {
        vAdjustment->DecTop(std::abs(lines));
      }
      else if (lines>0) {
        vAdjustment->IncTop(lines);
      }
    }

    void Edit::ScrollSidewards(long pixel)
    {
      assert(pixel!=0);

      hAdjustment->IncTop(pixel);
    }

    /**
      Make the given cursor visible in the text.
    */
    void Edit::MakeCursorVisible(Cursor* cursor)
    {
      LineInfo info;

      /*
        Adjust vertical
      */

      if ((cursor->GetY()<GetFirstVisiblePos() &&
           GetFirstVisiblePos()-cursor->GetY()>vSize) ||
          (cursor->GetY()>GetLastVisiblePos() &&
           cursor->GetY()-GetLastVisiblePos()>vSize)) {
        /*
          Movement over more than one page, we 
          center the line on display
         */
        vAdjustment->CenterOn(cursor->GetY());
      }     
      else if (cursor->GetY()<GetFirstVisiblePos()) {
        /* Up */
        vAdjustment->SetTop(cursor->GetY());
      }
      else if (cursor->GetY()>GetLastVisiblePos()) {
        /* Down */
        vAdjustment->SetTop(cursor->GetY()-vSize+1);
      }
      
      /*
        Adjust horizontal
      */

      GetLine(cursor->GetY(),workCursor);
      GetLineInfo(workCursor,info);

      int xPos=-(hAdjustment->GetTop()-1);

      size_t a=0;
      while (a<info.pos.size() && info.pos[a]<cursor->GetX()) {
        xPos+=info.width[a];
        a++;
      }

      // Snap back to first==1 if we can show the cursor
      // without an horizontal offset.
      if (xPos-hScrollOffset*spaceWidth>=0 &&
          xPos+(hAdjustment->GetTop()-1)+hScrollOffset*spaceWidth<=width) {
        if (hAdjustment->GetTop()>1) {
          hAdjustment->SetTop(1);
        }
      }
      else if (xPos-(int)(hScrollOffset*spaceWidth)<0) {
        // Cursor is left of left edge
        hAdjustment->SetTop(xPos+(hAdjustment->GetTop()-1)-hScrollOffset*spaceWidth-width/2+1);
      }
      else if (xPos+hScrollOffset*spaceWidth>width) {
        // Cursor is right of right edge
        hAdjustment->SetTop(xPos+(hAdjustment->GetTop()-1)+hScrollOffset*spaceWidth-width/2+1);
      }
    }

    bool Edit::IsUnselected() const
    {
      return !IsSelected() && !IsMarkSet();
    }

    bool Edit::IsSelected() const
    {
      return markA!=NULL && markB!=NULL;
    }

    bool Edit::IsMarkSet() const
    {
      return markA!=NULL && markB==NULL;
    }

    Cursor* Edit::GetStartMark() const
    {
      assert(markA!=NULL);

      if (markB==NULL || markB->GetPos()>=markA->GetPos()) {
        return markA;
      }
      else {
        return markB;
      }
    }

    Cursor* Edit::GetEndMark() const
    {
      assert(IsSelected());

      if (markB->GetPos()>=markA->GetPos()) {
        return markB;
      }
      else {
        return markA;
      }
    }

    void Edit::SetStartMark(Cursor* cursor)
    {
      markA=model->GetOrCreateCursor(L"markA",0);
      markA->Goto(cursor);
    }

    void Edit::SetEndMark(Cursor* cursor)
    {
      size_t  a,b,c,d;
      Cursor  *markA,*markB;

      assert(GetStartMark()!=NULL);

      if (IsSelected()) {
        markA=GetStartMark();
        markB=GetEndMark();
        a=markA->GetY();
        b=markB->GetY();

        this->markB->Goto(cursor);

        markA=GetStartMark();
        markB=GetEndMark();
        c=markA->GetY();
        d=markB->GetY();

        // TODO: Make only a refresh of changed region
        if (a==c) {
          if (d>=b) {
            model->NotifyRedraw(b,d);
          }
          else {
            model->NotifyRedraw(d,b);
          }
        }
        else if (b==d) {
          if (c>=a) {
            model->NotifyRedraw(a,c);
          }
          else {
            model->NotifyRedraw(c,a);
          }
        }
        else {
          assert(false);
        }
      }
      else {
        OS::display->RegisterSelection(this,GetWindow());

        this->markB=model->GetOrCreateCursor(L"markB",0);
        this->markB->Goto(cursor);
        markA=GetStartMark();
        markB=GetEndMark();
        model->NotifyRedraw(markA->GetY(),markB->GetY());
      }
    }

    void Edit::ClearSelectionInternal()
    {
      size_t a,b;
      Cursor *markA,*markB;

      assert(IsSelected());

      markA=GetStartMark();
      markB=GetEndMark();
      a=markA->GetY();
      b=markB->GetY();
      model->FreeCursor(markB);
      model->FreeCursor(markA);
      this->markB=NULL;
      this->markA=NULL;
      model->NotifyRedraw(a,b);
    }

    /*
      PROCEDURE (e: Edit) ClearSelectionCallback*;

      (**
        Called by the display layer, when e.DeregisterGlobalSelection was
        successfull
      *)

      BEGIN
        IF ~e.stableSelection THEN
          e.ClearSelectionInternal;
        END;
      END ClearSelectionCallback;
    */

    /**
      Remove the current selection without modifying the text
    */
    void Edit::ClearSelection()
    {
      if (IsSelected()) {
        ClearSelectionInternal();
        if (!stableSelection) {
          //D.display.CancelSelection;
        }
      }
      else if (IsMarkSet()) {
        model->FreeCursor(markA);
        markA=NULL;
      }
    }

    void Edit::DeleteSelection()
    {
      if (IsSelected()) {
        if (model->DeleteArea(GetStartMark(),GetEndMark())) {
          ClearSelection();
        }
      }
    }

    void Edit::ShowBlock()
    {
      if (!showBlockCursor) {
        showBlockCursor=true;
        
        if (CanRefreshRedraw()) {       
          refreshLines.push_back(blockCursor->GetY());
        }
        else {
          refreshFull=true;
        }
        
        Redraw();
      }
    }

    void Edit::HideBlock()
    {
      if (showBlockCursor) {
        if (CanRefreshRedraw()) {       
          refreshLines.push_back(blockCursor->GetY());
        }
        else {
          refreshFull=true;
        }
        
        Redraw();
        
        showBlockCursor=false;
      }
    }

    bool Edit::SetModel(Base::Model* model)
    {
      if (this->model.Valid()) {
        // Free
        /*
        IF e.IsSelected() THEN
          D.display.CancelSelection;
        END;
        */

        blockCursor=NULL;
        markB=NULL;
        markA=NULL;
        tmp3Cursor=NULL;
        tmp2Cursor=NULL;
        tmpCursor=NULL;
        cursor=NULL;
        startLine=NULL;
        workCursor=NULL;
      }

      this->model=dynamic_cast<Text*>(model);

      if (this->model.Valid()) {
        //e.config=e.resolver(e);
        // Init
        cursor=this->model->GetOrCreateCursor(L"cursor",0);
        startLine=this->model->GetOrCreateCursor(L"startLine",0/*{T.cursorFix,T.cursorHidden}*/);
        workCursor=this->model->GetOrCreateCursor(L"workCursor",0/*{T.cursorFix,T.cursorHidden}*/);
        tmpCursor=this->model->GetOrCreateCursor(L"tmp",0/*{T.cursorHidden}*/);
        tmp2Cursor=this->model->GetOrCreateCursor(L"tmp2",0/*{T.cursorHidden}*/);
        tmp3Cursor=this->model->GetOrCreateCursor(L"tmp3",0/*{T.cursorHidden}*/);
        markA=this->model->GetCursor(L"markA");
        markB=this->model->GetCursor(L"markB");
        blockCursor=this->model->GetOrCreateCursor(L"BlockCursor",0/*{T.cursorHidden}*/);

        TriggerResolveConfigAction();
        TriggerPropertyChangedAction();
      }

      Scrollable::SetModel(this->model);

      return this->model.Valid();
    }

    void Edit::GetDimension(size_t& width, size_t& height)
    {
      if (model.Valid() && model->GetLines()>0) {
        width=32000;
        height=font->height*model->GetLines();
      }
      else {
        width=0;
        height=0;
      }
    }

    void Edit::Layout()
    {
      UpdateDimensions();

      Scrollable::Layout();
    }

    void Edit::Draw(OS::DrawInfo* draw,
                    int x, int y, size_t w, size_t h)
    {
      Scrollable::Draw(draw,x,y,w,h);

      if (!OIntersect(x,y,w,h)) {
        return;
      }

      /* --- */

      if (!Intersect(x,y,w,h)) {
        return;
      }

      if (model.Valid()) {
        if (refreshLines.empty() && !refreshMove && !refreshFull) {
            size_t start,end;

            start=GetFirstVisiblePos()+(y-this->y)/font->height;
            end=start+(h-1)/font->height;
            
            for (size_t i=start; i<=end; i++) {
              refreshLines.push_back(i);
            }
        }

        if (height/font->height!=vSize) {
          vSize=height/font->height;
          ResizeLines();

          MakeCursorVisible(cursor);
          refreshFull=true;
        }

        if (refreshMove) {
          if (refreshLastTop==std::numeric_limits<size_t>::max()) {
            // adjustment changed from invalid to valid
            refreshFull=true;
          }
          else if (refreshLastTop>=vAdjustment->GetTop()+vSize ||
                   vAdjustment->GetTop()>=refreshLastTop+vSize) {
            // Move is bigger than screen height       
            refreshFull=true;
          }
        }
      
        if (refreshFull) {
          // Resync startLine with vAdjustment->GetTop()          
          if (vAdjustment->GetTop()>GetFirstVisiblePos()) {
            GetLine(GetLastVisiblePos(),workCursor);
            while (vAdjustment->GetTop()>GetFirstVisiblePos() && workCursor->Down() && startLine->Down()) {
            }
          }
          else if (vAdjustment->GetTop()<GetFirstVisiblePos()) {
            GetLine(GetLastVisiblePos(),workCursor);
            while (vAdjustment->GetTop()<GetFirstVisiblePos() && startLine->Up() && workCursor->Up()) {
            }
          }
          
          SetDirtyVisibleRange(1,lines.size());
          Update(draw);

          if ((GetLastVisiblePos()-GetFirstVisiblePos()+1)*font->height<height) {
            draw->PushForeground(colorSheme->styles[ColorSheme::normalStyle].bg);
            draw->FillRectangle(this->x,
                                this->y+(GetLastVisiblePos()-GetFirstVisiblePos()+1)*font->height,
                                width,
                                height-(GetLastVisiblePos()-GetFirstVisiblePos()+1)*font->height);
            draw->PopForeground();
          }
          
          refreshFull=false;
          refreshMove=false;
          refreshLines.clear();
        }
        
        if (refreshMove) {
          // Resync startLine with vAdjustment->GetTop()          
          if (vAdjustment->GetTop()>GetFirstVisiblePos()) {
            GetLine(GetLastVisiblePos(),workCursor);
            while (vAdjustment->GetTop()>GetFirstVisiblePos() && workCursor->Down() && startLine->Down()) {
            }
          }
          else if (vAdjustment->GetTop()<GetFirstVisiblePos()) {
            GetLine(GetLastVisiblePos(),workCursor);
            while (vAdjustment->GetTop()<GetFirstVisiblePos() && startLine->Up() && workCursor->Up()) {
            }
          }
          
          if (vAdjustment->GetTop()>refreshLastTop) {
            draw->CopyArea(this->x,                                   
                           this->y+(vAdjustment->GetTop()-refreshLastTop)*font->height,
                           width,
                           (vSize-(vAdjustment->GetTop()-refreshLastTop))*font->height,
                           this->x,
                           this->y);
            SetDirtyVisibleRange(vSize-(vAdjustment->GetTop()-refreshLastTop)+1,vAdjustment->GetTop()-refreshLastTop);
          }
          else {
            draw->CopyArea(this->x,                                   
                           this->y,
                           width,
                           (vSize-(refreshLastTop-vAdjustment->GetTop()))*font->height,
                           this->x,
                           this->y+(refreshLastTop-vAdjustment->GetTop())*font->height);
            SetDirtyVisibleRange(1,refreshLastTop-vAdjustment->GetTop());
          }
          
          Update(draw);
          
          refreshMove=false;
        }  
        
        if (!refreshLines.empty()) {
          for (std::list<size_t>::const_iterator line=refreshLines.begin();
               line!=refreshLines.end();
               ++line) {          
            SetDirty(*line);
          }
          
          Update(draw);

          if ((GetLastVisiblePos()-GetFirstVisiblePos()+1)*font->height<height) {
            draw->PushForeground(colorSheme->styles[ColorSheme::normalStyle].bg);
            draw->FillRectangle(this->x,
                                this->y+(GetLastVisiblePos()-GetFirstVisiblePos()+1)*font->height,
                                width,
                                height-(GetLastVisiblePos()-GetFirstVisiblePos()+1)*font->height);
            draw->PopForeground();
          }
          
          refreshLines.clear();
        }
      }
      else {
        draw->PushForeground(colorSheme->styles[ColorSheme::normalStyle].bg);
        draw->FillRectangle(this->x,this->y,this->width,this->height);
        draw->PopForeground();
        
        refreshFull=false;
        refreshMove=false;
        refreshLines.clear();
      }
      
      // Reinitialize refresh tracking
      if (!vAdjustment->IsValid()) {
        refreshLastTop=std::numeric_limits<size_t>::max();
      }
      else {
        refreshLastTop=GetFirstVisiblePos();
      }
    }

    void Edit::DrawFocus()
    {
      // no code
    }

    void Edit::HideFocus()
    {
      // no code
    }

  bool Edit::HandleMouseEvent(const OS::MouseEvent& event)
  {
    /* It makes no sense to get the focus if we are currently not visible */
    if (!visible || !model.Valid() || !model->IsEnabled()) {
      return false;
    }

    /*
      When the left mousebutton gets pressed without any qualifier
      in the bounds of our button...
    */

    if (event.type==OS::MouseEvent::down && PointIsIn(event)) {
      OS::Window*   window;
      std::wstring  name,description;

      window=GetWindow();

      switch (event.button) {
      case OS::MouseEvent::button1:
        name=L"MouseButton1";
        break;
      case OS::MouseEvent::button2:
        name=L"MouseButton2";
        break;
      case OS::MouseEvent::button3:
        name=L"MouseButton3";
        break;
      case OS::MouseEvent::button4:
        name=L"MouseButton4";
        break;
      case OS::MouseEvent::button5:
        name=L"MouseButton5";
        break;
      case OS::MouseEvent::none:
        name=L"";
        break;
      }

      OS::display->KeyToKeyDescription(event.qualifier,name,description);


      /*if (window->IsTrippleClicked()) {
        description+=L"-trippleclick";
      }
      else */
      if (window->IsDoubleClicked()) {
        size_t px,py,tx,ty;

        if (PointIsIn(GetWindow()->GetLastButtonClickEvent()) &&
            CoordsToTextPos(window->GetLastButtonClickEvent().x,
                            window->GetLastButtonClickEvent().y,
                            px,py) &&
            CoordsToTextPos(event.x,event.y,tx,ty) &&
            px==tx && py==ty) {
          description+=L"-doubleclick";
        }
      }

      if (description==L"MouseButton1") {
        size_t x,y;

        // TODO: Make this a function!
        if (CoordsToTextPos(event.x,event.y,x,y)) {
          ClearSelection();
          if (cursor->Goto(x,y)) {
            SetStartMark(cursor);
          }
        }
        ExecutePost();

        return true;
      }
      else if (ExecuteFunctionKey(description)) {
        return true;
      }
    }
    else if (event.IsGrabEnd()) {
      if (IsMarkSet() || (IsSelected() && markA->GetPos()==markB->GetPos())) {
        ClearSelection();
      }
      Redraw();
      //Update();
      return true;
    }
    else if (event.type==OS::MouseEvent::move && event.IsGrabed()) {
      if (event.qualifier==OS::qualifierButton1) {
        // TODO: Make this a function
        if (IsMarkSet() || IsSelected()) {
          size_t x,y;

          if (CoordsToTextPos(event.x,event.y,x,y)) {
            if (cursor->Goto(x,y)) {
              SetEndMark(cursor);
            }
          }
        }
        ExecutePost();
      }
      else {
        std::wstring description;

        OS::display->KeyToKeyDescription(event.qualifier,L"MouseMotion",description);

        ExecuteFunctionKey(description);
      }

      return true;
    }

    return false;

  }

  bool Edit::HandleKeyEvent(const OS::KeyEvent& event)
  {
    if (event.type==OS::KeyEvent::down) {
      std::wstring name,description;

      event.GetName(name);
      OS::display->KeyToKeyDescription(event.qualifier,name,description);

      if (ExecuteFunctionKey(description)) {
        return true;
      }
      else {
        unsigned long qualifier;

        qualifier= event.qualifier & ~(OS::qualifierShiftLeft|OS::qualifierShiftRight|OS::qualifierShift|OS::qualifierCapsLock);
        if (!event.text.empty() && !(name==event.text && qualifier!=0) && event.text[0]>=32) {
          model->NewUndoGroup();
          DeleteSelection();
          model->Insert(cursor,event.text);
          MakeCursorVisible(cursor);
          ExecutePost();

          return true;
        }
        else {
          //std::cerr << "Unknown key '" << Base::WStringToString(description) << "'" << std::endl;
        }
      }
    }

    return false;
  }

    void Edit::Resync(Base::Model* model, const Base::ResyncMsg& msg)
    {
      if (model==this->model) {
        const Text::CursorMsg          *cursorMsg;
        const Text::RedrawMsg          *redrawMsg;
        const Text::DeleteMsg          *deleteMsg;
        const Text::InsertMsg          *insertMsg;
        const Text::PropertyChangedMsg *propertyChangedMsg;
        const Text::FilenameChangedMsg *filenameChangedMsg;
        const Text::TextChangedMsg     *textChangedMsg;

        if ((cursorMsg=dynamic_cast<const Text::CursorMsg*>(&msg))!=NULL) {
          if (cursorMsg->cursor==cursor) {
            MoveCursor(cursorMsg->oldPos.x,cursorMsg->oldPos.y,
                       cursorMsg->cursor->GetX(),cursorMsg->cursor->GetY());
            MakeCursorVisible(cursorMsg->cursor);
          }
        }
        else if ((redrawMsg=dynamic_cast<const Text::RedrawMsg*>(&msg))!=NULL) {
          if (CanRefreshRedraw()) {
            for (size_t i=redrawMsg->from; i<=redrawMsg->to; i++) {
              refreshLines.push_back(i);
            }
          }
          else {
            refreshFull=true;
          }
          
          Redraw();
        }
        else if ((deleteMsg=dynamic_cast<const Text::DeleteMsg*>(&msg))!=NULL) {
          refreshFull=true;
          Redraw();
          SetRelayout();
        }
        else if ((insertMsg=dynamic_cast<const Text::InsertMsg*>(&msg))!=NULL) {
          refreshFull=true;
          Redraw();
          SetRelayout();
        }
        else if ((propertyChangedMsg=dynamic_cast<const Text::PropertyChangedMsg*>(&msg))!=NULL) {
          TriggerPropertyChangedAction();
        }
        else if ((filenameChangedMsg=dynamic_cast<const Text::FilenameChangedMsg*>(&msg))!=NULL) {
          TriggerResolveConfigAction();
        }
        else if ((textChangedMsg=dynamic_cast<const Text::TextChangedMsg*>(&msg))!=NULL) {
          // ignored
        }
        else {
          refreshFull=true;
          Redraw();
          SetRelayout();
          
          if (visible) {
            MakeCursorVisible(cursor);
          }          
        }
      }
      else if (model==vAdjustment->GetTopModel()) {
        if (CanRefreshMove()) {
          refreshMove=true;
          SetRelayout();
          Redraw();
        }
        else {
          refreshFull=true;
          SetRelayout();
          Redraw();
        }
      }
      else if (model==hAdjustment->GetTopModel()) {
        refreshFull=true;
        Redraw();
        SetRelayout();
      }
    }

    void Edit::ExecuteFunction(Function function)
    {
      model->NewUndoGroup();
      function(*this);
      ExecutePost();
    }

    bool Edit::ExecuteFunctionKey(const std::wstring& key)
    {
      Function function;

      function=config->GetFunctionForKey(key);
      if (function!=NULL) {
        ExecuteFunction(function);
        return true;
      }
      else {
        return false;
      }
    }

    void Edit::ExecutePost()
    {
      TriggerFunctionExecutedAction();
    }
  }
}
