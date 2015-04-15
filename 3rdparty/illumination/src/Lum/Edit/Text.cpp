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

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fstream>

#include <Lum/Base/String.h>

#include <Lum/Edit/Text.h>

#include <Lum/Private/Config.h>

#if defined(HAVE_ICONV)
  #include <Lum/Private/IconvHelper.h>
#endif

namespace Lum {
  namespace Edit {

    Cursor::Cursor(const std::wstring& name, unsigned long flags)
    : name(name),flags(flags)
    {
      // no code
    }

    void Cursor::LinkText(Text* text)
    {
      this->text=text;

      pos=0;
      y=1;
      x=1;
    }

    void Cursor::UnlinkText()
    {
      text=NULL;
    }

    std::wstring Cursor::GetName() const
    {
      return name;
    }

    unsigned long Cursor::GetFlags() const
    {
      return flags;
    }

    /**
      Return the character the cursor is currently pointing to..
    */
    wchar_t Cursor::GetChar() const
    {
      return text->GetChar(GetPos());
    }

    /**
      Return the character before the character the cursor is currently pointing to.
    */
    wchar_t Cursor::GetPreviousChar() const
    {
      assert(GetPos()>0);

      return text->GetChar(GetPos()-1);
    }

    size_t Cursor::GetLineLength() const
    {
      size_t pos;
      size_t length=0;

      // Count characters before the current
      pos=GetPos();
      while (pos>0 && !text->IsEOL(pos-1)) {
        ++length;
        --pos;
      }

      // Count current and the following characters
      pos=GetPos();
      while (pos<text->GetSize() && !text->IsEOL(pos)) { // We skip the trailing EOT marker
        ++length;
        ++pos;
      }

      if (length>0 && text->GetEOLMode()==EOL_DOS && pos<text->GetSize()) {
        --length;
      }

      return length;
    }

    std::wstring Cursor::GetLine() const
    {
      size_t       pos;
      std::wstring buffer;

      buffer.reserve(1024);

      pos=GetPos();
      while (pos<text->GetSize() && !text->IsEOL(pos)) { // We skip the trailing EOT marker
        buffer.append(1,text->GetChar(pos));
        ++pos;
      }
      if (buffer.length()>0 && text->GetEOLMode()==EOL_DOS && pos<text->GetSize()) {
        buffer[buffer.length()-1]=L'\n';
      }
      else {
        buffer.append(1,L'\n');
      }

      return buffer;
    }

    size_t Cursor::GetX() const
    {
      return x;
    }

    size_t Cursor::GetY() const
    {
      return y;
    }

    size_t Cursor::GetPos() const
    {
      return pos;
    }

    void Cursor::Goto(CursorPtr cursor)
    {
      Text::CursorMsg msg;

      assert(cursor!=NULL);

      msg.cursor=this;
      msg.oldPos.x=GetX();
      msg.oldPos.y=GetY();
      msg.oldPos.pos=GetPos();

      pos=cursor->pos;
      y=cursor->y;
      x=cursor->x;

      text->Notify(msg);
    }

    bool Cursor::GotoPos(size_t pos)
    {
      Text::CursorMsg msg;

      if (GetPos()==pos) {
        return true;
      }

      if (pos>=text->GetSize()) {
        return false;
      }

      msg.cursor=this;
      msg.oldPos.x=GetX();
      msg.oldPos.y=GetY();
      msg.oldPos.pos=GetPos();

      if (pos>this->pos) {
        while (pos>this->pos) {
          if (text->IsEOL(this->pos)) {
            ++y;
            x=0;
          }
          x++;
          this->pos++;
        }
      }
      else if (this->pos>pos) {
        while (this->pos>pos) {
          --this->pos;
          if (text->IsEOL(this->pos)) {
            --y;
          }
        }
        // Recalculate X-position
        x=1;
        while (pos>0 && !text->IsEOL(pos-1)) {
          --pos;
          ++x;
        }
      }

      text->Notify(msg);

      return true;
    }

    bool Cursor::Goto(size_t x, size_t y)
    {
      size_t          length;
      Text::CursorMsg msg;

      if (x==0 || y==0 || y>text->GetLines()) {
        return false;
      }

      if (x==GetX() && y==GetY()) {
        return true;
      }

      msg.cursor=this;
      msg.oldPos.x=GetX();
      msg.oldPos.y=GetY();
      msg.oldPos.pos=GetPos();

      size_t pos,currentLine;

      pos=GetPos();
      currentLine=GetY();

      // Move cursor to the start of the requested line
      if (y>currentLine) {
        while (y>currentLine) {
          if (pos>=text->GetChars()) {
            return false;
          }

          if (text->IsEOL(pos)) {
            ++currentLine;
          }
          ++pos;
        }
      }
      else if (currentLine>y) {
        while (currentLine>y) {
          if (pos==0) {
            return false;
          }

          --pos;
          if (text->IsEOL(pos)) {
            --currentLine;
          }
        }
        // We are in the correct line, but still not at the start of it
        while (pos>0 && !text->IsEOL(pos-1)) {
          --pos;
        }
      }
      else {
        // We are in the correct line, but possibly not at the start of it
        while (pos>0 && !text->IsEOL(pos-1)) {
          --pos;
        }
      }

      this->pos=pos;
      this->y=y;
      this->x=x;

      length=GetLineLength();
      if (x<=length) {
        this->pos+=x-1;
      }
      else if (this->pos+length<text->GetChars()) {
        this->pos+=length;
      }
      else {
        this->pos+=length-1;
      }

      text->Notify(msg);
      return true;
    }

