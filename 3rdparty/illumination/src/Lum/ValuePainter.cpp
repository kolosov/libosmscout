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

#include <Lum/ValuePainter.h>

#include <cmath>
#include <cstdlib>
#include <memory>

#include <Lum/Base/Size.h>

#include <Lum/Model/Calendar.h>
#include <Lum/Model/String.h>
#include <Lum/Model/Time.h>

#include <Lum/OS/Display.h>

#include <Lum/Table.h>

namespace Lum {

  ValuePainter::ValuePainter()
  : parent(NULL)
  {
    // no code
  }

  ValuePainter::~ValuePainter()
  {
    // no code
  }

  OS::Font* ValuePainter::GetFont() const
  {
    assert(parent!=NULL);

    return parent->GetFont(this);
  }

  OS::Color ValuePainter::GetTextColor(OS::DrawInfo* draw) const
  {
    assert(parent!=NULL);

    return parent->GetTextColor(draw,this);
  }

  bool ValuePainter::GetFontStyleSmart() const
  {
    assert(parent!=NULL);

    return parent->GetFontStyleSmart(this);
  }

  void ValuePainter::SetParent(Lum::Object* parent)
  {
    this->parent=parent;
  }

  void ValuePainter::SetModel(Lum::Base::Model* model)
  {
    this->model=model;
  }

  Lum::Base::Model* ValuePainter::GetModel() const
  {
    return model;
  }

  void ValuePainter::DrawStringLeftAligned(OS::DrawInfo* draw,
                                           int x,
                                           int y,
                                           size_t width,
                                           size_t height,
                                           const std::wstring& string) const
  {
    size_t textWidth=GetFont()->StringWidth(string);
    bool   cliped=textWidth>width || GetFont()->height>height;

    if (cliped){
      draw->PushClip(x,y,width,height);
    }

    draw->PushFont(GetFont());

    if (GetFontStyleSmart()) {
      draw->PushForeground(OS::Display::textSmartAColor);
      draw->DrawString(x+1,
                       y+(height-GetFont()->height)/2+GetFont()->ascent+1,
                       string);
      draw->PopForeground();

      draw->PushForeground(OS::Display::textSmartBColor);
      draw->DrawString(x,
                       y+(height-GetFont()->height)/2+GetFont()->ascent,
                       string);
      draw->PopForeground();
    }
    else {
      draw->PushForeground(GetTextColor(draw));
      draw->DrawString(x,
                       y+(height-GetFont()->height)/2+GetFont()->ascent,
                       string);
      draw->PopForeground();
    }

    draw->PopFont();

    if (cliped){
      draw->PopClip();
    }
  }

  void ValuePainter::DrawStringCentered(OS::DrawInfo* draw,
                                        int x,
                                        int y,
                                        size_t width,
                                        size_t height,
                                        const std::wstring& string) const
  {
    size_t textWidth=GetFont()->StringWidth(string);
    bool   cliped=textWidth>width || GetFont()->height>height;

    if (cliped){
      draw->PushClip(x,y,width,height);
    }

    draw->PushFont(GetFont());

    if (GetFontStyleSmart()) {
      draw->PushForeground(OS::Display::textSmartAColor);
      draw->DrawString(x+(width-textWidth)/2+1,
                       y+(height-GetFont()->height)/2+GetFont()->ascent+1,
                       string);
      draw->PopForeground();

      draw->PushForeground(OS::Display::textSmartBColor);
      draw->DrawString(x+(width-textWidth)/2,
                       y+(height-GetFont()->height)/2+GetFont()->ascent,
                       string);
      draw->PopForeground();
    }
    else {
      draw->PushForeground(GetTextColor(draw));
      draw->DrawString(x+(width-textWidth)/2,
                       y+(height-GetFont()->height)/2+GetFont()->ascent,
                       string);
      draw->PopForeground();
    }

    draw->PopFont();

    if (cliped){
      draw->PopClip();
    }
  }

