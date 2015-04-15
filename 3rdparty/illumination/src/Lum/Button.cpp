
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

#include <Lum/Button.h>

#include <Lum/Base/DateTime.h>
#include <Lum/Base/L10N.h>

#include <Lum/OS/Theme.h>

#include <Lum/Dialog.h>
#include <Lum/Text.h>

namespace Lum {

  static size_t pulseSec  = 0;      // Make this a global preference value
  static size_t pulseMSec = 100000;

  Button::Button()
  : object(NULL),font(OS::display->GetFont()),type(typeNormal),
    pulse(false),shortCut('\0'),scAssigned(false),
    triggering(false)
  {
    SetBackground(OS::display->GetFill(OS::Display::buttonBackgroundFillIndex));

    if (!OS::display->GetTheme()->FocusOnEditableControlsOnly()) {
      SetCanFocus(true);
    }

    SetRedrawOnMouseActive(true);
  }

  Button::~Button()
  {
    delete object;
  }

  bool Button::HasBaseline() const
  {
    return object!=NULL && object->HasBaseline();
  }

  size_t Button::GetBaseline() const
  {
    assert(inited && object!=NULL && object->HasBaseline());

    return GetTopBorder()+object->GetBaseline();
  }

  /**
    Set a new font to be used by the button gadget.
  */
  Button* Button::SetFont(OS::Font *font)
  {
    this->font=font;

    return this;
  }

  /**
    Use this method if you do not want text displayed in the button but
    want to use some other Object as label
  */
  Button* Button::SetLabel(Object* label)
  {
    delete this->object;

    this->object=label;
    this->object->SetParent(this);

    return this;
  }

  /**
    Assign an additional image. Some buttons type will automatically assign an image.
  */
  Button* Button::SetImage(OS::Image* image)
  {
    this->image=image;

    return this;
  }

  /**
    Call this method if you want the given text to be displayed in
    the button.

    This creates an instance of Text using the given text
    and sets it as button label.

    use '_' to mark the next character as key shortcut.
  */
  Button* Button::SetText(const std::wstring& string)
  {
    Text   *text;
    size_t pos;

    text=new Text();
    text->SetParent(this);
    text->SetFlex(true,true);
    text->SetAlignment(Text::centered);

    pos=string.find('_');
    if (pos!=std::wstring::npos && pos+1<string.length()) {
      shortCut=string[pos+1];
      text->AddText(string.substr(0,pos));
      if (OS::display->GetTheme()->ShowKeyShortcuts()) {
        text->AddText(string.substr(pos+1,1),OS::Font::underlined);
      }
      else {
        text->AddText(string.substr(pos+1,1));
      }
      text->AddText(string.substr(pos+2));
    }
    else {
      text->AddText(string);
    }

    delete object;

    object=text;

    return this;
  }

  /**
    We can define special types of buttons.

    \see Type.
  */
  Button* Button::SetType(Type type)
  {
    this->type=type;

    return this;
  }

  /**
    Is pulsemode is true, the button sends permanent events on
    mouse button down and none on the final button up.

    This is usefull for buttons in a scroller or similar.
  */
  Button* Button::SetPulse(bool pulse)
  {
    this->pulse=pulse;

    return this;
  }

  bool Button::SetModel(Base::Model* model)
  {
    this->model=dynamic_cast<Model::Action*>(model);

    Control::SetModel(this->model);

    return this->model.Valid();
  }

  bool Button::ShowObject() const
  {
    return object!=NULL && !(image.Valid() && type==typeIcon);
  }

  bool Button::ShowImage() const
  {
    return image.Valid() && (OS::display->GetTheme()->ShowButtonImages() || type==typeIcon);
  }