    bool Cursor::GotoColumn(size_t column)
    {
      return Goto(column,GetY());
    }

    bool Cursor::GotoRow(size_t row)
    {
      // This will always succeed
      return Goto(GetX(),row);
    }

    bool Cursor::GotoNextOccurence(const std::wstring& string, bool caseMatch)
    {
      size_t   pos,chars,length;

      pos=GetPos();
      chars=text->GetSize();
      length=string.length();

      if (length==0) {
        return false;
      }

      if (pos+length>=chars) {
        return false;
      }

      pos++;

      // For speed, we duplicate the code
      if (caseMatch) {
        while (pos+length<chars) {
          if (text->GetChar(pos)==string[0]) {
            size_t x=1;
            size_t a=pos;
            size_t b=pos+1;

            while (x<length && text->GetChar(b)==string[x]) {
              x++;
              b++;
            }

            if (x==length) {
              GotoPos(a);
              return true;
            }
          }
          pos++;
        }
      }
      else {
        std::wstring tmp=string;

        for (size_t i=0; i<string.length(); i++) {
          tmp=toupper(string[i]);
        }

        while (pos+length<chars) {
          if (toupper(text->GetChar(pos))==string[0]) {
            size_t x=1;
            size_t a=pos;
            size_t b=pos+1;

            while (x<length && toupper(text->GetChar(b))==string[x]) {
              x++;
              b++;
            }

            if (x==length) {
              GotoPos(a);
              return true;
            }
          }
          pos++;
        }
      }

      return false;
    }

    bool Cursor::Move(int offset)
    {
      return GotoPos(GetPos()+offset);
    }

    bool Cursor::Back()
    {
      return Move(-1);
    }

    bool Cursor::Forward()
    {
      return Move(1);
    }

    bool Cursor::Left()
    {
      return Goto(GetX()-1,GetY());
    }

    bool Cursor::Right()
    {
      return Goto(GetX()+1,GetY());
    }

    bool Cursor::Up()
    {
      return Goto(GetX(),GetY()-1);
    }

    bool Cursor::Down()
    {
      return Goto(GetX(),GetY()+1);
    }

    Buffer::Buffer()
    {
        // no code
    }

    Buffer::Buffer(const std::wstring& text,EOLMode eolMode)
    : text(text),eolMode(eolMode)
    {
      // No code
    }

    void Buffer::SetText(const std::wstring& text, EOLMode eolMode)
    {
      this->text=text;
      this->eolMode=eolMode;
    }

    const std::wstring& Buffer::GetText() const
    {
      return text;
    }

    const std::wstring Buffer::GetText(EOLMode eolMode) const
    {
      if (this->eolMode==eolMode) {
        return text;
      }
      else if (this->eolMode==EOL_Unix && eolMode==EOL_DOS) {
        size_t       count=0,pos;
        std::wstring tmp;

        for (size_t i=0; i<text.length(); ++i) {
          if (text[i]==L'\n') {
            ++count;
          }
        }

        tmp.resize(text.length()+count);

        pos=0;
        for (size_t i=0; i<text.length(); i++) {
          if (text[i]==L'\n') {
            tmp[pos]=L'\r';
            ++pos;
            tmp[pos]=L'\n';
          }
          else {
            tmp[pos]=text[i];
          }
          ++pos;
        }

        return tmp;
      }
      else if (this->eolMode==EOL_DOS && eolMode==EOL_Unix) {
        size_t       count=0,pos,i;
        std::wstring tmp;

        for (size_t i=0; i<text.length()-1; ++i) {
          if (text[i]==L'\r' && text[i+1]==L'\n') {
            ++count;
          }
        }

        tmp.resize(text.length()-count);

        pos=0;
        i=0;
        while (i<text.length()) {
          if (text[i]==L'\r' && i+1<text.length() && text[i+1]==L'\n') {
            tmp[pos]=L'\n';
            ++i;
          }
          else {
            tmp[pos]=text[i];
          }
          ++pos;
          ++i;
        }

        return tmp;
      }
      else if (this->eolMode==EOL_Unix && eolMode==EOL_Mac) {
        std::wstring tmp;

        tmp.resize(text.length());

        for (size_t i=0; i<text.length(); ++i) {
          if (text[i]==L'\n') {
            tmp[i]=L'\r';
          }
          else {
            tmp[i]=text[i];
          }
        }

        return tmp;
      }
      else if (this->eolMode==EOL_Mac && eolMode==EOL_Unix) {
        std::wstring tmp;

        tmp.resize(text.length());

        for (size_t i=0; i<text.length(); ++i) {
          if (text[i]==L'\r') {
            tmp[i]=L'\n';
          }
          else {
            tmp[i]=text[i];
          }
        }

        return tmp;
      }
      else if (this->eolMode==EOL_DOS && eolMode==EOL_Mac) {
        size_t       count=0,pos,i;
        std::wstring tmp;

        for (size_t i=0; i<text.length()-1; ++i) {
          if (text[i]==L'\r' && text[i+1]==L'\n') {
            ++count;
          }
        }

        tmp.resize(text.length()-count);

        pos=0;
        i=0;
        while (i<text.length()) {
          if (text[i]==L'\r' && i+1<text.length() && text[i+1]==L'\n') {
            tmp[pos]=L'\r';
            ++i;
          }
          else {
            tmp[pos]=text[i];
          }
          ++pos;
          ++i;
        }

        return tmp;
      }
      else if (this->eolMode==EOL_Mac && eolMode==EOL_DOS) {
        size_t       count=0,pos;
        std::wstring tmp;

        for (size_t i=0; i<text.length(); ++i) {
          if (text[i]==L'\r') {
            ++count;
          }
        }

        tmp.resize(text.length()+count);

        pos=0;
        for (size_t i=0; i<text.length(); i++) {
          if (text[i]==L'\r') {
            tmp[pos]=L'\r';
            ++pos;
            tmp[pos]=L'\n';
          }
          else {
            tmp[pos]=text[i];
          }
          ++pos;
        }

        return tmp;
      }
      else {
        return L"";
      }
    }