  void ValuePainter::DrawStringRightAligned(OS::DrawInfo* draw,
                                            int x,
                                            int y,
                                            size_t width,
                                            size_t height,
                                            const std::wstring& string) const
  {
    size_t textWidth=GetFont()->StringWidth(string);
    bool   cliped=textWidth>width || GetFont()->height>height;

    if (cliped){
      draw->PushClip(x,y,width,height);
    }

    draw->PushFont(GetFont());

    if (GetFontStyleSmart()) {
      draw->PushForeground(OS::Display::textSmartAColor);
      draw->DrawString(x+width-1-textWidth,
                       y+(height-GetFont()->height)/2+GetFont()->ascent+1,
                       string);
      draw->PopForeground();

      draw->PushForeground(OS::Display::textSmartBColor);
      draw->DrawString(x+width-1-textWidth-1,
                       y+(height-GetFont()->height)/2+GetFont()->ascent,
                       string);
      draw->PopForeground();
    }
    else {
      draw->PushForeground(GetTextColor(draw));
      draw->DrawString(x+width-1-textWidth,
                       y+(height-GetFont()->height)/2+GetFont()->ascent,
                       string);

      draw->PopForeground();
    }

    draw->PopFont();

    if (cliped){
      draw->PopClip();
    }
  }

  ValuePainter* ValuePainter::GetDefaultValuePainter(Base::Model* model)
  {
    if (model==NULL) {
      return NULL;
    }
    else if (dynamic_cast<Model::Time*>(model)!=NULL) {
      return new TimePainter();
    }
    else if (dynamic_cast<Model::Calendar*>(model)!=NULL) {
      return new DatePainter();
    }
    else if (dynamic_cast<Model::String*>(model)!=NULL) {
      return new StringPainter();
    }
    else if (dynamic_cast<Model::StringTable*>(model)!=NULL) {
      return new StringTablePainter();
    }
    else if (dynamic_cast<Model::StringRefTable*>(model)!=NULL) {
      return new StringRefTablePainter();
    }
    else if (dynamic_cast<Model::StringMatrix*>(model)!=NULL) {
      return new StringMatrixPainter();
    }
    else {
      return NULL;
    }
  }

  TimePainter::TimePainter(Base::TimeFormat format)
   : format(format)
  {
    // no code
  }

  size_t TimePainter::GetProposedWidth() const
  {
    return 0;
  }

  size_t TimePainter::GetProposedHeight() const
  {
    if (GetFontStyleSmart()) {
      return GetFont()->height+1;
    }
    else {
      return GetFont()->height;
    }
  }

  void TimePainter::Draw(OS::DrawInfo* draw,
                         int x,
                         int y,
                         size_t width,
                         size_t height) const
  {
    Model::Time* model=dynamic_cast<Model::Time*>(GetModel());

    if (model!=NULL) {
      DrawStringLeftAligned(draw,
                            x,y,width,height,
                            model->Get().GetLocaleTime(format));
    }
  }

  size_t DatePainter::GetProposedWidth() const
  {
    return 0;
  }

  size_t DatePainter::GetProposedHeight() const
  {
    return GetFont()->height;
  }

  void DatePainter::Draw(OS::DrawInfo* draw,
                         int x,
                         int y,
                         size_t width,
                         size_t height) const
  {
    Model::Calendar* model=dynamic_cast<Model::Calendar*>(GetModel());

    if (model!=NULL) {
      DrawStringLeftAligned(draw,
                            x,y,width,height,
                            model->Get().GetLocaleDate());
    }
  }

  TableCellPainter::TableCellPainter()
  : column(0),
    row(0)
  {
    // no code
  }

  size_t StringPainter::GetProposedWidth() const
  {
    return 0;
  }

  size_t StringPainter::GetProposedHeight() const
  {
    return GetFont()->height;
  }

