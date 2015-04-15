#ifndef LUM_STRING_H
#define LUM_STRING_H

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

#include <Lum/Model/Action.h>
#include <Lum/Model/Converter.h>

#include <Lum/OS/Display.h>

#include <Lum/Button.h>
#include <Lum/Object.h>
#include <Lum/Popup.h>

namespace Lum {

  /**
    Implements a single one-line text input object.

    It expects a model of type Model::String.
  */
  class LUMAPI String : public Control
  {
  public:
    enum Alignment
    {
      left,
      right,
      center
    };

    enum Mode
    {
      normal,
      password
    };

  private:
    enum Actions {
      actionCut,
      actionCopy,
      actionPaste,
      actionSelectAll,
      actionReturn,
      actionEscape,
      actionCursorUp,
      actionCursorDown,
      actionSelectLeft,
      actionCursorLeft,
      actionSelectRight,
      actionCursorRight,
      actionSelectStart,
      actionCursorStart,
      actionSelectEnd,
      actionCursorEnd,
      actionBackspace,
      actionDelete
    };

  public:
    class Controller : public Lum::Controller
    {
    public:
      Controller();

      bool DoAction(Lum::Object* object, Action action);
    };

  private:
    Alignment        textAlign;        //! Alignment of text
    bool             baseline;         //! Draw the baseline (TODO: Move to theme)

    int              visWidth;         //! Visible with of the control in pixel
    int              offset;
    size_t           cursor;           //! Position of cursor in text
    size_t           markA,markB;
    int              textPos;          //! Absolute position of text
    int              cursorPos;        //! Absolute position of cursor
    OS::FontRef      font;             //! Font to use

    Base::ModelRef   model;            //! Value model
    std::wstring     buffer;           //! local edit buffer, in sync with model
    bool             sync;             //! If true, sync back model changes to buffer, else not
    Model::StringConverterRef converter; //! Converts from internal buffer to model and back

    std::wstring     label;            //! Optional label, shown if no value and no focus
    bool             selected;         //! The is currently a selection
    bool             cursorVisible;    //! The cursor is currently visible (=> cursor blinking)
    bool             showClearButton;  //! Show a clear button as part of control
    Lum::OS::ImageRef clearImage;      //! Image to be shown as clear button
    Mode             mode;             //! Draw mode to be used for drawing the text
    Model::ActionRef blinkAction;      //! Timer action for triggering cursor blinking

    Model::ActionRef cutAction;
    Model::ActionRef copyAction;
    Model::ActionRef pasteAction;
    Model::ActionRef eraseAction;
    Model::ActionRef markAllAction;

    Model::ActionRef cursorUpAction;
    Model::ActionRef cursorDownAction;
    Model::ActionRef returnAction;
    Model::ActionRef escapeAction;
    Model::ActionRef focusInAction;
    Model::ActionRef focusOutAction;

  private:
    void SyncModelToBuffer();
    void SyncBufferToModel();

    size_t GetCursorPos(int x) const;
    bool Selected() const;
    void SetSelection(size_t a, size_t b);
    void ClearSelection();
    void SelectAll();
    void DeleteSelection();

    std::wstring GetValue() const;

    void DrawCursor();

    void TriggerCursorBlink();
    void StopCursorBlink();

    void Deselect();

    bool HandleKeys(const OS::KeyEvent& event);

    void EvaluateMarkAllActionRights();

    bool GetShowClearButton() const;

  public:
    String();
    ~String();

    bool HasBaseline() const;
    size_t GetBaseline() const;

    bool HasImplicitLabel() const;

    bool IsInEditingArea(OS::MouseEvent* event) const;

    IMHandler* GetIMHandler();

    // Setter
    bool SetModel(Base::Model* model);

    bool GetBuffer(std::wstring& buffer,
                   size_t& cursorOffset) const;
    bool SetBuffer(const std::wstring& buffer);

    String* SetConverter(Model::StringConverter* converter);
    String* SetLabel(const std::wstring& label);

    void SetCursorUpAction(Model::Action* action);
    void SetCursorDownAction(Model::Action* action);
    void SetReturnAction(Model::Action* action);
    void SetEscapeAction(Model::Action* action);
    void SetFocusInAction(Model::Action* action);
    void SetFocusOutAction(Model::Action* action);

    String* SetMode(Mode mode);
    String* SetAlignment(Alignment alignment);
    String* ShowClearButton(bool show);

    // Getter
    Model::Action* GetCursorUpAction() const;
    Model::Action* GetCursorDownAction() const;
    Model::Action* GetReturnAction() const;
    Model::Action* GetEscapeAction() const;
    Model::Action* GetFocusInAction() const;
    Model::Action* GetFocusOutAction() const;

    Alignment GetAlignment() const;

    void ActionInsertString(const std::wstring& string);

    void ActionCut();
    void ActionCopy();
    void ActionPaste();
    void ActionErase();
    void ActionEraseAll();
    void ActionSelectAll();

    bool ActionReturn();
    bool ActionEscape();

    void ActionSelectLeft();
    void ActionSelectRight();
    void ActionSelectStart();
    void ActionSelectEnd();

    bool ActionCursorUp();
    bool ActionCursorDown();
    void ActionCursorLeft();
    void ActionCursorRight();
    void ActionCursorStart();
    void ActionCursorEnd();

    void ActionBackspace();
    void ActionDelete();

    void SetCursorPos(size_t pos);
    size_t GetCursorPos() const;

    bool CanExecuteAction(const Base::Datatype& type,
                          Base::DataExchangeObject::ExchangeAction action) const;
    bool ExecuteAction(const Base::Datatype& type,
                       Base::Data& data,
                       Base::DataExchangeObject::ExchangeAction action);
    bool PostprocessAction(Base::DataExchangeObject::ExchangeAction action);

    void CalcSize();

    void PrepareForBackground(OS::DrawInfo* draw);
    void Draw(OS::DrawInfo* draw,
              int x, int y, size_t w, size_t h);
    void RecatchedFocus();
    void CatchedFocus();
    void LostFocus();

    void PrepareMenu();

    bool HandleMouseEvent(const OS::MouseEvent& event);
    bool HandleKeyEvent(const OS::KeyEvent& event);

    void Resync(Base::Model* model, const Base::ResyncMsg& msg);

    static String* Create(bool horizontalFlex=false, bool verticalFlex=false);
    static String* Create(Base::Model* model, bool horizontalFlex=false, bool verticalFlex=false);
    static String* Create(Base::Model* model, size_t widthInChars,
                          bool horizontalFlex=false, bool verticalFlex=false);
  };
}

#endif