    State::State(Text* text)
     : text(text)
    {
      // no code
    }

    /**
      (Re)Initialize the state
     */
    void State::Init()
    {
      row=1;
      memory.clear();
      state.Reset();
      highMark=0;
      cursor=text->GetOrCreateCursor(L"state",Cursor::cursorFix|Cursor::cursorHidden);
      RememberState(1,state);
    }

    /**
       Store the given state for the given line, if line=highmark+1
     */
    void State::RememberState(size_t line, const StateInfo& state)
    {
      if (line<highMark) {
        return;
      }

      if (line>highMark) {
        assert(highMark+1==line);
        if (line>=memory.size()) {
          if (memory.size()==0) {
            memory.resize(std::max(line,(size_t)10000u));
          }
          else {
            memory.resize(std::max(line,memory.size()*10));
          }
        }

        memory[line]=state;
        highMark=line;
      }
    }

    /**
      Move the highmark back, thus droping state information for everything beyond
      the high mark.
     */
    void State::ForgetStateBehindLine(size_t line)
    {
      if (line<1) {
        line=1;
      }
      
      if (line<highMark) {
        cursor->Goto(1,line-1);
        row=line;
        state=memory[line];
        highMark=line;
      }
    }

    /**
       Tell the state machine to move to the given line and calculate its state.
       State will be either used by using the meory variable to get already
       calculated state for a line or by calculating the new state by iterating
       over the text and parsing it (from highmark up to line).
     */
    StateInfo State::MoveToLine(size_t line)
    {
      if (line<1) {
        line=1;
      }

      // See, if we can jump to a position close before the requested line
      if (line==1) {
        cursor->GotoPos(0);
        row=line;
        state.Reset(); // reinitialize
        RememberState(1,state);
        return state;
      }
      else if (line<=highMark) { // go back
        cursor->Goto(1,line-1);
        row=line;
        state=memory[line];
        return state;
      }
      else { // go forward
        std::wstring text;

        cursor->Goto(1,highMark-1);
        row=highMark;
        state=memory[highMark];

        // Now walk the rest line by line
        while (line>row) {

          //RememberState(row,state);

          cursor->Goto(1,row);

          row++;

          text=cursor->GetLine();

          size_t x;

          x=0;
          while (x<text.length()) {
            size_t   length=1;

            this->text->syntax->ParseToken(text,x,length,state);
            x+=length;
          }

          RememberState(row,state);
        }
      }
      
      return state;
    }


    Text::UndoInsert::UndoInsert(size_t pos, size_t count)
    : pos(pos),count(count)
    {
      // no code
    }

    Text::UndoDelete::UndoDelete(size_t pos, const std::wstring& text)
    : pos(pos),text(text)
    {
      // no code
    }

    Text::UndoGroup::~UndoGroup()
    {
      std::list<Undo*>::iterator iter;

      iter=entries.begin();
      while (iter!=entries.end()) {
        delete *iter;

        ++iter;
      }

      entries.clear();
    }

    Text::Text()
    : undoMode(undoNormal),
      changed(false),
      readOnly(false),
      buffer(NULL),
      bufferSize(0),
      fillSize(0),
      gapPos(0),
      state(this),
      syntax(syntaxRepository->Get(L"Standard"))
    {
      Clear();
    }

    Text::~Text()
    {
      std::list<Cursor*>::iterator cursor;

      cursor=cursors.begin();
      while (cursor!=cursors.end()) {
        delete *cursor;

        ++cursor;
      }

      undoStack.clear();
      redoStack.clear();

      free(buffer);
    }

    void Text::ReserveBuffer(size_t size)
    {
      size_t old=bufferSize;

      if (size>bufferSize) {
        bufferSize=((size / 65536)+1)*65536;
        buffer=(wchar_t*)realloc(buffer,bufferSize*sizeof(wchar_t));

        if (gapPos==old) {
          // nothing to do, gap is still behind buffer
          gapPos=bufferSize;

        }
        else {
          // Move right side of all buffer to the end of the new buffer
          memmove((void*)&buffer[gapPos+bufferSize-fillSize],(void*)&buffer[gapPos+old-fillSize],(old-fillSize-gapPos)*sizeof(wchar_t));
        }
      }
    }

