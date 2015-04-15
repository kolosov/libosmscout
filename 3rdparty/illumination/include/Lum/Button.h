#ifndef LUM_BUTTON_H
#define LUM_BUTTON_H

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

#include <Lum/Base/Size.h>

#include <Lum/Model/Action.h>

#include <Lum/OS/Theme.h>

#include <Lum/Object.h>

namespace Lum {

  /**
    Implements a powerfull general purpose button class with
    "action on press" semantic. The button label can either be
    text or any other class derived from @otype{G.Object}.

    The button can be on of a number of types that slidly differ
    in their apearance.

    Instead of the "one action on press" semantic, it can be
    configured to retrigger until the button is deselected again.
  */
  class LUMAPI Button : public Control
  {
  public:
    enum Type {
      typeNormal,     //! normal button
      typeDefault,
      typeCommit,
      typeCancel,
      typeIcon,
      typeAdditional, //! Additional helper buttons, possibly drawn smaller than normal buttons
      typeToolBar,    //! a tool bar button
      typeScroll
    };

  private:
    Object           *object;       //! The label of the button
    OS::ImageRef     image;         //! Optional image (depending on type) to be displayed with the object
    OS::FontRef      font;          //! Font used
    Type             type;          //! Type
    bool             pulse;         //! Button creates repeately press events while clicked
    wchar_t          shortCut;
    bool             scAssigned;
    Model::ActionRef model;         //! The action to trigger on press
    int              imageX,imageY; //! Position of image
    bool             triggering;    //! Flag signaling, that we are triggering the model (and not somebody else)

  private:
    bool ShowObject() const;
    bool ShowImage() const;

  public:
    Button();
    ~Button();

    bool HasBaseline() const;
    size_t GetBaseline() const;

    Button* SetFont(OS::Font* font);
    Button* SetLabel(Object* object);
    Button* SetImage(OS::Image* image);
    Button* SetText(const std::wstring& string);
    Button* SetType(Type type);
    Button* SetPulse(bool pulse);

    bool SetModel(Base::Model* model);

    void CalcSize();
    void Layout();
    void PrepareForBackground(OS::DrawInfo* draw);
    void Draw(OS::DrawInfo* draw,
              int x, int y, size_t w, size_t h);
    void Hide();

    bool HandleMouseEvent(const OS::MouseEvent& event);
    bool HandleKeyEvent(const OS::KeyEvent& event);

    void Resync(Base::Model* model, const Base::ResyncMsg& msg);

    static Button* Create(Object* object,
                          Base::Model* action=NULL,
                          bool horizontalFlex=false, bool verticalFlex=false);
    static Button* Create(const std::wstring& text,
                          bool horizontalFlex=false, bool verticalFlex=false);
    static Button* Create(const std::wstring& text,
                          Base::Model* action,
                          bool horizontalFlex=false, bool verticalFlex=false);
    static Button* Create(const std::wstring& text,
                          OS::Image* image,
                          Base::Model* action,
                          bool horizontalFlex=false, bool verticalFlex=false);
    static Button* Create(const std::wstring& text,
                          Base::Model* action,
                          Type type,
                          bool horizontalFlex=false, bool verticalFlex=false);
    static Button* Create(const std::wstring& text,
                          OS::Image* image,
                          Base::Model* action,
                          Type type,
                          bool horizontalFlex=false, bool verticalFlex=false);
    static Button* Create(const std::wstring& text,
                          Lum::OS::Theme::StockImage image,
                          Base::Model* action,
                          Type type,
                          bool horizontalFlex=false, bool verticalFlex=false);
    static Button* Create(OS::Image* image,
                          Base::Model* action,
                          Type type,
                          bool horizontalFlex=false, bool verticalFlex=false);

    static Button* CreateOk(Model::Action* action,
                            bool horizontalFlex=false, bool verticalFlex=false);
    static Button* CreateCancel(Model::Action* action,
                                bool horizontalFlex=false, bool verticalFlex=false);
    static Button* CreateClose(Model::Action* action,
                               bool horizontalFlex=false, bool verticalFlex=false);
    static Button* CreateQuit(Model::Action* action,
                              bool horizontalFlex=false, bool verticalFlex=false);
    static Button* CreateHelp(Model::Action* action,
                              bool horizontalFlex=false, bool verticalFlex=false);
  };
}

#endif
