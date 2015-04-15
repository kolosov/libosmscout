#ifndef LUM_COMBO_H
#define LUM_COMBO_H

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
#include <Lum/Model/Header.h>
#include <Lum/Model/Number.h>
#include <Lum/Model/Selection.h>
#include <Lum/Model/String.h>
#include <Lum/Model/Table.h>

#include <Lum/Object.h>
#include <Lum/Popup.h>
#include <Lum/String.h>
#include <Lum/Table.h>

namespace Lum {
  /**
    Abstract base class for combo box like objects.
    A combo box is a object that shows one currently selected
    value (which depending on implementation might even be directly
    editable) and offers a popup window to offers a list
    of alternative values.

    The baseclass offers a framework for such functionality.
    It defines the needed models, the necessary drawing
    methods, the handling for opening and closing the
    popup window, callbacks for setting the new value on
    selection etc..
  */
  class LUMAPI Combo : public Control
  {
  private:
    /**
      The selection popup
      */
    class ComboPopup : public Popup
    {
    public:
      Combo *combo;
      bool  success;

    public:
      ComboPopup(Combo *combo);
      ~ComboPopup();
      void PreInit();
      void Resync(Base::Model* model, const Base::ResyncMsg& msg);
    };

  private:
    bool                          imageRight;   //! If true, image should be displayed to the right (else left)
    bool                          editable;     //! If true, the combobox is editable
    Table                         *table;       //! The table control in the popup
    ComboPopup*                   popup;        //! The selection popup

  protected:
    Model::ActionRef              prevAction;   //! Model trigger if previous element should be selected
    Model::ActionRef              nextAction;   //! Model trigger if next element should be selected
    Model::HeaderRef              headerModel;  //! The header model used for the table in the popup
    Model::TableRef               tableModel;   //! The table model used for the table in the popup
    TableCellPainterRef           tablePainter; //! The table painter for the popup table
    Model::SingleLineSelectionRef selection;
    Object                        *value;       //! Value editing control for read-write combobox

  private:
    void OpenPopup();
    void SetTableRow(size_t row);

  protected:
    Combo(Object* value, bool editable);
    virtual ~Combo();

    void PreparePainter(ValuePainter* painter);

    virtual void SetModelFromSelection() = 0;
    virtual void SetSelectionFromModel() = 0;

  public:
    bool HasBaseline() const;
    size_t GetBaseline() const;

    Object* GetValueObject() const;

    bool VisitChildren(Visitor &visitor, bool onlyVisible);

    void SetHeaderModel(Model::Header* model);
    void SetTableModel(Model::Table* model);
    void SetTableCellPainter(TableCellPainter *tablePainter);

    void CalcSize();
    void Layout();

    bool HandleMouseEvent(const OS::MouseEvent& event);
    bool HandleKeyEvent(const OS::KeyEvent& event);

    void PrepareForBackground(OS::DrawInfo* draw);
    void Draw(OS::DrawInfo* draw,
              int x, int y, size_t w, size_t h);
    void Hide();

    void Resync(Base::Model* model, const Base::ResyncMsg& msg);
  };

  class LUMAPI TextCombo : public Combo
  {
  private:
    Model::StringRef model; /** The model for the current value */

  protected:
    void SetModelFromSelection();
    void SetSelectionFromModel();

  public:
    TextCombo();
    ~TextCombo();

    bool SetModel(Base::Model* model);
  };

  class LUMAPI IndexCombo : public Combo
  {
  private:
    Model::NumberRef model; /** The model for the current value */

  protected:
    void SetModelFromSelection();
    void SetSelectionFromModel();

  public:
    IndexCombo();
    ~IndexCombo();

    bool SetModel(Base::Model* model);

    static IndexCombo* Create(Lum::Model::Number* index, Lum::Model::Table* table,
                              bool horizontalFlex=false, bool verticalFlex=false);
  };

  class LUMAPI TextEditCombo : public Combo
  {
  private:
    String           *string;
    Model::StringRef model; /** The model for the current value */

  protected:
    void SetModelFromSelection();
    void SetSelectionFromModel();

  public:
    TextEditCombo();
    ~TextEditCombo();

    bool SetModel(Base::Model* model);

    String* GetString() const;
  };
}

#endif