    void Text::SetBuffer(const std::wstring& text)
    {
      SetBuffer(text.data(),text.length());
    }

    void Text::SetBuffer(const wchar_t *text, size_t length)
    {
      ReserveBuffer(length+1);
      memcpy((void*)buffer,text,length*sizeof(wchar_t));
      buffer[length*sizeof(wchar_t)]=L'\0';
      fillSize=length+1;
      gapPos=bufferSize;
    }

    void Text::MoveGap(size_t pos, size_t length)
    {
      ReserveBuffer(fillSize+length);

      if (gapPos==pos) {
        return;
      }

      if (gapPos==bufferSize) {
        // Move everything behind gapPos to right side
        memmove((void*)&buffer[pos+(bufferSize-fillSize)],
                (void*)&buffer[pos],
                (fillSize-pos)*sizeof(wchar_t));
        gapPos=pos;
      }
      else if (pos>gapPos) {
        // Move some stuff from the right side to the left side
        memmove((void*)&buffer[gapPos],
                (void*)&buffer[gapPos+(bufferSize-fillSize)],
                (pos-gapPos)*sizeof(wchar_t));
        gapPos=pos;
      }
      else if (pos<gapPos) {
        // Move some stuff from the left side to the right side
        memmove((void*)&buffer[pos+(bufferSize-fillSize)],
                (void*)&buffer[pos],
                (gapPos-pos)*sizeof(wchar_t));
        gapPos=pos;
      }
      else {
        // nothing to do
      }
    }

    /**
      Insert a text as the given position.

      NOTE
      This method to not correct higher level datastructures like cursors
      neither does it fix text size counters (like line count) besides the text size.
    */
    void Text::InsertGap(size_t pos, const wchar_t* text, size_t length)
    {
      MoveGap(pos,length);

      for (size_t x=0; x<length; x++) {
        buffer[gapPos+x]=text[x];
      }
      gapPos+=length;
      fillSize+=length;
    }

    void Text::InsertGap(size_t pos, const std::wstring& text)
    {
      InsertGap(pos,text.data(),text.length());
    }

    void Text::NotifyRedraw(size_t a, size_t b)
    {
      RedrawMsg redrawMsg;

      redrawMsg.from=a;
      redrawMsg.to=b;

      Notify(redrawMsg);
    }

    void Text::NotifyDelete(size_t from, size_t count)
    {
      DeleteMsg deleteMsg;

      deleteMsg.from=from;
      deleteMsg.count=count;
      Notify(deleteMsg);
    }

    void Text::NotifyInsert(size_t from, size_t count)
    {
      InsertMsg insertMsg;

      insertMsg.from=from;
      insertMsg.count=count;
      Notify(insertMsg);
    }

    void Text::NotifyPropertyChanged()
    {
      PropertyChangedMsg msg;

      Notify(msg);
    }

    void Text::NotifyTextChanged()
    {
      TextChangedMsg msg;

      Notify(msg);
    }

    void Text::NotifyFilenameChanged()
    {
      FilenameChangedMsg msg;

      Notify(msg);
    }

    void Text::DetectEOLMode()
    {
      size_t unx=0;
      size_t dos=0;
      size_t mac=0;
      bool   cr=false;

      for (size_t x=0; x<10000 && x<GetSize(); x++) {
        switch (GetChar(x)) {
        case L'\r':
          if (cr) {
            mac++;
          }
          cr=true;
          break;
        case L'\n':
          if (cr) {
            dos++;
          }
          else {
            unx++;
          }
          cr=false;
          break;
        default:
          cr=false;
          break;
        }
      }

      if (dos>mac && dos>unx) {
        eolMode=EOL_DOS;
      }
      else if (mac>dos && mac>unx) {
        eolMode=EOL_Mac;
      }
      else if (unx>mac && unx>dos ) {
        eolMode=EOL_Unix;
      }
      else {
#if defined(__WIN32__) || defined(WIN32)
        eolMode=EOL_DOS;
#else
        eolMode=EOL_Unix;
#endif
      }
    }

    void Text::CountText(const std::wstring& text, size_t &firstX, size_t &lastX, size_t &lines)
    {
      bool cr=false;

      firstX=0;
      lastX=0;
      lines=0;

      for (size_t x=0; x<text.length(); x++) {
        wchar_t character=text[x];

        switch (eolMode) {
        case EOL_DOS:
          if (character==L'\n') {
            if (cr) {
              // Substract the \r again
              if (lines>0) {
                lastX--;
              }
              else {
                firstX--;
              }

              lines++;
              lastX=0;
              cr=false;
            }
            else {
              if (lines>0) {
                lastX++;
              }
              else {
                firstX++;
              }
            }
          }
          else if (character==L'\r') {
            cr=true;

            // We count the \r as normal character and
            // subtract it later, if the next character is a \n
            if (lines>0) {
              lastX++;
            }
            else {
              firstX++;
            }
          }
          else {
            cr=false;
            if (lines>0) {
              lastX++;
            }
            else {
              firstX++;
            }
          }
          break;
        case EOL_Mac:
          if (character==L'\r') {
            lines++;
            lastX=0;
          }
          else if (lines>0) {
            lastX++;
          }
          else {
            firstX++;
          }
          break;
        case EOL_Unix:
          if (character==L'\n') {
            lines++;
            lastX=0;
          }
          else if (lines>0) {
            lastX++;
          }
          else {
            firstX++;
          }

          break;
        }
      }
    }

