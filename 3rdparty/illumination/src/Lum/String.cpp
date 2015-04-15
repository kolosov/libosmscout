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

#include <Lum/String.h>

#include <Lum/Base/String.h>

#include <Lum/Manager/Keyboard.h>

#include <Lum/Model/Number.h>
#include <Lum/Model/String.h>

#include <Lum/OS/Theme.h>

#include <Lum/Array.h>
#include <Lum/Button.h>
#include <Lum/Menu.h>

namespace Lum {

  class StringIMHandler : public IMHandler
  {
  private:
    String* control;

  public:
    void SetControl(String* control)
    {
      this->control=control;
    }

    void GetData(std::wstring& data,
                 size_t& cursorOffset)
    {
      assert(control!=NULL);

      data=L"";
      cursorOffset=0;

      control->GetBuffer(data,cursorOffset);
    }

    void SetData(const std::wstring& data)
    {
      assert(control!=NULL);

      control->SetBuffer(data);
    }

    void SetCursorOffset(size_t cursorOffset)
    {
      assert(control!=NULL);

      Model::StringRef model=dynamic_cast<Model::String*>(control->GetModel());

      if (!model.Valid() ||
          model->IsNull()) {
        return;
      }

      control->SetCursorPos(cursorOffset);
    }

    bool IsInEditingArea(OS::MouseEvent* event) const
    {
      assert(control!=NULL);

      return control->IsInEditingArea(event);
    }
  };

  static StringIMHandler *imHandler=NULL;

  static ControllerRef controller(new String::Controller());

  String::Controller::Controller()
  {
    RegisterKeyAction(L"C+x",actionCut);
    RegisterKeyAction(L"S+Delete",actionCut);

    RegisterKeyAction(L"C+c",actionCopy);
    RegisterKeyAction(L"C+Insert",actionCopy);

    RegisterKeyAction(L"C+v",actionPaste);
    RegisterKeyAction(L"S+Insert",actionPaste);

    RegisterKeyAction(L"C+a",actionSelectAll);
    RegisterKeyAction(L"Return",actionReturn);
    RegisterKeyAction(L"Escape",actionEscape);

    RegisterKeyAction(L"S+Left",actionSelectLeft);
    RegisterKeyAction(L"S+Right",actionSelectRight);
    RegisterKeyAction(L"S+Home",actionSelectStart);
    RegisterKeyAction(L"S+End",actionSelectEnd);

    RegisterKeyAction(L"Up",actionCursorUp);
    RegisterKeyAction(L"Down",actionCursorDown);
    RegisterKeyAction(L"Left",actionCursorLeft);
    RegisterKeyAction(L"Right",actionCursorRight);
    RegisterKeyAction(L"Home",actionCursorStart);
    RegisterKeyAction(L"End",actionCursorEnd);

    RegisterKeyAction(L"BackSpace",actionBackspace);
    RegisterKeyAction(L"Delete",actionDelete);
  }

  bool String::Controller::DoAction(Lum::Object* object, Action action)
  {
    String *string;

    string=dynamic_cast<String*>(object);

    switch (action) {
    case actionCut:
      string->ActionCut();
      return true;
    case actionCopy:
      string->ActionCopy();
      return true;
    case actionPaste:
      string->ActionPaste();
      return true;
    case actionSelectAll:
      string->ActionSelectAll();
      return true;
    case actionReturn:
      return string->ActionReturn();
    case actionEscape:
      return string->ActionEscape();
    case actionCursorUp:
      return string->ActionCursorUp();
    case actionCursorDown:
      return string->ActionCursorDown();
    case actionSelectLeft:
      string->ActionSelectLeft();
      break;
    case actionCursorLeft:
      string->ActionCursorLeft();
      return true;
    case actionSelectRight:
      string->ActionSelectRight();
      return true;
    case actionCursorRight:
      string->ActionCursorRight();
      return true;
    case actionSelectStart:
      string->ActionSelectStart();
      return true;
    case actionCursorStart:
      string->ActionCursorStart();
      return true;
    case actionSelectEnd:
      string->ActionSelectEnd();
      return true;
    case actionCursorEnd:
      string->ActionCursorEnd();
      return true;
    case actionBackspace:
      string->ActionBackspace();
      return true;
    case actionDelete:
      string->ActionDelete();
      return true;
    }

    return false;
  }