  void Button::CalcSize()
  {
    /* Let the frame calculate its size */
    switch (type) {
      case typeNormal:
      SetBackground(OS::display->GetFill(OS::Display::buttonBackgroundFillIndex));
      if (!OS::display->GetTheme()->FocusOnEditableControlsOnly()) {
        RequestFocus();
      }

      if (OS::display->GetTheme()->RequestFingerFriendlyControls()) {
        SetMinWidth(OS::display->GetTheme()->GetFingerFriendlyMinWidth());
        SetMinHeight(OS::display->GetTheme()->GetFingerFriendlyMinHeight());
      }
      break;
    case typeDefault:
      SetBackground(OS::display->GetFill(OS::Display::defaultButtonBackgroundFillIndex));

      if (!OS::display->GetTheme()->FocusOnEditableControlsOnly()) {
        RequestFocus();
      }

      if (OS::display->GetTheme()->RequestFingerFriendlyControls()) {
        SetMinWidth(OS::display->GetTheme()->GetFingerFriendlyMinWidth());
        SetMinHeight(OS::display->GetTheme()->GetFingerFriendlyMinHeight());
      }
      break;
    case typeCommit:
      SetBackground(OS::display->GetFill(OS::Display::positiveButtonBackgroundFillIndex));

      if (!OS::display->GetTheme()->FocusOnEditableControlsOnly()) {
        RequestFocus();
      }

      if (OS::display->GetTheme()->RequestFingerFriendlyControls()) {
        SetMinWidth(OS::display->GetTheme()->GetFingerFriendlyMinWidth());
        SetMinHeight(OS::display->GetTheme()->GetFingerFriendlyMinHeight());
      }
      break;
    case typeCancel:
      SetBackground(OS::display->GetFill(OS::Display::negativeButtonBackgroundFillIndex));

      if (!OS::display->GetTheme()->FocusOnEditableControlsOnly()) {
        RequestFocus();
      }

      if (OS::display->GetTheme()->RequestFingerFriendlyControls()) {
        SetMinWidth(OS::display->GetTheme()->GetFingerFriendlyMinWidth());
        SetMinHeight(OS::display->GetTheme()->GetFingerFriendlyMinHeight());
      }
      break;
    case typeIcon:
      SetBackground(OS::display->GetFill(OS::Display::buttonBackgroundFillIndex));

      if (!OS::display->GetTheme()->FocusOnEditableControlsOnly()) {
        RequestFocus();
      }

      break;
    case typeAdditional:
      SetBackground(OS::display->GetFill(OS::Display::buttonBackgroundFillIndex));

      if (!OS::display->GetTheme()->FocusOnEditableControlsOnly()) {
        RequestFocus();
      }

      break;
    case typeToolBar:
      SetBackground(OS::display->GetFill(OS::Display::toolbarButtonBackgroundFillIndex));
      SetCanFocus(false);
      break;
    case typeScroll:
      SetBackground(OS::display->GetFill(OS::Display::scrollButtonBackgroundFillIndex));
      SetCanFocus(false);
      break;
    }

    width=0;
    height=0;
    minWidth=0;
    minHeight=0;

    if (ShowObject()) {
      /*
        Now we let the image calculate its bounds and simply add its size
        to the size of the button.
      */
      object->CalcSize();
      width=object->GetOWidth();
      height=object->GetOHeight();
      minWidth=object->GetOMinWidth();
      minHeight=object->GetOMinHeight();
    }

    if (ShowImage()) {
      minWidth+=image->GetWidth();
      width+=image->GetWidth();
      minHeight=std::max(minHeight,image->GetHeight());
      height=std::max(height,image->GetHeight());
    }

    if (ShowObject() && ShowImage()) {
      minWidth+=OS::display->GetSpaceHorizontal(OS::Display::spaceIntraObject);
      width+=OS::display->GetSpaceHorizontal(OS::Display::spaceIntraObject);
    }

    maxWidth=30000;
    maxHeight=30000;

    /* We *must* call CalcSize of our superclass! */
    Control::CalcSize();
  }

  bool Button::HandleMouseEvent(const OS::MouseEvent& event)
  {
    /* It makes no sense to get the focus if we are currently not visible */
    if (!visible || !model.Valid() || !model->IsEnabled()) {
      return false;
    }

    /*
      When the left mousebutton gets pressed without any qualifier
      in the bounds of our button...
    */

    if (event.type==OS::MouseEvent::down &&
        PointIsIn(event) &&
        event.button==OS::MouseEvent::button1) {
      if (!model->IsStarted()) {
        /* We change our state to pressed and redisplay ourself */
        triggering=true;
        model->Start();

        if (pulse) {
          model->Trigger();
          triggering=false;
          OS::display->AddTimer(pulseSec,pulseMSec,model);
        }

        /*
          Since we want the focus for waiting for buttonup we return
          a pointer to ourself.
        */
        return true;
      }
    }
    else if (event.IsGrabEnd()) {
      if (model->IsStarted()) {
        /*
          If the users released the left mousebutton over our bounds we really
          got selected.
        */
        if (PointIsIn(event)) {
          if (pulse) {
            model->Cancel();
            triggering=false;
          }
          else {
            model->Finish();
            triggering=false;
          }
        }
        else {
          model->Cancel();
          triggering=false;
        }
      }

      // Clean up and remove possibly remaining timer event.
      if (pulse) {
        OS::display->RemoveTimer(model);
      }
    }
    else if (event.type==OS::MouseEvent::move && event.IsGrabed()) {
      if (PointIsIn(event)) {
        if (!model->IsStarted()) {
          triggering=true;
          model->Start();
        }
      }
      else {
        if (model->IsStarted()) {
          model->Cancel();
          triggering=false;
        }
      }

      return true;
    }

    return false;
  }