    void Text::CountLines()
    {
      bool eol=false;

      lines=1;
      for (size_t x=0; x<GetSize(); x++) {
        if (eol) {
          lines++;
        }
        eol=IsEOL(x);
      }
    }

    void Text::ResetCursors()
    {
      std::list<CursorPtr>::const_iterator iter;

      iter=cursors.begin();
      while (iter!=cursors.end()) {
        (*iter)->UnlinkText();
        (*iter)->LinkText(this);

        ++iter;
      }
    }

    /**
      Return the file name of the buffer.
    */
    std::wstring Text::GetFileName() const
    {
      return fileName;
    }

    /**
      Return true, if the buffer (and the underlying file) cannot be modified.
    */
    bool Text::IsReadOnly() const
    {
      return readOnly;
    }

    /**
      Returns true, if the buffer has changed since last load or stored.
    */
    bool Text::HasChanged() const
    {
      return changed;
    }

    /**
      Return the number of text lines.
    */
    size_t Text::GetLines() const
    {
      return lines;
    }

    /**
      Return the number of characters in the text.
    */
    size_t Text::GetSize() const
    {
      return fillSize-1;
    }

    /**
      This method returns the number of characters in the text buffer
      \em including a unvisible terminating EOT charcater. For getting the
      \em real buffer size substract 1 from the method result or call Text::GetSize().
    */
    size_t Text::GetChars() const
    {
      return fillSize;
    }

    EOLMode Text::GetEOLMode() const
    {
      return eolMode;
    }

    std::wstring Text::GetEOLToken() const
    {
      switch (eolMode) {
      case EOL_DOS:
        return L"\r\n";
      case EOL_Mac:
        return L"\r";
      case EOL_Unix:
        return L"\n";
      }

#if defined(__WIN32__) || defined(WIN32)
      return L"\r\n";
#else
      return L"\n";
#endif
    }

    void Text::SetReadOnly(bool readOnly)
    {
      if (this->readOnly!=readOnly) {
        this->readOnly=readOnly;
        NotifyPropertyChanged();
      }
    }

    void Text::SetChanged(bool changed)
    {
      if (this->changed!=changed) {
        this->changed=changed;
        NotifyPropertyChanged();
      }
    }

    void Text::SetFileName(const std::wstring& fileName/* syntax : S.Syntax*/)
    {
      this->fileName=fileName;
      //t.syntax:=syntax;

      NotifyPropertyChanged();
      NotifyFilenameChanged();
    }

    void Text::SetSyntax(Syntax* syntax)
    {
      this->syntax=syntax;

      state.Init();
    }

    Cursor* Text::CreateCursor(const std::wstring& name, unsigned long flags)
    {
      Cursor *cursor;

      cursor=new Cursor(name,flags);

      cursors.push_back(cursor);

      cursor->LinkText(this);

      return cursor;
    }

    Cursor* Text::GetCursor(const std::wstring& name) const
    {
      std::list<Cursor*>::const_iterator iter;

      iter=cursors.begin();
      while (iter!=cursors.end()) {
        if ((*iter)->GetName()==name) {
          return *iter;
        }

        ++iter;
      }

      return NULL;
    }

    Cursor* Text::GetOrCreateCursor(const std::wstring& name, unsigned long flags)
    {
      Cursor *cursor=GetCursor(name);

      if (cursor==NULL) {
        cursor=CreateCursor(name,flags);
      }

      return cursor;
    }

    void Text::FreeCursor(Cursor* cursor)
    {
      cursor->UnlinkText();

      cursors.remove(cursor);
    }

    void Text::SetText(const wchar_t* text, size_t length)
    {
      undoStack.clear();
      redoStack.clear();

      SetBuffer(text,length);

      DetectEOLMode();

      CountLines();
      ResetCursors();
      fileName=L"";
      SetChanged(false);
      SetReadOnly(false);
      //t.syntax:=syntax;

      state.Init();

      NotifyTextChanged();
      NotifyPropertyChanged();
      NotifyFilenameChanged();
      Notify();
    }

    void Text::SetText(const std::wstring& text)
    {
      SetText(text.data(),text.length());
    }

    std::wstring Text::GetText() const
    {
      std::wstring result;

      result.reserve(GetSize());

      if (gapPos==0) {
        // Gap before text
        result.assign(&buffer[gapPos],GetSize());
      }
      else if (gapPos>=GetSize()) {
        // Gap after text
        result.assign(buffer,GetSize());
      }
      else {
        result.assign(buffer,gapPos);
        result.append(&buffer[gapPos+bufferSize-fillSize],GetSize()-gapPos);
      }

      return result;
    }

    void Text::Clear()
    {
      SetText(L"",0/*,Standard.syntax*/);
    }

    /**
      Reads the given file into the text.
    */