  void StringPainter::Draw(OS::DrawInfo* draw,
                           int x,
                           int y,
                           size_t width,
                           size_t height) const
  {
    Model::String* model=dynamic_cast<Model::String*>(GetModel());

    if (model!=NULL) {
      DrawStringLeftAligned(draw,
                            x,y,width,height,
                            model->Get());
    }
  }

  void TableCellPainter::SetCell(size_t column, size_t row)
  {
    this->column=column;
    this->row=row;
  }

  size_t TableCellPainter::GetRow() const
  {
    return row;
  }

  size_t TableCellPainter::GetColumn() const
  {
    return column;
  }

  size_t TableCellPainter::GetProposedWidth() const
  {
    return 0;
  }

  size_t TableCellPainter::GetProposedHeight() const
  {
    return GetFont()->height;
  }

  OS::Fill* TableCellPainter::GetCellBackground(OS::DrawInfo* draw) const
  {
    return NULL;
  }

  TableCellPainter* TableCellPainter::GetDefaultTableCellPainter(Base::Model* model)
  {
    if (model==NULL) {
      return NULL;
    }
    else if (dynamic_cast<Model::StringTable*>(model)!=NULL) {
      return new StringTablePainter();
    }
    else if (dynamic_cast<Model::StringRefTable*>(model)!=NULL) {
      return new StringRefTablePainter();
    }
    else if (dynamic_cast<Model::StringMatrix*>(model)!=NULL) {
      return new StringMatrixPainter();
    }
    else {
      return NULL;
    }
  }

  StringCellPainter::StringCellPainter()
  : proposedWidth(0)
  {
    // no code
  }

  size_t StringCellPainter::GetProposedWidth() const
  {
    if (proposedWidth!=0) {
      return proposedWidth;
    }

    Model::Table *tableModel=dynamic_cast<Model::Table*>(GetModel());

    if (tableModel==NULL) {
      return 0;
    }

    size_t width=0;

    for (size_t r=1; r<=tableModel->GetRows(); r++) {
      column=1;
      row=r;

      std::wstring data=GetCellData();

      if (GetFontStyleSmart()) {
        width=std::max(width,GetFont()->StringWidth(data)+1);
      }
      else {
        width=std::max(width,GetFont()->StringWidth(data));
      }
    }

    column=0;
    row=0;

    proposedWidth=width;

    return proposedWidth;
  }

  void StringCellPainter::SetAlignment(size_t column, Alignment alignment)
  {
    assert(column>=1);

    size_t index=column-1;

   if (index>=alignments.size()) {
     alignments.resize(index+1,left);
   }

    alignments[index]=alignment;
  }

  StringCellPainter::Alignment StringCellPainter::GetAlignment(size_t column) const
  {
    assert(column>=1);

    size_t index=column-1;

    if (index>=alignments.size()) {
      return left;
    }
    else {
      return alignments[index];
    }
  }

  void StringCellPainter::Draw(OS::DrawInfo* draw,
                               int x,
                               int y,
                               size_t width,
                               size_t height) const
  {
    assert(GetColumn()>=1);

    std::wstring   string=GetCellData();
    switch (GetAlignment(GetColumn())) {
    case left:
      DrawStringLeftAligned(draw,x,y,width,height,string);
      break;
    case center:
      DrawStringCentered(draw,x,y,width,height,string);
      break;
    case right:
      DrawStringRightAligned(draw,x,y,width,height,string);
      break;
    }
  }

  std::wstring StringTablePainter::GetCellData() const
  {
    const Model::StringTable *m=dynamic_cast<const Model::StringTable*>(GetModel());

    return m->GetEntry(GetRow());
  }

  std::wstring StringRefTablePainter::GetCellData() const
  {
    const Model::StringRefTable *m=dynamic_cast<const Model::StringRefTable*>(GetModel());

    return m->GetEntry(GetRow());
  }

  std::wstring StringMatrixPainter::GetCellData() const
  {
    const Model::StringMatrix *m=dynamic_cast<const Model::StringMatrix*>(GetModel());

    return m->GetEntry(GetRow())[GetColumn()-1];
  }
}