  String::String()
  : textAlign(left),
    baseline(false),
    visWidth(std::max((size_t)20u,OS::display->GetFont()->height*5)),
    offset(0),cursor(0),
    markA((size_t)-1),markB((size_t)-1),
    font(OS::display->GetFont()),
    model(NULL),
    sync(true),
    selected(false),
    cursorVisible(false),
    showClearButton(false),
    clearImage(OS::display->GetTheme()->GetStockImage(OS::Theme::imageClear)),
    mode(normal),
    blinkAction(new Model::Action),
    cutAction(new Model::Action),
    copyAction(new Model::Action),
    pasteAction(new Model::Action),
    eraseAction(new Model::Action),
    markAllAction(new Model::Action)
  {
    SetBackground(OS::display->GetFill(OS::Display::entryBackgroundFillIndex));

    SetController(Lum::controller);

    SetCanFocus(true);
    RequestFocus();
    SetRequestsKeyboard(true);
    SetRedrawOnMouseActive(true);

    cutAction->Disable();
    copyAction->Disable();
    eraseAction->Disable();
    markAllAction->Disable();

    Observe(blinkAction);

    Observe(cutAction);
    Observe(copyAction);
    Observe(pasteAction);
    Observe(eraseAction);
    Observe(markAllAction);
  }

  String::~String()
  {
    OS::display->RemoveTimer(blinkAction);
  }

  bool String::HasBaseline() const
  {
    return true;
  }

  size_t String::GetBaseline() const
  {
    assert(inited);

    return GetTopBorder()+(height-font->height) / 2 + font->ascent;
  }

  bool String::IsInEditingArea(OS::MouseEvent* event) const
  {
    if (!PointIsIn(*event)) {
      return false;
    }

    if (showClearButton) {
      size_t imageWidth=clearImage->GetWidth();
      size_t imageHeight=clearImage->GetHeight();

      if (imageHeight>height) {
        imageWidth=(imageWidth*height)/imageHeight;
      }

      if (event->x>=GetX()+(int)(GetWidth()-1-imageWidth)) {
        return false;
      }
    }

    return true;
  }

  bool String::HasImplicitLabel() const
  {
    return !label.empty();
  }

  IMHandler* String::GetIMHandler()
  {
    if (imHandler==NULL) {
      imHandler=new StringIMHandler();
    }

    imHandler->SetControl(this);

    return imHandler;
  }

  bool String::SetModel(Base::Model* model)
  {
    this->model=model;

    Control::SetModel(this->model);

    if (!converter.Valid()) {
      if (dynamic_cast<Model::String*>(model)) {
        SetConverter(new Model::StringStringConverter());
      }
      else if (dynamic_cast<Model::Number*>(model)) {
        SetConverter(new Model::NumberStringConverter());
      }
    }

    return this->model.Valid();
  }

  String* String::SetConverter(Model::StringConverter* converter)
  {
    this->converter=converter;

    SyncModelToBuffer();

    return this;
  }

  String* String::SetLabel(const std::wstring& label)
  {
    this->label=label;

    Redraw();

    return this;
  }

  void String::SyncModelToBuffer()
  {
    if (converter.Valid()) {
      converter->ConvertModelToString(model,buffer);
    }
  }

  void String::SyncBufferToModel()
  {
    if (converter.Valid()) {
      sync=false;
      converter->ConvertStringToModel(buffer,model);
      sync=true;

      Redraw();
    }
  }

  bool String::GetBuffer(std::wstring& buffer,
                         size_t& cursorOffset) const
  {
    if (!model.Valid()) {
      return false;
    }

    buffer=this->buffer;
    cursorOffset=this->cursor;

    return true;
  }

  bool String::SetBuffer(const std::wstring& buffer)
  {
    if (!model.Valid() ||
        model->IsDisabled()) {
      return false;
    }

    this->buffer=buffer;

    SyncBufferToModel();

    Redraw();

    return true;
  }

  void String::SetCursorUpAction(Model::Action* action)
  {
    cursorUpAction=action;
  }

  void String::SetCursorDownAction(Model::Action* action)
  {
    cursorDownAction=action;
  }

  void String::SetReturnAction(Model::Action* action)
  {
    returnAction=action;
  }

  void String::SetEscapeAction(Model::Action* action)
  {
    escapeAction=action;
  }

  void String::SetFocusInAction(Model::Action* action)
  {
    focusInAction=action;
  }

  void String::SetFocusOutAction(Model::Action* action)
  {
    focusOutAction=action;
  }

  String* String::SetMode(Mode mode)
  {
    this->mode=mode;

    Redraw();

    return this;
  }