    Base::Status Text::Load(const std::wstring& fileName)
    {
      Base::Status  status;

#if defined (HAVE_ICONV)
      const size_t  bufferSize=65536;

      std::ifstream file;
      iconv_t       cd;
      size_t        size;
      char          tmp[bufferSize];
      size_t        insize=0;

      wchar_t       *tmpBuffer=NULL;
      size_t        tmpBufferSize=0;
      size_t        tmpFillSize=0;

      cd=iconv_open (ICONV_WCHAR_T,ICONV_EMPTY_STRING);
      if (cd==(iconv_t)-1) {
        status.SetToCurrentErrno();
        return status;
      }

      file.open(Base::WStringToString(fileName).c_str(),std::ios::in|std::ios::binary);

      if (!file) {
        iconv_close(cd);
        status.SetToCurrentErrno();
        return status;
      }

      file.seekg(0,std::ios::end);
      size=file.tellg();
      file.seekg(0,std::ios::beg);

      tmpBuffer=(wchar_t*)malloc(size*sizeof(wchar_t));

      while (size>0) {
        wchar_t buffer[bufferSize+1];
        wchar_t *bufferPtr=buffer;
        char    *inptr=tmp;
        size_t  avail=sizeof(buffer)*sizeof(wchar_t);
        size_t  nread,nconv,readSize;

        readSize=sizeof(tmp)-insize;
        if (readSize>size) {
          readSize=size;
        }

        file.read(tmp+insize,readSize);
        nread=file.gcount();
        size-=nread;
        insize+=nread;

        nconv=iconv(cd,(ICONV_CONST char**)&inptr,&insize,(char**)&bufferPtr, &avail);
        if (nconv==(size_t)-1) {
          if (errno==EINVAL) {
            // Some bytes are missing. Just reuse the unconverted part the next round
            memmove(tmp,inptr,insize);
          }
          else { // Some strange effect under Mac OS X
            status.SetToCurrentErrno();
          }
        }

        avail=avail/sizeof(wchar_t);

        if (tmpFillSize+sizeof(buffer)-avail>tmpBufferSize) {
          tmpBufferSize=(tmpBufferSize+bufferSize*2);
          tmpBuffer=(wchar_t*)realloc(tmpBuffer,tmpBufferSize*sizeof(wchar_t));
          assert(tmpBuffer!=NULL);
        }
        memcpy((void*)&tmpBuffer[tmpFillSize],(void*)buffer,(sizeof(buffer)-avail)*sizeof(wchar_t));
        tmpFillSize+=sizeof(buffer)-avail;
      }

      file.close();

      if (!file) {
        status.SetToCurrentErrno();
        return status;
      }

      if (iconv_close(cd)!=0) {
        status.SetToCurrentErrno();
        return status;
      }

      if (!status) {
        return status;
      }

      undoStack.clear();
      redoStack.clear();

      free(buffer);

      // remove potential byte order marks
      if (sizeof(wchar_t)==4) {
        // strip off potential BOM if ICONV_WCHAR_T is UTF-32
        if (tmpBuffer[0]==0xfeff) {
          tmpFillSize--;
          memmove(tmpBuffer,tmpBuffer+1,tmpFillSize*sizeof(wchar_t));
        }
      }
      else if (sizeof(wchar_t)==2) {
        // strip off potential BOM if ICONV_WCHAR_T is UTF-16
        if (tmpBuffer[0]==0xfeff || tmpBuffer[0]==0xfffe) {
          tmpFillSize--;
          memmove(tmpBuffer,tmpBuffer+1,tmpFillSize*sizeof(wchar_t));
        }
      }

      buffer=tmpBuffer;
      this->bufferSize=tmpBufferSize;
      fillSize=tmpFillSize;
      gapPos=this->bufferSize;

      InsertGap(GetChars(),L"\0",1);

      SetChanged(false);
      SetFileName(fileName);

      NotifyTextChanged();

      DetectEOLMode();

      CountLines();
      ResetCursors();
      state.Init();

      file.open(Base::WStringToString(fileName).c_str(),std::ios::out|std::ios::binary);
      SetReadOnly(!file);
      file.close();

      Notify();
#endif
      return status;
    }

    /**
      Save the text in the given text to a file.
    */
    Base::Status Text::Save(const std::wstring& fileName, bool stripTrailingSpace)
    {
      Base::Status  status;

#if defined (HAVE_ICONV)
      std::ofstream file;
      size_t        y;
      std::wstring  line;
      iconv_t       cd;

      cd=iconv_open (ICONV_WCHAR_T,ICONV_EMPTY_STRING);
      if (cd==(iconv_t)-1) {
        status.SetToCurrentErrno();
        return status;
      }

      file.open(Base::WStringToString(fileName).c_str(),std::ios::out|std::ios::trunc|std::ios::binary);

      if (!file) {
        iconv_close(cd);
        status.SetToCurrentErrno();
        return status;
      }

      line.clear();
      for (y=0; y<fillSize-1; y++) {
        size_t pos=y;

        if (pos>=gapPos) {
          pos+=(bufferSize-fillSize);
        }

        if (buffer[pos]==L'\n') {
          if (stripTrailingSpace) {
            while (line.length()>0 && line[line.length()-1]==L' ') {
              line.erase(line.length()-1);
            }
          }
          file << Lum::Base::WStringToString(line) << std::endl;
          line.clear();
        }
        else {
          line.append(1,buffer[pos]);
        }
      }

      if (stripTrailingSpace) {
        while (line.length()>0 && line[line.length()-1]==L' ') {
          line.erase(line.length()-1);
        }
      }
      file << Lum::Base::WStringToString(line);

      file.close();

      if (iconv_close(cd)!=0) {
        status.SetToCurrentErrno();
        return status;
      }

      SetChanged(false);
#endif
      return status;
    }