  bool Button::HandleKeyEvent(const OS::KeyEvent& event)
  {
    if (event.type==OS::KeyEvent::down) {
      if (event.key==OS::keySpace && model->IsInactive()) {
        triggering=true;
        model->Start();

        return true;
      }
    }
    else if (event.type==OS::KeyEvent::up) {
      if (event.key==OS::keySpace) {
        if (event.qualifier==0 && model->IsStarted()) {
          model->Finish();
          triggering=false;
        }
        else if (event.qualifier!=0 && model->IsStarted()) {
          model->Cancel();
          triggering=false;
        }
      }
    }

    return false;
  }

  /*
     We tell the image to resize themself to
     our current bounds. Our bounds could have changed
     because Resize may have been called by some layout-objects
     between Button.CalcSize and Button.Draw.
   */
  void Button::Layout()
  {
    int    x,y;
    size_t width,height;

    x=this->x;
    y=this->y;
    width=this->width;
    height=this->height;

    if (ShowImage()) {
      imageX=x;
      imageY=y+(height-image->GetHeight())/2;
      x+=image->GetWidth();
      width-=image->GetWidth();
    }

    if (ShowObject()) {
      if (ShowImage()) {
        x+=OS::display->GetSpaceHorizontal(OS::Display::spaceIntraObject);
        width-=OS::display->GetSpaceHorizontal(OS::Display::spaceIntraObject);
      }
      object->Resize(width,height);
      object->Move(x+(width-object->GetOWidth()) / 2,
                 y+(height-object->GetOHeight()) / 2);
      object->Layout();
    }

    Control::Layout();
  }

  void Button::PrepareForBackground(OS::DrawInfo* draw)
  {
    if (model.Valid() && model->IsEnabled()) {
      if (IsMouseActive() && !model->IsStarted()) {
        draw->activated=true;
      }
      if (triggering && model->IsStarted()) {
        draw->selected=true;
      }
    }
    else {
      draw->disabled=true;
    }

    draw->focused=HasFocus();
  }

  void Button::Draw(OS::DrawInfo* draw,
                    int x, int y, size_t w, size_t h)
  {
    Control::Draw(draw,x,y,w,h); /* We must call Draw of our superclass */

    if (!OIntersect(x,y,w,h)) {
      return;
    }

    /*
      Set the correct draw mode before calling the baseclass,
      since the baseclass draw the object frame.
    */
    if (model.Valid() && model->IsEnabled()) {
      if (IsMouseActive() && !model->IsStarted()) {
        draw->activated=true;
      }
      if (triggering && model->IsStarted()) {
        draw->selected=true;
      }
    }
    else {
      draw->disabled=true;
    }

    draw->focused=HasFocus();

    // Draw object
    if (ShowObject()) {
      object->Draw(draw,x,y,w,h);
    }

    // Draw optional image
    if (ShowImage()) {
      image->Draw(draw,imageX,imageY);
    }

    draw->activated=false;
    draw->selected=false;
    draw->disabled=false;
    draw->focused=false;

    if (!scAssigned) {
      Dialog* window;

      assert(GetWindow()!=NULL);

      window=dynamic_cast<Dialog*>(GetWindow()->GetMaster());
      if (window!=NULL) {
        if (shortCut!='\0') {
          window->RegisterShortcut(this,0,std::wstring(1,shortCut),model);
          window->RegisterShortcut(this,OS::qualifierAlt,std::wstring(1,shortCut),model);
        }

        switch (type) {
        case typeCommit:
          window->RegisterCommitShortcut(this,model);
          break;
        case typeCancel:
          window->RegisterCancelShortcut(this,model);
          break;
        case typeDefault:
          window->RegisterDefaultShortcut(this,model);
          break;
        default:
          break;
        }
      }
      scAssigned=true;
    }
 }

  void Button::Hide()
  {
    if (visible) {
      if (object!=NULL) {
        /* Hide the image */
        object->Hide();
      }
      /* hide the frame */
      Control::Hide();
    }
  }