  String* String::SetAlignment(Alignment alignment)
  {
    textAlign=alignment;

    Redraw();

    return this;
  }

  String* String::ShowClearButton(bool show)
  {
    showClearButton=show;

    Redraw();

    return this;
  }

  bool String::GetShowClearButton() const
  {
    if (showClearButton) {

      return clearImage.Valid();
    }
    else {
      return false;
    }
  }

  Model::Action* String::GetCursorUpAction() const
  {
    return cursorUpAction.Get();
  }

  Model::Action* String::GetCursorDownAction() const
  {
    return cursorDownAction.Get();
  }

  Model::Action* String::GetReturnAction() const
  {
    return returnAction.Get();
  }

  Model::Action* String::GetEscapeAction() const
  {
    return escapeAction.Get();
  }

  Model::Action* String::GetFocusInAction() const
  {
    return focusInAction.Get();
  }

  Model::Action* String::GetFocusOutAction() const
  {
    return focusOutAction.Get();
  }

  String::Alignment String::GetAlignment() const
  {
    return textAlign;
  }

  void String::ActionInsertString(const std::wstring& string)
  {
    if (!model.Valid() ||
        !model->IsEnabled()) {
      OS::display->Beep();

      return;
    }

    DeleteSelection();
    buffer.insert(cursor,string);
    cursor+=string.length();

    SyncBufferToModel();
  }

  void String::ActionCut()
  {
    if (!model.Valid() ||
        !Selected() ||
        !model->IsEnabled()) {
      OS::display->Beep();

      return;
    }

    std::wstring text(buffer);

    text=text.substr(markA,markB-markA);

    if (!OS::display->SetClipboard(text)) {
      OS::display->Beep();
    }

    DeleteSelection();
  }

  void String::ActionCopy()
  {
    if (!model.Valid() ||
        !Selected()) {
      OS::display->Beep();

      return;
    }

    std::wstring text(buffer);

    text=text.substr(markA,markB-markA);

    if (!OS::display->SetClipboard(text)) {
      OS::display->Beep();
    }
  }

  void String::ActionPaste()
  {
    if (!model->IsEnabled()) {
      OS::display->Beep();

      return;
    }

    std::wstring res;

    res=OS::display->GetClipboard();

    if (!res.empty()) {
      ActionInsertString(res);
    }
  }

  void String::ActionErase()
  {
    if (!model.Valid() ||
        !Selected() ||
        !model->IsEnabled()) {
      OS::display->Beep();

      return;
    }

    DeleteSelection();
  }

  void String::ActionEraseAll()
  {
    if (!model.Valid() ||
        !model->IsEnabled()) {
      OS::display->Beep();

      return;
    }

    ClearSelection();

    buffer=L"";

    SyncBufferToModel();
  }

  void String::ActionSelectAll()
  {
    SelectAll();
    Redraw();
  }

  bool String::ActionReturn()
  {
    if (!returnAction.Valid()) {
      return false;
    }

    if (!HasFocus()) {
      selected=false;
      Redraw();
    }

    returnAction->Trigger();

    return true;
  }

  bool String::ActionEscape()
  {
    if (!escapeAction.Valid()) {
      return false;
    }

    if (!HasFocus()) {
      selected=false;
      Redraw();
    }

    escapeAction->Trigger();

    return true;
  }

  void String::ActionSelectLeft()
  {
    if (cursor>0) {
      if (!Selected()) {
        cursor--;
        SetSelection(cursor,cursor+1);
        Redraw();
      }
      else if ((markA+1==markB) && (markB==cursor)) {
        ClearSelection();
        cursor--;
        Redraw();
      }
      else if (cursor==markA) {
        cursor--;
        SetSelection(cursor,markB);
        Redraw();
      }
      else if (cursor==markB) {
        cursor--;
        SetSelection(markA,cursor);
        Redraw();
      }
    }
  }

  void String::ActionSelectRight()
  {
    if (cursor<buffer.length()) {
      if (!Selected()) {
        cursor++;
        SetSelection(cursor-1,cursor);
        Redraw();
      }
      else if ((markA+1==markB) && (markA==cursor)) {
        ClearSelection();
        cursor++;
        Redraw();
      }
      else if (cursor==markB) {
        cursor++;
        SetSelection(markA,cursor);
        Redraw();
      }
      else if (cursor==markA) {
        cursor++;
        SetSelection(cursor,markB);
        Redraw();
      }
    }
  }

