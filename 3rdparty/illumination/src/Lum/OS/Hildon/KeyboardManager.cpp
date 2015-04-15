/*
  This source is part of the Illumination library
  Copyright (C) 2008  Tim Teulings

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

#include <Lum/OS/Hildon/Keyboard.h>

#include <X11/Xatom.h>

#include <hildon/hildon-version.h>
#include <gtk/gtkenums.h>

#include <Lum/Base/String.h>

#include <Lum/OS/X11/Display.h>
#include <Lum/OS/X11/Event.h>
#include <Lum/OS/X11/Window.h>

#include <Lum/Window.h>

#include <iostream>

namespace Lum {
  namespace OS {
    namespace Hildon {

      Keyboard::Keyboard()
      : window(NULL),
        object(NULL)
      {
#if HILDON_CHECK_VERSION(2,1,0)
        triggerType=HILDON_IM_TRIGGER_FINGER;
#else
        triggerType=HILDON_IM_TRIGGER_STYLUS;
#endif
        // no code
      }

      Keyboard::~Keyboard()
      {
        // no code
      }

      void Keyboard::Shutdown()
      {
        //HideKeyboard();
      }

      void Keyboard::OnWindowFocusIn(OS::Window* window)
      {
        this->window=window;
        this->object=NULL;
      }

      void Keyboard::OnWindowFocusOut(OS::Window* window)
      {
        // no code
      }

      void Keyboard::OnFocusChange(OS::Window* window,
                                   Lum::Object* oldObject,
                                   Lum::Object* newObject)
      {
        if (this->window!=window) {
          return;
        }

#if HILDON_CHECK_VERSION(2,1,0)
        if (newObject!=NULL &&
                 newObject->RequestsKeyboard() &&
                 newObject->GetIMHandler()!=NULL) {
          object=newObject;
        }
        else {
          object=NULL;
        }
#else
        if (object==newObject) {
          return;
        }

        if (object!=NULL && object->RequestsKeyboard() &&
            newObject!=NULL && newObject->RequestsKeyboard()) { // "do nothing" case
          object=newObject;
        }
        else if (newObject!=NULL && newObject->RequestsKeyboard()) { // "show" case
          object=newObject;
          SendInputMode();
          ShowKeyboard();
        }
        else { // "hide" case
          object=NULL;
          HideKeyboard();
        }
#endif
      }

      void Keyboard::OnMouseClick(Window* window,
                                  Lum::Object* object,
                                  MouseEvent* event)
      {
        if (this->window!=window) {
          //std::cerr << "Wrong window!" << std::endl;
          return;
        }

#if HILDON_CHECK_VERSION(2,1,0)
        if (object!=NULL &&
            this->object==object &&
            object->RequestsKeyboard() &&
            object->GetIMHandler()!=NULL &&
            object->GetIMHandler()->IsInEditingArea(event)) {
          SendInputMode();
          ShowKeyboard();
        }
#endif
      }

      bool Keyboard::HandleEvent(OS::Window* window, OS::Event* event)
      {
        XEvent x11Event;

        X11::GetX11Event(event,x11Event);

        if (x11Event.type!=ClientMessage) {
          return false;
        }

        if (this->window!=window) {
          return false;
        }

        X11::Display *display=dynamic_cast<X11::Display*>(Lum::OS::display);

        if (display==NULL) {
          return false;
        }

        ::Window imWindow=GetKeyboardWindow();

        if (imWindow==None) {
          return false;
        }

        if (x11Event.xclient.message_type==XInternAtom(display->display,
                                                       HILDON_IM_COM_NAME,false)) {
          HildonIMComMessage *msg = (HildonIMComMessage *)&x11Event.xclient.data;

          if (msg->type==HILDON_IM_CONTEXT_HANDLE_ENTER) {
            SendKey(keyReturn);
            return true;
          }
          else if (msg->type==HILDON_IM_CONTEXT_HANDLE_TAB) {
            SendKey(keyTab);
            return true;
          }
          else if (msg->type==HILDON_IM_CONTEXT_HANDLE_BACKSPACE) {
            SendKey(keyBackspace);
            return true;
          }
          else if (msg->type==HILDON_IM_CONTEXT_HANDLE_SPACE) {
            SendKey(keySpace);
            return true;
          }
          else if (msg->type==HILDON_IM_CONTEXT_CONFIRM_SENTENCE_START) {
            //std::cout << "HILDON_IM_CONTEXT_CONFIRM_SENTENCE_START" << std::endl;
            if (!SendCommand(HILDON_IM_LOW)) {
              std::cerr << "Error while sending LOW (_HILDON_IM_ACTIVATE)" << std::endl;
        }
          }
          else if (msg->type==HILDON_IM_CONTEXT_FLUSH_PREEDIT) {
            //std::cout << "HILDON_IM_CONTEXT_FLUSH_PREEDIT" << std::endl;
          }
          else if (msg->type==HILDON_IM_CONTEXT_CANCEL_PREEDIT) {
            //std::cout << "HILDON_IM_CONTEXT_CANCEL_PREEDIT" << std::endl;
          }
          else if (msg->type==HILDON_IM_CONTEXT_BUFFERED_MODE) {
            //std::cout << "HILDON_IM_CONTEXT_BUFFERED_MODE" << std::endl;
          }
          else if (msg->type==HILDON_IM_CONTEXT_DIRECT_MODE) {
            //std::cout << "HILDON_IM_CONTEXT_DIRECT_MODE" << std::endl;
          }
          else if (msg->type==HILDON_IM_CONTEXT_REDIRECT_MODE) {
            //std::cout << "HILDON_IM_CONTEXT_REDIRECT_MODE" << std::endl;
          }
          else if (msg->type==HILDON_IM_CONTEXT_SURROUNDING_MODE) {
            //std::cout << "HILDON_IM_CONTEXT_SURROUNDING_MODE" << std::endl;
          }
          else if (msg->type==HILDON_IM_CONTEXT_PREEDIT_MODE) {
            //std::cout << "HILDON_IM_CONTEXT_PREEEDIT_MODE" << std::endl;
          }
          else if (msg->type==HILDON_IM_CONTEXT_CLIPBOARD_COPY) {
            //std::cout << "HILDON_IM_CONTEXT_CLIPBOARD_COPY" << std::endl;
            Copy();
          }
          else if (msg->type==HILDON_IM_CONTEXT_CLIPBOARD_CUT) {
            //std::cout << "HILDON_IM_CONTEXT_CLIPBOARD_CUT" << std::endl;
            Cut();
          }
          else if (msg->type==HILDON_IM_CONTEXT_CLIPBOARD_PASTE) {
            //std::cout << "HILDON_IM_CONTEXT_CLIPBOARD_PASTE" << std::endl;
            Paste();
          }
          else if (msg->type==HILDON_IM_CONTEXT_CLIPBOARD_SELECTION_QUERY) {
            //std::cout << "HILDON_IM_CONTEXT_CLIPBOARD_SELECTION_QUERY" << std::endl;
            SendClipboardSelectionQueryResponse();
          }
          else if (msg->type==HILDON_IM_CONTEXT_REQUEST_SURROUNDING) {
            //std::cout << "HILDON_IM_CONTEXT_REQUEST_SURROUNDING" << std::endl;
          }
#if HILDON_CHECK_VERSION(2,1,0)
          else if (msg->type==HILDON_IM_CONTEXT_REQUEST_SURROUNDING_FULL) {
            //std::cout << "HILDON_IM_CONTEXT_REQUEST_SURROUNDING_FULL" << std::endl;
            SendSurrounding();
          }
#endif
          else if (msg->type==HILDON_IM_CONTEXT_WIDGET_CHANGED) {
            //std::cout << "HILDON_IM_CONTEXT_WIDGET_CHANGED" << std::endl;
          }
          else if (msg->type==HILDON_IM_CONTEXT_OPTION_CHANGED) {
            //std::cout << "HILDON_IM_CONTEXT_OPTION_CHANGED" << std::endl;
          }
          /*else if (msg->type==HILDON_IM_CONTEXT_CLEAR_STICKY) {
            //std::cout << "HILDON_IM_CONTEXT_CLEAR_STICKY" << std::endl;
          }*/
          else if (msg->type==HILDON_IM_CONTEXT_ENTER_ON_FOCUS) {
            //std::cout << "HILDON_IM_CONTEXT_ENTER_ON_FOCUS" << std::endl;
          }
          else if (msg->type==HILDON_IM_CONTEXT_SPACE_AFTER_COMMIT) {
            //std::cout << "HILDON_IM_CONTEXT_SPACE_AFTER_COMMIT" << std::endl;
          }
          else if (msg->type==HILDON_IM_CONTEXT_NO_SPACE_AFTER_COMMIT) {
            //std::cout << "HILDON_IM_CONTEXT_NO_SPACE_AFTER_COMMIT" << std::endl;
          }
          else if (msg->type==HILDON_IM_CONTEXT_SHIFT_LOCKED) {
            //std::cout << "HILDON_IM_CONTEXT_SHIFT_LOCKED" << std::endl;
          }
          else if (msg->type==HILDON_IM_CONTEXT_SHIFT_UNLOCKED) {
            //std::cout << "HILDON_IM_CONTEXT_SHIFT_UNLOCKED" << std::endl;
          }
          else if (msg->type==HILDON_IM_CONTEXT_LEVEL_LOCKED) {
            //std::cout << "HILDON_IM_CONTEXT_LEVEL_LOCKED" << std::endl;
          }
          else if (msg->type==HILDON_IM_CONTEXT_LEVEL_UNLOCKED) {
            //std::cout << "HILDON_IM_CONTEXT_LEVEL_UNLOCKED" << std::endl;
          }
          else if (msg->type==HILDON_IM_CONTEXT_SHIFT_UNSTICKY) {
            //std::cout << "HILDON_IM_CONTEXT_SHIFT_UNSTICKY" << std::endl;
          }
          else if (msg->type==HILDON_IM_CONTEXT_LEVEL_UNSTICKY) {
            //std::cout << "HILDON_IM_CONTEXT_LEVEL_UNSTICKY" << std::endl;
          }
          else {
            //std::cout << "got HILDON-COM " << std::hex << msg->type << std::endl;
          }
        }
        else if (x11Event.xclient.message_type==XInternAtom(display->display,
                                                            HILDON_IM_INSERT_UTF8_NAME,
                                                            false)) {
          //std::cout << "HILDON_IM_INSERT_UTF8" << std::endl;
          X11::KeyEvent keyEvent;
          std::string   inputText;
          size_t        i;

          // Decode text
          i=4;
          while (i<20 && x11Event.xclient.data.b[i]!='\0') {
            inputText.append(1,(char)x11Event.xclient.data.b[i]);
            i++;
          }

          // Fake key event
          keyEvent.key=keyUnknown;
          keyEvent.qualifier=0;
          keyEvent.text=Lum::Base::UTF8ToWString(inputText);
          keyEvent.type=KeyEvent::down;

          window->HandleEvent(&keyEvent);

          keyEvent.type=KeyEvent::up;

          window->HandleEvent(&keyEvent);
        }
        else if (x11Event.xclient.message_type==XInternAtom(display->display,
                                                            HILDON_IM_SURROUNDING_CONTENT_NAME,
                                                            false)) {
          HildonIMSurroundingContentMessage *msg=reinterpret_cast<HildonIMSurroundingContentMessage *>(&x11Event.xclient.data);

          //std::cout << "HILDON_IM_SURROUNDING_CONTENT " << msg->msg_flag << " '" << msg->surrounding << "'" << std::endl;

          if (msg->msg_flag==HILDON_IM_MSG_START) {
            buffer.clear();
          }

          buffer+=msg->surrounding;


          if (msg->msg_flag==HILDON_IM_MSG_END) {
            assert(object!=NULL);
            assert(object->GetIMHandler()!=NULL);
            object->GetIMHandler()->SetData(Lum::Base::UTF8ToWString(buffer));
          }
        }
        else if (x11Event.xclient.message_type==XInternAtom(display->display,
                                                            HILDON_IM_SURROUNDING_NAME,
                                                            false)) {

          HildonIMSurroundingMessage *msg = reinterpret_cast<HildonIMSurroundingMessage *>(&x11Event.xclient.data);

          //std::cout << "HILDON_IM_SURROUNDING " << msg->commit_mode << " "  << msg->offset_is_relative << " " << msg->cursor_offset << std::endl;

          assert(object!=NULL);
          assert(object->GetIMHandler()!=NULL);

          object->GetIMHandler()->SetCursorOffset(msg->cursor_offset);
        }
        /*else if (x11Event.xclient.message_type==XInternAtom(display->display,
                                                            HILDON_IM_INPUT_MODE_NAME,
                                                            false)) {
          //std::cout << "HILDON_IM_INPUT_MODE" << std::endl;
        }*/
        else if (x11Event.xclient.message_type==XInternAtom(display->display,
                                                            HILDON_IM_KEY_EVENT_NAME,
                                                            false)) {
          //std::cout << "HILDON_IM_KEY_EVENT" << std::endl;
        }
        /*else if (x11Event.xclient.message_type==XInternAtom(display->display,
                                                            HILDON_IM_PREEDIT_COMMITTED_NAME,
                                                            false)) {
          //std::cout << "HILDON_IM_PREEDIT_COMMITTED" << std::endl;
        }
        else if (x11Event.xclient.message_type==XInternAtom(display->display,
                                                            HILDON_IM_PREEDIT_COMMITTED_CONTENT_NAME,
                                                            false)) {
          //std::cout << "HILDON_IM_PREEDIT_COMMITTED_CONTENT" << std::endl;
        }*/

        return false;
      }

      void Keyboard::SendInputMode()
      {
        //std::cout << "Keyboard::SendInputMode()" << std::endl;
        ::XEvent     event;
        ::Window     imWindow=GetKeyboardWindow();
        X11::Display *display=dynamic_cast<X11::Display*>(Lum::OS::display);
        std::wstring inputTypeHint;

        if (display==NULL) {
          return;
        }

        if (imWindow==None) {
          return;
        }

        assert(object!=NULL);
        assert(object->GetIMHandler()!=NULL);

        inputTypeHint=object->GetInputTypeHint(),

        memset(&event,0,sizeof(event));
        event.xclient.type=ClientMessage;
        event.xclient.window=imWindow;
        event.xclient.message_type=XInternAtom(display->display,
                                               HILDON_IM_INPUT_MODE_NAME,
                                               false);
        event.xclient.format=HILDON_IM_INPUT_MODE_FORMAT;

        HildonIMInputModeMessage *msg=reinterpret_cast<HildonIMInputModeMessage *>(&event.xclient.data);

        msg->input_mode=HILDON_GTK_INPUT_MODE_FULL;

        if (inputTypeHint==Lum::Object::inputTypeAlpha) {
          msg->input_mode=HILDON_GTK_INPUT_MODE_ALPHA;
        }
        else if (inputTypeHint==Lum::Object::inputTypeUnsignedBinaryNumber) {
          msg->input_mode=HILDON_GTK_INPUT_MODE_NUMERIC;
        }
        else if (inputTypeHint==Lum::Object::inputTypeUnsignedOctalNumber) {
          msg->input_mode=HILDON_GTK_INPUT_MODE_NUMERIC;
        }
        else if (inputTypeHint==Lum::Object::inputTypeSignedDecimalNumber) {
          msg->input_mode=HILDON_GTK_INPUT_MODE_NUMERIC;
        }
        else if (inputTypeHint==Lum::Object::inputTypeUnsignedDecimalNumber) {
          msg->input_mode=HILDON_GTK_INPUT_MODE_NUMERIC;
        }
        else if (inputTypeHint==Lum::Object::inputTypeUnsignedHexadecimalNumber) {
          msg->input_mode=HILDON_GTK_INPUT_MODE_HEXA;
        }
        else if (inputTypeHint==Lum::Object::inputTypeSignedFloatNumber) {
          msg->input_mode=HILDON_GTK_INPUT_MODE_FULL;
        }
        else if (inputTypeHint==Lum::Object::inputTypeUnsignedFloatNumber) {
          msg->input_mode=HILDON_GTK_INPUT_MODE_FULL;
        }
        else if (inputTypeHint==Lum::Object::inputTypePhoneNumber) {
          msg->input_mode=HILDON_GTK_INPUT_MODE_TELE;
        }

        msg->default_input_mode=HILDON_GTK_INPUT_MODE_FULL;

        if (XSendEvent(display->display,imWindow,False,NoEventMask,&event)==0) {
          std::cerr << "Error while sending OpenKeyboard (_HILDON_IM_INPUT_MODE)" << std::endl;
        }
      }

      void Keyboard::SendSurrounding()
      {
        //std::cout << "Keyboard::SendSurrounding()" << std::endl;

        ::XEvent     event;
        std::wstring data;
        std::string  utf8Data;
        size_t       cursorPos;
        size_t       currentPos;
        size_t       length;
        ::Window     imWindow=GetKeyboardWindow();
        X11::Display *display=dynamic_cast<X11::Display*>(Lum::OS::display);

        if (display==NULL) {
          return;
        }

        if (imWindow==None) {
          return;
        }

        assert(object!=NULL);
        assert(object->GetIMHandler()!=NULL);

        object->GetIMHandler()->GetData(data,cursorPos);

        utf8Data=Lum::Base::WStringToUTF8(data);

        memset(&event,0,sizeof(event));
        event.xclient.type=ClientMessage;
        event.xclient.window=imWindow;
        event.xclient.message_type=XInternAtom(display->display,
                                               HILDON_IM_SURROUNDING_CONTENT_NAME,
                                               false);
        event.xclient.format=HILDON_IM_SURROUNDING_CONTENT_FORMAT;

        HildonIMSurroundingContentMessage *msg = reinterpret_cast<HildonIMSurroundingContentMessage *>(&event.xclient.data);

        currentPos=0;
        length=std::min(utf8Data.length(),
                        HILDON_IM_CLIENT_MESSAGE_BUFFER_SIZE);

        msg->msg_flag=HILDON_IM_MSG_START;
        strcpy(msg->surrounding,utf8Data.substr(currentPos,length).c_str());

        if (XSendEvent(display->display,imWindow,False,NoEventMask,&event)==0) {
          std::cerr << "Error while sending HILDON_IM_SURROUNDING_CONTENT" << std::endl;
        }

        if (utf8Data.length()>0) {
          currentPos+=length;

          while (currentPos<utf8Data.length()-1) {
          length=std::min(utf8Data.length()-currentPos,
                          HILDON_IM_CLIENT_MESSAGE_BUFFER_SIZE);

            msg->msg_flag=HILDON_IM_MSG_CONTINUE;
            strcpy(msg->surrounding,utf8Data.substr(currentPos,length).c_str());

            if (XSendEvent(display->display,imWindow,False,NoEventMask,&event)==0) {
              std::cerr << "Error while sending HILDON_IM_SURROUNDING_CONTENT" << std::endl;
            }

            currentPos+=length;
          }
        }

        msg->msg_flag=HILDON_IM_MSG_END;
        msg->surrounding[0]='\0';

        if (XSendEvent(display->display,imWindow,False,NoEventMask,&event)==0) {
          std::cerr << "Error while sending HILDON_IM_SURROUNDING_CONTENT_NAME" << std::endl;
        }

        memset(&event,0,sizeof(event));
        event.xclient.type=ClientMessage;
        event.xclient.window=imWindow;
        event.xclient.message_type=XInternAtom(display->display,
                                               HILDON_IM_SURROUNDING_NAME,
                                               false);
        event.xclient.format=HILDON_IM_SURROUNDING_FORMAT;

        HildonIMSurroundingMessage *smsg = reinterpret_cast<HildonIMSurroundingMessage *>(&event.xclient.data);

        smsg->commit_mode=HILDON_IM_COMMIT_SURROUNDING;
        smsg->offset_is_relative=false;
        smsg->cursor_offset=cursorPos;

        if (XSendEvent(display->display,imWindow,False,NoEventMask,&event)==0) {
          std::cerr << "Error while sending HILDON_IM_SURROUNDING" << std::endl;
        }
      }

      void Keyboard::SendClipboardSelectionQueryResponse()
      {
        ::XEvent     event;
        bool         hasSelection;
        ::Window     imWindow=GetKeyboardWindow();
        X11::Display *display=dynamic_cast<X11::Display*>(Lum::OS::display);

        if (display==NULL) {
          return;
        }

        if (imWindow==None) {
          return;
        }

        hasSelection=object!=NULL &&
                     object->CanExecuteAction(Lum::Base::Datatype(Lum::Base::Datatype::text,
                                                                  Lum::Base::Datatype::plain),
                                              Lum::Base::DataExchangeObject::actionCopy);

        memset(&event,0,sizeof(event));
        event.xclient.type=ClientMessage;
        event.xclient.window=imWindow;
        event.xclient.message_type=XInternAtom(display->display,
                                               HILDON_IM_CLIPBOARD_SELECTION_REPLY_NAME,
                                               false);
        event.xclient.format=HILDON_IM_CLIPBOARD_SELECTION_REPLY_FORMAT;
        event.xclient.data.l[0]=hasSelection;

        if (XSendEvent(display->display,imWindow,False,NoEventMask,&event)==0) {
          std::cerr << "Error while sending HILDON_IM_CLIPBOARD_SELECTION_REPLY" << std::endl;
        }
      }

      void Keyboard::SendKey(Key key)
      {
        //std::cout << "Keyboard::SendKey(" << key << ")" << std::endl;
        X11::KeyEvent keyEvent;

        keyEvent.key=key;
        keyEvent.qualifier=0;
        keyEvent.type=KeyEvent::down;

        window->HandleEvent(&keyEvent);

        keyEvent.type=KeyEvent::up;

        window->HandleEvent(&keyEvent);
      }

      bool Keyboard::SendCommand(HildonIMCommand command)
      {
        assert(window!=0);

        X11::Display *display=dynamic_cast<X11::Display*>(Lum::OS::display);

        if (display==NULL) {
          std::cerr << "No display!" << std::endl;
          return false;
        }

        ::Window imWindow=GetKeyboardWindow();

        if (imWindow==None) {
          std::cerr << "No IM window!" << std::endl;
          return false;
        }

        ::XEvent event;

        memset(&event,0,sizeof(event));
        event.xclient.type=ClientMessage;
        event.xclient.window=imWindow;
        event.xclient.message_type=XInternAtom(display->display,
                                               HILDON_IM_ACTIVATE_NAME,
                                               false);
        event.xclient.format=HILDON_IM_ACTIVATE_FORMAT;

        HildonIMActivateMessage *msg = reinterpret_cast<HildonIMActivateMessage *>(&event.xclient.data);

        msg->input_window=dynamic_cast<X11::Window*>(window)->GetDrawable();
        msg->app_window=dynamic_cast<X11::Window*>(window)->GetDrawable();
        msg->cmd=command;
        msg->trigger=triggerType;

        return XSendEvent(display->display,imWindow,False,NoEventMask,&event)!=0;
      }

      void Keyboard::ShowKeyboard()
      {
        //std::cout << "Keyboard::ShowKeyboard()" << std::endl;

        if (!SendCommand(HILDON_IM_SETNSHOW)) {
          std::cerr << "Error while sending OpenKeyboard (_HILDON_IM_ACTIVATE)" << std::endl;
        }
      }

      void Keyboard::HideKeyboard()
      {
        //std::cout << "Keyboard::HideKeyboard()" << std::endl;
        if (!SendCommand(HILDON_IM_HIDE)) {
          std::cerr << "Error while sending 'CloseKeyboard' (_HILDON_IM_ACTIVATE)" << std::endl;
        }
      }

      ::Window Keyboard::GetKeyboardWindow() const
      {
        X11::Display *display=dynamic_cast<X11::Display*>(Lum::OS::display);

        if (display==NULL) {
          return None;
        }

        Atom hildonWindow=XInternAtom(display->display,"_HILDON_IM_WINDOW",true);

        if (hildonWindow==None) {
          return None;
        }

        Atom           atomType;
        int            atomFormat;
        int            res;
        unsigned long  atomItems;
        unsigned long  atomBytes;
        unsigned char* atomProperties;
        ::Window       window;

        res=XGetWindowProperty(display->display,RootWindow(display->display,display->scrNum),
                               hildonWindow,0,1,false,display->atoms[display->windowAtom],
                               &atomType,&atomFormat,&atomItems,&atomBytes,&atomProperties);
        if (res!=Success ||
            atomType!=XA_WINDOW ||
            atomFormat!=HILDON_IM_WINDOW_ID_FORMAT ||
            atomItems!=1 ||
            atomProperties==NULL) {
          std::cerr << "Cannot get IM window (_HILDON_IM_WINDOW)" << std::endl;
          return None;
        }

        window=reinterpret_cast< ::Window*>(atomProperties)[0];

        XFree(atomProperties);

        return window;
      }

      void Keyboard::Cut()
      {
        Lum::Base::DataExchangeObject *object=OS::display->GetSelectionOwner();

        if (object==NULL) {
          return;
        }

        Lum::Base::Data data;

        if (!object->ExecuteAction(Lum::Base::Datatype(Lum::Base::Datatype::text,
                                                       Lum::Base::Datatype::plain),
                                   data,
                                   Lum::Base::DataExchangeObject::actionMove)) {
          return;
        }

        OS::display->SetClipboard(data.GetText());

        object->PostprocessAction(Lum::Base::DataExchangeObject::actionMove);
      }

      void Keyboard::Copy()
      {
        Lum::Base::DataExchangeObject *object=OS::display->GetSelectionOwner();

        if (object==NULL) {
          return;
        }

        Lum::Base::Data data;

        if (!object->ExecuteAction(Lum::Base::Datatype(Lum::Base::Datatype::text,
                                                       Lum::Base::Datatype::plain),
                                   data,
                                   Lum::Base::DataExchangeObject::actionCopy)) {
          return;
        }

        OS::display->SetClipboard(data.GetText());
      }

      void Keyboard::Paste()
      {
        if (object==NULL) {
          return;
        }

        std::wstring clipboard;

        clipboard=OS::display->GetClipboard();

        if (clipboard.empty()) {
          return;
        }

        Lum::Base::Data data(clipboard);

        object->ExecuteAction(Lum::Base::Datatype(Lum::Base::Datatype::text,
                                                  Lum::Base::Datatype::plain),
                              data,
                              Lum::Base::DataExchangeObject::actionInsert);
      }
    }
  }
}

