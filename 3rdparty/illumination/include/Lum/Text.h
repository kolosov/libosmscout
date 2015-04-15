#ifndef LUM_TEXT_H
#define LUM_TEXT_H

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

#include <list>

#include <Lum/OS/Display.h>

#include <Lum/Object.h>

namespace Lum {

  class LUMAPI Text : public Object
  {
  public:
    static const unsigned long smart       = 1 << (OS::Font::maxStyle+1); /** That nice 3-D effect with bright color on dark color */
    static const unsigned long superscript = 1 << (OS::Font::maxStyle+2); /** Write in text raised in relation to baseline */
    static const unsigned long subscript   = 1 << (OS::Font::maxStyle+3); /** Write text lowered in relation to baseline */

    enum Alignment {
      left,    /** Text will be displayed left aligned. */
      right,   /** Text will displayed right aligned. */
      centered /** Text will be displayed centered. */
    };

  private:
    class Part
    {
    public:
      size_t        width;
      size_t        ascent;
      size_t        descent;
      unsigned long style;
      std::wstring  text;
      OS::FontRef   font;
      int           hOffset;
      int           vOffset;

    public:
      Part(const std::wstring& text, unsigned long style, OS::Font *font);

      void CalcSize(bool isFirst, bool isLast);
    };

    class Line
    {
    public:
      Alignment        alignment;
      std::list<Part*> parts;
      size_t           width;
      size_t           height;
      size_t           ascent;
      size_t           descent;

    public:
      Line(Alignment justification);
      virtual ~Line();

      void CalcSize(const OS::FontRef& defaultFont);

      void AddText(const std::wstring& text, unsigned long style, OS::Font *font);
    };

  private:
    std::list<Line*> text;
    Line             *current;
    size_t           textWidth,textHeight;
    size_t           lineSpace;
    //
    bool             calced;

    Alignment        currentJust;
    unsigned long    defaultStyle;
    OS::FontRef      defaultFont;

  private:
    void CalcTextSize();
    void Clear();
    void AddTextToPart(const std::wstring& text, unsigned long style, OS::Font *font);
    void CloseLine();

  public:
    Text(const std::wstring& text=L"",
         unsigned long style=OS::Font::normal,
         Alignment alignment=left,
         OS::Font *font=Lum::OS::display->GetFont());
    virtual ~Text();

    bool HasBaseline() const;
    size_t GetBaseline() const;

    void SetAlignment(Alignment alignment);
    void SetFont(OS::Font *font);
    void SetStyle(unsigned long style);

    void SetText(const std::wstring& text);
    void SetText(const std::wstring& text,OS::Font *font);
    void SetText(const std::wstring& text,unsigned long style);
    void SetText(const std::wstring& text,unsigned long style,OS::Font *font);
    void SetText(const std::wstring& text,
                 unsigned long style,
                 Alignment alignment);
    void SetText(const std::wstring& text,
                 unsigned long style,
                 Alignment alignment,
                 OS::Font *font);
    void AddText(const std::wstring& text);
    void AddText(const std::wstring& text,OS::Font *font);
    void AddText(const std::wstring& text,unsigned long style);
    void AddText(const std::wstring& text,unsigned long style,OS::Font *font);
    void AddLF();

    void CalcSize();

    void Draw(OS::DrawInfo* draw,
              int x, int y, size_t w, size_t h);

    static Text* Create(const std::wstring& text,
                        bool horizontalFlex=false, bool verticalFlex=false);
    static Text* Create(const std::wstring& text,
                        Alignment alignment,
                        bool horizontalFlex=false, bool verticalFlex=false);
  };
}

#endif