  void String::ActionSelectStart()
  {
    if (Selected()) {
      if (cursor==markA) {
        SetSelection(0,markB);
      }
      else {
        SetSelection(0,markA);
      }
    }
    else {
      SetSelection(0,cursor);
    }

    cursor=0;
    Redraw();
  }

  void String::ActionSelectEnd()
  {
    if (Selected()) {
      if (cursor==markA) {
        SetSelection(markB,buffer.length());
      }
      else {
        SetSelection(markA,buffer.length());
      }
    }
    else {
      SetSelection(cursor,buffer.length());
    }

    cursor=buffer.length();
    Redraw();
  }

  bool String::ActionCursorUp()
  {
    if (!cursorUpAction.Valid()) {
      return false;
    }

    cursorUpAction->Trigger();

    return true;
  }

  bool String::ActionCursorDown()
  {
    if (!cursorDownAction.Valid()) {
      return false;
    }

    cursorDownAction->Trigger();
    return true;
  }

  void String::ActionCursorLeft()
  {
    if (Selected()) {
      cursor=markA;
      ClearSelection();
    }
    else if (cursor>0) {
      cursor--;
    }

    TriggerCursorBlink();

    Redraw();
  }

  void String::ActionCursorRight()
  {
    if (Selected()) {
      cursor=markB;
      ClearSelection();
    }
    else if (cursor<buffer.length()) {
      cursor++;
    }

    TriggerCursorBlink();

    Redraw();
  }

  void String::ActionCursorStart()
  {
    ClearSelection();

    cursor=0;

    TriggerCursorBlink();

    Redraw();
  }

  void String::ActionCursorEnd()
  {
    ClearSelection();

    cursor=buffer.length();

    TriggerCursorBlink();

    Redraw();
  }

  void String::ActionBackspace()
  {
    if (!model.Valid() ||
        model->IsDisabled()) {
      return;
    }

    if (Selected()) {
      cursor=markA;
      DeleteSelection();
    }
    else if (cursor>0) {
      ClearSelection();
      cursor--;
      TriggerCursorBlink();
      buffer.erase(cursor,1);
      SyncBufferToModel();
    }
  }

  void String::ActionDelete()
  {
    if (!model.Valid() ||
        model->IsDisabled()) {
      return;
    }

    if (Selected()) {
      cursor=markA;
      DeleteSelection();
    }
    else if (cursor<buffer.length()) {
      ClearSelection();
      TriggerCursorBlink();
      buffer.erase(cursor,1);
      SyncBufferToModel();
    }
  }

  bool String::CanExecuteAction(const Base::Datatype& type,
                                Base::DataExchangeObject::ExchangeAction action) const
  {
    if (!type.Matches(Lum::Base::Datatype::text)) {
      return false;
    }

    switch (action) {
    case actionMove:
      return model.Valid() && Selected();
      break;
    case actionCopy:
      return model.Valid() && Selected();
      break;
    case actionInsert:
      return model.Valid();
      break;
    default:
      return false;
    }
  }


  bool String::ExecuteAction(const Base::Datatype& type,
                             Base::Data& data,
                             Base::DataExchangeObject::ExchangeAction action)
  {
    std::wstring text;

    if (!CanExecuteAction(type,action)) {
      return false;
    }

    switch (action) {
    case actionInsert:
      if (Selected()) {
        DeleteSelection();
      }

      text=data.GetText();

      cursor+=text.length();
      buffer.insert(cursor-text.length(),text);
      SyncBufferToModel();
      return true;
    case actionCopy:
    case actionMove:
      data.SetText(buffer.substr(markA,markB-markA));
      return true;
    default:
      return false;
    }
  }

  bool String::PostprocessAction(Base::DataExchangeObject::ExchangeAction action)
  {
    if (!model.Valid() ||
        action!=actionMove ||
        !Selected()) {
      return false;
    }

    DeleteSelection();

    return true;
  }

  void String::CalcSize()
  {
    width=visWidth;
    height=font->height;

    minWidth=width;
    minHeight=height;

    if (GetMenu()==NULL) {
      Menu *menu;

      menu=new Menu();
      menu->SetParent(GetWindow());
      menu->AddActionItem(L"Cut",cutAction);
      menu->AddActionItem(L"Copy",copyAction);
      menu->AddActionItem(L"Paste",pasteAction);
      menu->AddActionItem(L"Erase",eraseAction);
      menu->AddSeparator();
      menu->AddActionItem(L"Mark all",markAllAction);

      SetMenu(menu->GetWindow());
    }

    Control::CalcSize();
  }

