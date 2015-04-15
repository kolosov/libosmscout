#ifndef LUM_VALUEPAINTER_H
#define LUM_VALUEPAINTER_H

/*
  This source is part of the Illumination library
  Copyright (C) 2010  Tim Teulings

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

#include <Lum/Base/Reference.h>

#include <Lum/Model/Selection.h>
#include <Lum/Model/Table.h>

#include <Lum/Object.h>

namespace Lum {
  /**
    A value painter paints the value of a model into the given
    area. It is helpful in cases where you have composite controls
    visualy wrapping the value of a model and where the control
    does not know any details of the model and in fact should display
    values of any models. Examples are Combobox and valueButton.
    */
  class LUMAPI ValuePainter : public Base::Referencable
  {
  private:
    Lum::Object    *parent;
    Base::ModelRef model;

  protected:
    virtual OS::Font* GetFont() const;
    virtual OS::Color GetTextColor(OS::DrawInfo* draw) const;
    virtual bool GetFontStyleSmart() const;

    virtual void DrawStringLeftAligned(OS::DrawInfo* draw,
                                       int x,
                                       int y,
                                       size_t width,
                                       size_t height,
                                       const std::wstring& string) const;
    virtual void DrawStringCentered(OS::DrawInfo* draw,
                                    int x,
                                    int y,
                                    size_t width,
                                    size_t height,
                                    const std::wstring& string) const;
    virtual void DrawStringRightAligned(OS::DrawInfo* draw,
                                        int x,
                                        int y,
                                        size_t width,
                                        size_t height,
                                        const std::wstring& string) const;

  public:
    ValuePainter();
    virtual ~ValuePainter();

    virtual void SetParent(Lum::Object* parent);
    virtual void SetModel(Base::Model* model);
    virtual Base::Model* GetModel() const;

    virtual size_t GetProposedWidth() const = 0;
    virtual size_t GetProposedHeight() const = 0;

    virtual void Draw(OS::DrawInfo* draw,
                      int x,
                      int y,
                      size_t width,
                      size_t height) const = 0;
                      
    static ValuePainter* GetDefaultValuePainter(Base::Model* model);
  };

  typedef Base::Reference<ValuePainter> ValuePainterRef;

  class LUMAPI TimePainter : public ValuePainter
  {
  private:
    Base::TimeFormat format;

  public:
    TimePainter(Base::TimeFormat format=Base::timeFormatDefault);

    size_t GetProposedWidth() const;
    size_t GetProposedHeight() const;

    void Draw(OS::DrawInfo* draw,
              int x,
              int y,
              size_t width,
              size_t height) const;
  };

  class LUMAPI DatePainter : public ValuePainter
  {
  public:
    size_t GetProposedWidth() const;
    size_t GetProposedHeight() const;

    void Draw(OS::DrawInfo* draw,
              int x,
              int y,
              size_t width,
              size_t height) const;
  };

  class LUMAPI StringPainter : public ValuePainter
  {
  public:
    size_t GetProposedWidth() const;
    size_t GetProposedHeight() const;

    void Draw(OS::DrawInfo* draw,
              int x,
              int y,
              size_t width,
              size_t height) const;
  };

  /**
    A special table cell painter that paints the value of a
    table cell. This is an abstract class where the Paint()
    method is still to be implemented.
    */
  class LUMAPI TableCellPainter : public ValuePainter
  {
  protected:
    mutable size_t column;
    mutable size_t row;

  protected:
    TableCellPainter();

  public:
    void SetCell(size_t column, size_t row);

    size_t GetRow() const;
    size_t GetColumn() const;

    virtual size_t GetProposedWidth() const;
    virtual size_t GetProposedHeight() const;

    virtual OS::Fill* GetCellBackground(OS::DrawInfo* draw) const;

    virtual void Draw(OS::DrawInfo* draw,
                      int x,
                      int y,
                      size_t width,
                      size_t height) const = 0;
                      
    static TableCellPainter* GetDefaultTableCellPainter(Base::Model* model);
  };

  typedef Base::Reference<TableCellPainter> TableCellPainterRef;

  /**
    An implementation of TableCellPainter that assumes that
    the cell value is a simple string. You have to implement
    the GetCellData() method to surply this string value from the
    model. The Paint() method is already implemented.
    */
  class LUMAPI StringCellPainter : public TableCellPainter
  {
  public:
    enum Alignment
    {
      left,
      center,
      right
    };

  private:
    std::vector<Alignment> alignments;
    mutable size_t         proposedWidth;

  public:
    StringCellPainter();

    void SetAlignment(size_t column, Alignment alignment);
    Alignment GetAlignment(size_t column) const;

    size_t GetProposedWidth() const;
    virtual std::wstring GetCellData() const = 0;

    virtual void Draw(OS::DrawInfo* draw,
                      int x,
                      int y,
                      size_t width,
                      size_t height) const;
  };

  /**
    An implementation of StringCellPainter for the StringTable model.
    */
  class LUMAPI StringTablePainter : public StringCellPainter
  {
  public:
    std::wstring GetCellData() const;
  };

  /**
    An implementation of StringCellPainter for the StringRefTable model.
    */
  class LUMAPI StringRefTablePainter : public StringCellPainter
  {
  public:
    std::wstring GetCellData() const;
  };

  /**
    An implementation of StringCellPainter for the StringMatrix model.
    */
  class LUMAPI StringMatrixPainter : public StringCellPainter
  {
  public:
    std::wstring GetCellData() const;
  };
}

#endif