    void Text::PushUndo(Undo* undo)
    {
      undoStack.front().entries.push_front(undo);
      if (undoMode==undoNormal) {
        redoStack.clear();
      }
    }

    void Text::PushRedo(Undo* undo)
    {
      redoStack.front().entries.push_front(undo);
    }

    void Text::NewUndoGroup()
    {
      if (undoStack.size()==0 || undoStack.front().entries.size()>0) {
        UndoGroup group;

        group.changed=changed;
        undoStack.push_front(group);
      }
    }

    void Text::NewRedoGroup()
    {
      if (redoStack.size()==0 || redoStack.front().entries.size()>0) {
        UndoGroup group;

        group.changed=changed;
        redoStack.push_front(group);
      }
    }

    /**
      Make a undo with the help of the two handed cursors. We expect at least
      cursor to be valid.
    */
    bool Text::UndoStep(Cursor* cursor, Cursor* cursor2)
    {
      if (undoStack.size()>0 && undoStack.front().entries.size()==0) {
        undoStack.erase(undoStack.begin());
      }

      if (undoStack.size()==0){
        return false;
      }

      undoMode=undoUndo;
      NewRedoGroup();

      std::list<Undo*>::iterator iter;

      iter=undoStack.front().entries.begin();
      while (iter!=undoStack.front().entries.end()) {
        bool       res;
        UndoInsert *insert;
        UndoDelete *del;

        res=false;

        if ((insert=dynamic_cast<UndoInsert*>(*iter))!=NULL) {
          // Delete inserted area
          if (cursor->GotoPos(insert->pos)) {
            cursor2->Goto(cursor);
            if (cursor2->GotoPos(insert->pos+insert->count)) {
              res=DeleteArea(cursor,cursor2);
            }
            else {
              // TODO: Restaurate position of cursor
            }
          }
        }
        else if ((del=dynamic_cast<UndoDelete*>(*iter))!=NULL) {
         if (cursor->GotoPos(del->pos)) {
           res=Insert(cursor,del->text);
          }
        }

        if (res) {
          ++iter;
        }
        else {
          iter=undoStack.front().entries.end();
        }
      }


      SetChanged(undoStack.front().changed);

      undoStack.erase(undoStack.begin());

      undoMode=undoNormal;

      return true;
    }

    /**
      Make a undo with the help of the two handed cursors. We expect at least
      cursor to be valid.
    */
    bool Text::RedoStep(Cursor* cursor, Cursor* cursor2)
    {
      if (redoStack.size()==0){
        return false;
      }

      undoMode=undoRedo;

      std::list<Undo*>::iterator iter;

      iter=redoStack.front().entries.begin();
      while (iter!=redoStack.front().entries.end()) {
        bool       res;
        Undo       *undo;
        UndoInsert *insert;
        UndoDelete *del;

        res=false;

        undo=*iter;
        if ((insert=dynamic_cast<UndoInsert*>(undo))!=NULL) {
          // Delete inserted area
          if (cursor->GotoPos(insert->pos)) {
            cursor2->Goto(cursor);
            if (cursor2->GotoPos(insert->pos+insert->count)) {
              res=DeleteArea(cursor,cursor2);
            }
            else {
              // TODO: Restaurate position of cursor
            }
          }
        }
        else if ((del=dynamic_cast<UndoDelete*>(undo))!=NULL) {
          if (cursor->GotoPos(del->pos)) {
            res=Insert(cursor,del->text);
          }
        }

        if (res) {
          iter++;
        }
        else {
          iter=redoStack.front().entries.end();
        }
      }

      SetChanged(redoStack.front().changed);

      redoStack.erase(redoStack.begin());

      undoMode=undoNormal;

      return true;
    }

    wchar_t Text::GetChar(size_t pos) const
    {
      if (buffer==NULL) {
        return L'\0';
      }
      else if (pos>=gapPos) {
        return buffer[pos+bufferSize-fillSize];
      }
      else {
        return buffer[pos];
      }
    }

    bool Text::IsEOL(size_t pos) const
    {
      switch (eolMode) {
      case EOL_DOS:
        return GetChar(pos)==L'\n' && pos>0 && GetChar(pos-1)==L'\r';
      case EOL_Mac:
        return GetChar(pos)==L'\r';
      case EOL_Unix:
        return GetChar(pos)==L'\n';
      }

      return false;
    }