  size_t String::GetCursorPos(int x) const
  {
    std::wstring help;
    int          widthA,widthB;

    x-=textPos;

    if (x<=0) {
      return 0;
    }

    help=GetValue();

    if (help.empty()) {
      return 0;
    }

    widthA=0;
    for (size_t y=0; y<help.length()-1; y++) {
      OS::FontExtent extent;

      font->StringExtent(help.substr(0,y+1),extent);
      widthB=extent.width;
      if (x>=widthA && x<=widthB) {
        if (x-widthA>(widthB-widthA)/2) {
          y++;
        }
        return y;
      }

      widthA=widthB;
    }

    return help.length();
  }

  bool String::Selected() const
  {
    return markA!=(size_t)-1;
  }

  void String::SetSelection(size_t a, size_t b)
  {
    if (OS::display->RegisterSelection(this,GetWindow())) {
      markA=a;
      markB=b;

      if (model->IsEnabled()) {
        cutAction->Enable();
      }
      else {
        cutAction->Disable();
      }

      copyAction->Enable();

      if (model->IsEnabled()) {
        eraseAction->Enable();
      }
      else {
        eraseAction->Disable();
      }

      EvaluateMarkAllActionRights();
    }
  }

  void String::ClearSelection()
  {
    if (Selected()) {
      markA=(size_t)-1;
      markB=(size_t)-1;
      OS::display->CancelSelection();

      cutAction->Disable();
      copyAction->Disable();
      eraseAction->Disable();

      EvaluateMarkAllActionRights();
    }
  }

  void String::SelectAll()
  {
    if (!model.Valid()) {
      return;
    }

    if (Selected()) {
      ClearSelection();
    }

    if (buffer.length()>0) {
      SetSelection(0,buffer.length());
      cursor=0;
    }
  }

  void String::DeleteSelection()
  {
    if (!Selected()) {
      return;
    }

    int a,b;

    a=markA;
    b=markB;
    ClearSelection();

    buffer.erase(a,b-a);
    cursor=a;

    SyncBufferToModel();

    TriggerCursorBlink();
  }

  void String::DrawCursor()
  {
    Redraw(cursorPos,y+(height-font->height) / 2,1,font->height);
  }

  std::wstring String::GetValue() const
  {
    if (mode==password) {
      return std::wstring(buffer.length(),L'*');
    }
    else {
      return buffer;
    }
  }

  /**
    Sets the cursor to visible, cancels old cursor blink
    timer and starts new timer.

    Expects calling code to refresh control afterwards.
    This method does not trigger a redraw itself!
   */
  void String::TriggerCursorBlink()
  {
    cursorVisible=true;
    OS::display->RemoveTimer(blinkAction);
    if (OS::display->GetTheme()->GetCursorBlinkTimeMicroSeconds()>0) {
      OS::display->AddTimer(0,OS::display->GetTheme()->GetCursorBlinkTimeMicroSeconds(),
                            blinkAction);
    }
  }

  /**
    Sets the cursor to hidden and cancels old cursor blink
    timer.

    Expects calling code to refresh control afterwards.
    This method does not trigger a redraw itself!
   */
  void String::StopCursorBlink()
  {
    cursorVisible=false;
    OS::display->RemoveTimer(blinkAction);
  }

  void String::PrepareForBackground(OS::DrawInfo* draw)
  {
    draw->focused=HasFocus();
    draw->disabled=(!model.Valid() || !model->IsEnabled());
  }

