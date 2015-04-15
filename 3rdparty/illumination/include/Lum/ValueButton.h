#ifndef LUM_VALUEBUTTON_H
#define LUM_VALUEBUTTON_H

/*
  This source is part of the Illumination library
  Copyright (C) 2009  Tim Teulings

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

#include <Lum/Dlg/ValuePopup.h>

#include <Lum/Model/Action.h>

#include <Lum/OS/Theme.h>

#include <Lum/Object.h>
#include <Lum/Text.h>

namespace Lum {

  /**
  */
  class LUMAPI ValueButton : public Control
  {
  private:
    Object           *label;        //! The label
    ValuePainterRef  painter;       //! Painter, painting the value
    wchar_t          shortCut;
    bool             scAssigned;
    Model::ActionRef action;        //! The action to trigger on press
    bool             triggering;    //! Flag signaling, that we are triggering the model (and not somebody else)
    Dlg::ValuePopup  *popup;        //! Popup dialog for value selection
    bool             paintingValue; //! If true, we are currently painting the value using the painter

  protected:
    ValueButton();
    void SetPainter(ValuePainter* painter);

    virtual void PreparePainter(ValuePainter* painter);

  public:
    ~ValueButton();

    bool HasBaseline() const;
    size_t GetBaseline() const;

    OS::Font* GetFont(const void* child) const;
    bool GetFontStyleSmart(const void* child) const;

    bool HasImplicitLabel() const;

    ValueButton* SetLabel(Object* object);
    ValueButton* SetLabel(const std::wstring& string);

    ValueButton* SetPopup(Dlg::ValuePopup* popup);

    void CalcSize();
    void Layout();
    void PrepareForBackground(OS::DrawInfo* draw);
    void Draw(OS::DrawInfo* draw,
              int x, int y, size_t w, size_t h);
    void Hide();

    bool HandleMouseEvent(const OS::MouseEvent& event);
    bool HandleKeyEvent(const OS::KeyEvent& event);

    void Resync(Base::Model* model, const Base::ResyncMsg& msg);
  };

  class LUMAPI DateValueButton : public ValueButton
  {
  public:
    DateValueButton();
  };

  class LUMAPI TimeValueButton : public ValueButton
  {
  private:
    Base::TimeFormat format;

  public:
    TimeValueButton(Base::TimeFormat format=Base::timeFormatDefault);
  };

  class LUMAPI TableStringValueButton : public ValueButton
  {
  private:
    Model::TableRef  tableModel;
    Model::HeaderRef headerModel;

  public:
    TableStringValueButton(Model::Table* table,
                           Model::Header* header=NULL);
  };

  class LUMAPI TableIndexValueButton : public ValueButton
  {
  private:
    Model::TableRef     tableModel;
    Model::HeaderRef    headerModel;

  protected:
    void PreparePainter(ValuePainter* painter);

  public:
    TableIndexValueButton(Model::Table* table,
                          TableCellPainter* painter,
                          Model::Header* header=NULL);

    static TableIndexValueButton* Create(const std::wstring& label,
                                         Base::Model* model,
                                         Model::Table* table,
                                         TableCellPainter* painter,
                                         bool horizontalFlex=false,
                                         bool verticalFlex=false);
  };
}

#endif