    bool Text::Insert(Cursor* cursor, const std::wstring& string)
    {
      if (string.empty()) {
        return true;
      }

      size_t                       oldY,oldPos,firstX,lastX,lines;
      std::list<Cursor*>::iterator c;
      std::wstring                 text;

      if (IsReadOnly()) {
        return false;
      }

      /*
        Check for insertion after end of line.
      */

      text=string;
      if (cursor->GetX()>cursor->GetLineLength()) {
        text.insert(0,cursor->GetX()-cursor->GetLineLength()-1,L' ');
        // Fix cursor x position to the real insertion point, so we do not need
        // some extra code, when we later correct cursor positions
        cursor->x=cursor->GetLineLength()+1;
      }

      /*
        Calculate messure of run
        (size of first and last line, number of lines over all)
      */

      CountText(text,firstX,lastX,lines);

      InsertGap(cursor->GetPos(),text);

      this->lines+=lines;

      /*
        Store old position
      */

      oldPos=cursor->GetPos();
      oldY=cursor->GetY();

      /*
        Update global data and all cursor behind the insertion point using
        the calculated messurements.
      */

      c=cursors.begin();
      while (c!=cursors.end()) {
        size_t oldCY=(*c)->GetY();
        if ((*c)->GetPos()>=oldPos) {
          if ((*c)->GetY()==oldY) {
            if (lines==0) {
              (*c)->x+=firstX;
            }
            else {
              (*c)->x=lastX+1;
            }
          }
          
          (*c)->pos+=text.length();
          (*c)->y+=lines;
        }

        if (Cursor::cursorFix & (*c)->GetFlags()) {
          (*c)->Goto(1,oldCY);
        }

        ++c;
      }

      state.ForgetStateBehindLine(oldY-1);

      SetChanged(true);

      if (undoMode!=undoUndo) {
        PushUndo(new UndoInsert(oldPos,text.length()));
      }
      else {
        PushRedo(new UndoInsert(oldPos,text.length()));
      }

      NotifyRedraw(oldY,oldY);

      /*
        Create a message for inserted lines
      */

      if (lines>0) {
        NotifyInsert(oldY+1,lines);
      }

      NotifyTextChanged();

      return true;
    }

    bool Text::Insert(Cursor* cursor, wchar_t character, size_t count)
    {
      return Insert(cursor,std::wstring(count,character));
    }

    bool Text::Insert(Cursor* cursor, BufferRef buffer)
    {
      return Insert(cursor,buffer->GetText(eolMode));
    }

    bool Text::DeleteArea(Cursor* markA, Cursor* markB)
    {
      size_t                       firstX,lastX,lines;
      std::list<Cursor*>::iterator cursor;
      std::wstring                 tmp;

      if (markA->GetPos()>=markB->GetPos()) {
        // This may happen is a select an array behind line end. You visibly have something
        // selected, but this selction does not mark an area in the underlaying buffer.
        return true;
      }

      if (IsReadOnly()) {
        return false;
      }

      state.ForgetStateBehindLine(markA->GetY()-1);

      if (markA->GetX()>markA->GetLineLength()+1) {
        // TODO: Do we really need to fill?
        if (markA->GotoColumn(markA->GetLineLength()+1)) {
          Insert(markA,L' ',markA->GetX()-markA->GetLineLength()-1);
        }
      }

      MoveGap(markB->GetPos(),0);

      tmp.assign(&buffer[gapPos-(markB->GetPos()-markA->GetPos())],markB->GetPos()-markA->GetPos());

      /*
        Calculate messure of run
        (size of first and last line, number of lines over all)
      */

      CountText(tmp,firstX,lastX,lines);

      gapPos-=markB->GetPos()-markA->GetPos();
      fillSize-=markB->GetPos()-markA->GetPos();
      this->lines-=lines;

      // Fix cursors and global data

      cursor=cursors.begin();
      while (cursor!=cursors.end()) {
        if (*cursor==markA || *cursor==markB) {
          ++cursor;
          continue;
        }

        size_t oldCY=(*cursor)->GetY();

        /*
          All cursor within the deleted area are placed at the
          start of the deleted block. They get the coordinates of markA.
        */
        if ((*cursor)->GetPos()>=markA->GetPos() && (*cursor)->GetPos()<=markB->GetPos()) {
          (*cursor)->Goto(markA);
        }
        else if ((*cursor)->GetPos()>markB->GetPos()) {
          if ((*cursor)->GetY()==markB->GetY()) {
            if (lines==0) {
              (*cursor)->x-=firstX;
            }
            else {
              (*cursor)->x=markA->GetX()+(*cursor)->x-lastX;
            }
          }
          (*cursor)->y-=lines;
          (*cursor)->pos-=tmp.length();
        }

        if (Cursor::cursorFix & (*cursor)->GetFlags()) {
          (*cursor)->Goto(1,oldCY);
        }

        ++cursor;
      }

      markB->Goto(markA);

      SetChanged(true);

      if (undoMode!=undoUndo) {
        PushUndo(new UndoDelete(markA->GetPos(),tmp));
      }
      else {
        PushRedo(new UndoDelete(markA->GetPos(),tmp));
      }

      NotifyRedraw(markA->GetY(),markA->GetY());

      if (lines>0) {
        NotifyDelete(markA->GetY()+1,lines);
      }

      NotifyTextChanged();

      return true;
    }

    BufferRef Text::CopyArea(Cursor* markA, Cursor* markB)
    {
      std::wstring text;

      text.reserve(markB->GetPos()-markA->GetPos()+1);

      for (size_t x=markA->GetPos(); x<markB->GetPos(); x++) {
        text.append(1,GetChar(x));
      }

      return new Buffer(text,eolMode);
    }
  }
}