  void String::Draw(OS::DrawInfo* draw,
                    int x, int y, size_t w, size_t h)
  {
    bool drawLabel=false;

    draw->focused=HasFocus();
    draw->disabled=(!model.Valid() || !model->IsEnabled());

    Control::Draw(draw,x,y,w,h);

    if (!OIntersect(x,y,w,h)) {
      draw->focused=false;
      draw->disabled=false;
      return;
    }

    /* --- */

    OS::FontExtent extent1,extent2;
    std::wstring   string,help;
    int            xPos,fieldWidth,yOff;

    yOff=this->y+(height-font->height) / 2;

    /* Draw optional baseline */
    if (baseline) {
      draw->PushForeground(OS::Display::editTextColor);
      draw->DrawLine(this->x,yOff+font->ascent-1,
                     this->x+width-1,yOff+font->ascent-1);
      draw->PopForeground();
    }

    /* value to be printed */
    string=GetValue();

    if (string.empty() &&
        !draw->focused) {
      string=label;
      drawLabel=true;
    }

    draw->PushClip(this->x,this->y,width,height);

    if (cursor>string.length()) {
      cursor=string.length();
    }

    /* x = left starting position of string*/
    xPos=this->x;
    fieldWidth=width;

    if (GetShowClearButton()) {
      size_t imageWidth=clearImage->GetWidth();
      size_t imageHeight=clearImage->GetHeight();

      if (imageHeight>height) {
        imageWidth=(imageWidth*height)/imageHeight;
      }

      fieldWidth-=imageWidth;
    }

    /* Calculate starting pos depending on alignment */
    if (textAlign==left) {
      textPos=xPos;
    }
    else if (textAlign==right) {
      textPos=xPos+fieldWidth-font->StringWidth(string,OS::Font::normal);
    }
    else { /* center */
      textPos=xPos+(fieldWidth-font->StringWidth(string,OS::Font::normal)) / 2;
    }

    /* Calculate bound of first letter */
    font->StringExtent(string.substr(0,1),extent2);

    /*
      correct starting position of string by left hand
      space of starting character
     */
    textPos-=extent2.left;

    cursorPos=textPos;

    if (cursor>0) {
      /* Copy string from 0 before cursor to help */
      help=string.substr(0,cursor);

      /* calculate bounds of help */
      font->StringExtent(help,extent1);

      /* Calculate cursor position */
      cursorPos+=extent1.width;
    }

    /* Make cursor visible in gadget by correcting starting offset */
    if (cursorPos-offset>xPos+fieldWidth-1) { /* cursor is out on the right */
      offset+=cursorPos-offset-(xPos+fieldWidth-1);
    }
    else if (cursorPos-offset<xPos) { /* cursor is out on the left */
      offset-=xPos-cursorPos+offset;
    }

    /* correct textstart and cursorpos, too */
    cursorPos-=offset;
    textPos-=offset;

    /* Draw the string */

    if (drawLabel) {
      draw->PushFont(font,OS::Font::italic);
    }
    else{
      draw->PushFont(font,OS::Font::normal);
    }

    if (Selected()) { /* there exists a selection */
      int         markAPos,markBPos;
      OS::FillRef back=OS::display->GetFill(OS::Display::entryBackgroundFillIndex);

      //
      // Text before the selection
      //

      markAPos=textPos;

      if (markA>0) {
        /* Copy string from 0 before cursor to help */
        help=string.substr(0,markA);

        /* calculate bounds of help */
        font->StringExtent(help,extent1);

        /* Calculate cursor position */
        markAPos+=extent1.width-extent1.right;

        /* correct cursor position by first character of 2nd string */
        if (markA<string.length()) {
          font->StringExtent(string.substr(markA,1),extent2);
          markAPos+=extent2.left;
        }

        draw->PushClip(textPos,this->y,markAPos-textPos,height);
        draw->PushForeground(OS::Display::editTextColor);
        draw->DrawString(textPos,yOff+font->ascent,string);
        draw->PopForeground();
        draw->PopClip();
      }

      //
      // Text behind the selection
      //

      /* Copy string from 0 before cursor to help */
      help=string.substr(0,markB);

      /* calculate bounds of help^ */
      font->StringExtent(help,extent1);

      /* Calculate cursor position */
      markBPos=textPos+extent1.width-extent1.right;

      /* correct cursor position by first character of 2nd string */
      if (markB<string.length()) {
        font->StringExtent(string.substr(markB,1),extent2);
        markBPos+=extent2.left;

        draw->PushClip(markBPos,yOff,width,height);
        draw->PushForeground(OS::Display::editTextColor);
        draw->DrawString(textPos,yOff+font->ascent,string);
        draw->PopForeground();
        draw->PopClip();
      }

      //
      // Selected text
      //

      draw->selected=true;
      {
        // clip on x&width: back->Draw() will not clip by itself. It will assert...
        int markAPosClipped=markAPos;
        size_t markWidthClipped=markBPos-markAPos+1; // BTW: why +1 ??
        if (markAPosClipped<oX) {
          markWidthClipped-=oX-markAPosClipped;
          markAPosClipped=oX;
        }

        if (markAPosClipped+markWidthClipped>oX+oWidth) {
          markWidthClipped=oX+oWidth-markAPos;
        }

        back->Draw(draw,
                   oX,oY,oWidth,oHeight,
                   markAPosClipped,yOff,
                   markWidthClipped,font->height);
      }
      draw->selected=false;

      draw->PushClip(markAPos,yOff,markBPos-markAPos+1,height);
      draw->PushForeground(OS::Display::editTextFillColor);
      draw->DrawString(textPos,yOff+font->ascent,string);
      draw->PopForeground();
      draw->PopClip();
    }
    else {
      if (drawLabel) {
        draw->PushForeground(OS::Display::textDisabledColor);
      }
      else {
        draw->PushForeground(OS::Display::editTextColor);
      }
      draw->DrawString(textPos,yOff+font->ascent,string);
      draw->PopForeground();
    }

    draw->PopFont();

    /* Drawing the cursor */
    if (selected && !Selected() && cursorVisible) {
      if (OS::display->GetType()==OS::Display::typeTextual) {
        draw->PushForeground(OS::Display::fillColor);
        draw->FillRectangle(cursorPos,yOff,1,font->height);
        draw->PopForeground();

        draw->PushFont(font);
        draw->PushForeground(OS::Display::fillTextColor);
        draw->DrawString(cursorPos,yOff,string.substr(cursor,1));
        draw->PopForeground();
        draw->PopFont();
      }
      else {
        draw->PushForeground(OS::Display::editTextColor);
        draw->FillRectangle(cursorPos,yOff,
                            OS::display->GetTheme()->GetCursorWidth(font),font->height);
        draw->PopForeground();
      }
    }

    if (GetShowClearButton()) {
      size_t imageWidth=clearImage->GetWidth();
      size_t imageHeight=clearImage->GetHeight();

      if (imageHeight>height) {
        imageWidth=(imageWidth*height)/imageHeight;
        imageHeight=height;
      }
      clearImage->DrawScaled(draw,
                             this->x+fieldWidth,
                             this->y+((int)height-(int)imageHeight)/2,
                             imageWidth,imageHeight);
    }

    draw->PopClip();

    draw->focused=false;
    draw->disabled=false;
  }