  void Button::Resync(Base::Model* model, const Base::ResyncMsg& msg)
  {
    if (model==this->model && visible) {
      if (this->model->IsInactive()) {
        Redraw();
      }
      else if (this->model->IsStarted()) {
        Redraw();
      }
      else if (this->model->IsFinished()) {
        if (pulse) {
          OS::display->RemoveTimer(this->model);
          OS::display->AddTimer(pulseSec,pulseMSec,this->model);
        }
      }
    }

    Control::Resync(model,msg);
  }

  Button* Button::Create(Object* object,
                         Base::Model* action,
                         bool horizontalFlex, bool verticalFlex)
  {
    Button *button;

    button=new Button();
    button->SetFlex(horizontalFlex,verticalFlex);
    button->SetModel(action);
    button->SetLabel(object);

    return button;
  }

  Button* Button::Create(const std::wstring& text,
                         bool horizontalFlex, bool verticalFlex)
  {
    Button *button;

    button=new Button();
    button->SetFlex(horizontalFlex,verticalFlex);
    button->SetText(text);

    return button;
  }

  Button* Button::Create(const std::wstring& text,
                         Base::Model* action,
                         bool horizontalFlex, bool verticalFlex)
  {
    Button *button;

    button=new Button();
    button->SetFlex(horizontalFlex,verticalFlex);
    button->SetModel(action);
    button->SetText(text);

    return button;
  }

  Button* Button::Create(const std::wstring& text,
                         OS::Image* image,
                         Base::Model* action,
                         bool horizontalFlex, bool verticalFlex)
  {
    Button *button;

    button=new Button();
    button->SetFlex(horizontalFlex,verticalFlex);
    button->SetModel(action);
    button->SetText(text);
    button->SetImage(image);

    return button;
  }

  Button* Button::Create(const std::wstring& text,
                         Base::Model* action,
                         Type type,
                         bool horizontalFlex, bool verticalFlex)
  {
    Button *button;

    button=new Button();
    button->SetFlex(horizontalFlex,verticalFlex);
    button->SetModel(action);
    button->SetText(text);
    button->SetType(type);

    return button;
  }

  Button* Button::Create(const std::wstring& text,
                         OS::Image* image,
                         Base::Model* action,
                         Type type,
                         bool horizontalFlex, bool verticalFlex)
  {
    Button *button;

    button=new Button();
    button->SetFlex(horizontalFlex,verticalFlex);
    button->SetModel(action);
    button->SetText(text);
    button->SetImage(image);
    button->SetType(type);

    return button;
  }

  Button* Button::Create(const std::wstring& text,
                         OS::Theme::StockImage image,
                         Base::Model* action,
                         Type type,
                         bool horizontalFlex, bool verticalFlex)
  {
    return Create(text,
                  OS::display->GetTheme()->GetStockImage(image),
                  action,
                  type,
                  horizontalFlex,
                  verticalFlex);
  }

  Button* Button::Create(OS::Image* image,
                         Base::Model* action,
                         Type type,
                         bool horizontalFlex, bool verticalFlex)
  {
    Button *button;

    button=new Button();
    button->SetFlex(horizontalFlex,verticalFlex);
    button->SetModel(action);
    button->SetImage(image);
    button->SetType(type);

    return button;
  }

  Button* Button::CreateOk(Model::Action* action, bool horizontalFlex, bool verticalFlex)
  {
    return Create(_ld(dlgButtonOk),
                  OS::Theme::imagePositive,
                  action,
                  Button::typeCommit,
                  horizontalFlex,verticalFlex);
  }

  Button* Button::CreateCancel(Model::Action* action, bool horizontalFlex, bool verticalFlex)
  {
    return Create(_ld(dlgButtonCancel),
                  OS::Theme::imageNegative,
                  action,
                  Button::typeCancel,
                  horizontalFlex,verticalFlex);
  }

  Button* Button::CreateClose(Model::Action* action, bool horizontalFlex, bool verticalFlex)
  {
    return Create(_ld(dlgButtonClose),
                  OS::Theme::imageDefault,
                  action,
                  Button::typeDefault,
                  horizontalFlex,verticalFlex);
  }

  Button* Button::CreateQuit(Model::Action* action, bool horizontalFlex, bool verticalFlex)
  {
    return Create(_ld(dlgButtonQuit),
                  OS::Theme::imageDefault,
                  action,
                  Button::typeDefault,
                  horizontalFlex,verticalFlex);
  }

  Button* Button::CreateHelp(Model::Action* action, bool horizontalFlex, bool verticalFlex)
  {
    return Create(_ld(dlgButtonHelp),
                  OS::Theme::imageHelp,
                  action,
                  Button::typeNormal,
                  horizontalFlex,verticalFlex);
  }
}
