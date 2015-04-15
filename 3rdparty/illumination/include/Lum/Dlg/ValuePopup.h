#ifndef LUM_DLG_VALUEPOPUP_H
#define LUM_DLG_VALUEPOPUP_H

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

#include <Lum/Popup.h>

#include <Lum/Model/Calendar.h>
#include <Lum/Model/Header.h>
#include <Lum/Model/Selection.h>
#include <Lum/Model/Table.h>
#include <Lum/Model/Time.h>

#include <Lum/DatePicker.h>
#include <Lum/Table.h>

namespace Lum {
  namespace Dlg {

    class ValuePopup : public Popup
    {
    private:
      Base::ModelRef model;

    public:
      ValuePopup* SetModel(Base::Model* model);

      Base::Model* GetModel() const;
    };

    /**
    */
    class DateValuePopup : public ValuePopup
    {
    private:
      DatePicker *picker;

    public:
      DateValuePopup();
      void PreInit();

      void Resync(Base::Model* model, const Base::ResyncMsg& msg);
    };

    /**
    */
    class TimeValuePopup : public ValuePopup
    {
    private:
      Base::TimeFormat format;
      Model::ActionRef commit;

    public:
      TimeValuePopup(Base::TimeFormat format=Base::timeFormatDefault);
      void PreInit();

      void Resync(Base::Model* model, const Base::ResyncMsg& msg);
    };

    class TableStringValuePopup : public ValuePopup
    {
    private:
      Lum::Table                    *table;
      Model::TableRef               tableModel;
      Model::HeaderRef              headerModel;
      Model::SingleLineSelectionRef selectionModel;

    private:
      void PositionSelection();
      void CopySelection();

    public:
      TableStringValuePopup(Model::Table* table,
                            Model::Header* header=NULL);
      void PreInit();

      void Resync(Base::Model* model, const Base::ResyncMsg& msg);
    };

    class TableIndexValuePopup : public ValuePopup
    {
    private:
      Lum::Table                    *table;
      Model::TableRef               tableModel;
      TableCellPainterRef           painter;
      Model::HeaderRef              headerModel;
      Model::SingleLineSelectionRef selectionModel;

    private:
      void PositionSelection();
      void CopySelection();

    public:
      TableIndexValuePopup(Model::Table* table,
                           TableCellPainter* painter,
                           Model::Header* header=NULL);
      void PreInit();

      void Resync(Base::Model* model, const Base::ResyncMsg& msg);
    };
  }
}

#endif