  void String::SetCursorPos(size_t pos)
  {
    assert(pos<=buffer.length());

    cursor=pos;

    if (visible) {
      Redraw();
    }
  }

  size_t String::GetCursorPos() const
  {
    return cursor;
  }

  /**
    Clears the current selection.
  */
  void String::Deselect()
  {
    ClearSelection();
    if (visible) {
      Redraw();
    }
  }

  void String::PrepareMenu()
  {
    if (OS::display->HasClipboard()) {
      pasteAction->Enable();
    }
    else {
      pasteAction->Disable();
    }
  }

  bool String::HandleMouseEvent(const OS::MouseEvent& event)
  {
    if (!visible ||
        !model.Valid()) {
      return false;
    }

    if (event.type==OS::MouseEvent::down &&
        PointIsIn(event) &&
        event.button==OS::MouseEvent::button1) {
      if (GetShowClearButton() &&
          event.x>=this->x+(int)(this->width-1-OS::display->GetTheme()->GetStockImage(OS::Theme::imageClear)->GetWidth())) {
        ActionEraseAll();
      }
      else if (GetWindow()->IsDoubleClicked() &&
               PointIsIn(GetWindow()->GetLastButtonClickEvent())) {
        SelectAll();
        Redraw();
      }
      else {
        selected=true;
        cursor=GetCursorPos(event.x);
        TriggerCursorBlink();
        ClearSelection();
        Redraw();
      }

      return true;
    }
    else if (event.type==OS::MouseEvent::down &&
             PointIsIn(event) &&
             event.qualifier==0 &&
             event.button==OS::MouseEvent::button2) {
      selected=true;
      cursor=GetCursorPos(event.x);
      TriggerCursorBlink();
      ClearSelection();
      Redraw();

      OS::display->QuerySelection(GetWindow(),this);

      return true;
    }
    else if (event.IsGrabEnd() &&
             event.type==OS::MouseEvent::up &&
             event.button==OS::MouseEvent::button1) {
      if (!GetWindow()->IsDoubleClicked()) {
        size_t help=GetCursorPos(event.x);

        if (help>cursor) {
          SetSelection(cursor,help);
          cursor=help;
          Redraw();
        }
        else if (help<cursor) {
          SetSelection(help,cursor);
          Redraw();
        }
      }
    }
    else if (event.type==OS::MouseEvent::move &&
             event.IsGrabed() &&
             event.qualifier==OS::qualifierButton1) {
      size_t help=GetCursorPos(event.x);

      if (help>cursor) {
        SetSelection(cursor,help);
      }
      else if (help<cursor) {
        SetSelection(help,cursor);
      }
      else {
        ClearSelection();
      }
      Redraw();
      return true;
    }

    return false;
  }

  bool String::HandleKeys(const OS::KeyEvent& event)
  {
    switch (event.key) {
    case OS::keyTab:
      return false;
    case OS::keyLeftTab:
      return false;
    case OS::keyUp:
      return false;
    case OS::keyDown:
      return false;
    case OS::keyReturn:
      break;
    case OS::keyEscape:
      break;
    default:
      unsigned long qualifier;
      std::wstring  name;

      event.GetName(name);
      qualifier=event.qualifier & ~(OS::qualifierShiftLeft|OS::qualifierShiftRight|OS::qualifierShift|OS::qualifierCapsLock);

      if (!event.text.empty() &&
          !(name==event.text && qualifier!=0) &&
          event.text[0]>=32) {
        ActionInsertString(event.text);
        return true;
      }
    }

    return false;
  }

  void String::EvaluateMarkAllActionRights()
  {
    if (!this->model.Valid() ||
        buffer.length()==0 ||
        (markA==0 && markB==buffer.length())) {
      markAllAction->Disable();
    }
    else {
      markAllAction->Enable();
    }
  }

  bool String::HandleKeyEvent(const OS::KeyEvent& event)
  {
    if (Control::HandleKeyEvent(event)) {
      return true;
    }

    if (event.type==OS::KeyEvent::down) {
      return HandleKeys(event);
    }

    return false;
  }

  void String::RecatchedFocus()
  {
    selected=true;
    TriggerCursorBlink();

    Control::RecatchedFocus();

    if (focusInAction.Valid()) {
      focusInAction->Trigger();
    }
  }

  void String::CatchedFocus()
  {
    if (!selected && model.Valid()) {
      SelectAll();
    }

    selected=true;
    TriggerCursorBlink();

    Control::CatchedFocus();

    if (focusInAction.Valid()) {
      focusInAction->Trigger();
    }
  }

  void String::LostFocus()
  {
    selected=false;

    StopCursorBlink();

    //ClearSelection();
    Control::LostFocus();

    if (focusOutAction.Valid()) {
      focusOutAction->Trigger();
    }
  }

  void String::Resync(Base::Model* model, const Base::ResyncMsg& msg)
  {
    if (model==blinkAction && blinkAction->IsFinished()) {
      cursorVisible=!cursorVisible;

      if (visible && selected && !Selected()) {
        DrawCursor();
        if (OS::display->GetTheme()->GetCursorBlinkTimeMicroSeconds()>0) {
          OS::display->AddTimer(0,OS::display->GetTheme()->GetCursorBlinkTimeMicroSeconds(),
                                blinkAction);
        }
      }
    }
    else if (model==cutAction && cutAction->IsFinished()) {
      ActionCut();
    }
    else if (model==copyAction && copyAction->IsFinished()) {
      ActionCopy();
    }
    else if (model==pasteAction && pasteAction->IsFinished()) {
      ActionPaste();
    }
    else if (model==eraseAction && eraseAction->IsFinished()) {
      ActionErase();
    }
    else if (model==markAllAction && markAllAction->IsFinished()) {
      ActionSelectAll();
    }
    else if (model==this->model) {
      if (sync) {
         SyncModelToBuffer();

        if (Selected()) {
          SelectAll();
        }

        EvaluateMarkAllActionRights();

        if (visible) {
          Redraw();
        }
      }
    }

    Control::Resync(model,msg);
  }

  String* String::Create(bool horizontalFlex, bool verticalFlex)
  {
    String *s;

    s=new String();
    s->SetFlex(horizontalFlex,verticalFlex);

    return s;
  }

  String* String::Create(Base::Model* model, bool horizontalFlex, bool verticalFlex)
  {
    String *s;

    s=new String();
    s->SetFlex(horizontalFlex,verticalFlex);
    s->SetModel(model);

    return s;
  }

  String* String::Create(Base::Model* model, size_t widthInChars,
                         bool horizontalFlex, bool verticalFlex)
  {
    String *s;

    s=new String();
    s->SetFlex(horizontalFlex,verticalFlex);
    s->SetMinWidth(Lum::Base::Size::stdCharWidth,widthInChars);
    s->SetModel(model);

    return s;
  }
}

