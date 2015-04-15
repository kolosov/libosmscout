#ifndef LUM_LABEL_H
#define LUM_LABEL_H

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

#include <Lum/Base/Size.h>

#include <Lum/Object.h>

namespace Lum {

  /**
    A layout object that generates a vertical list of aligned
    textlabel and object pairs.

    It automatically adds spaces and corretcly aligns the
    labels and controls for optimal look.
  */
  class LUMAPI Label : public Group
  {
  private:
    class Entry
    {
    public:
      size_t indent;
      Object *label;
      Object *object;

    public:
      void CalcSize();

      size_t GetMinHeight() const;
      size_t GetHeight() const;
      size_t CanResize(bool grow) const;

      void   Resize(size_t height);

      bool   HasOffset() const;
      size_t GetLabelOffset() const;
      size_t GetObjectOffset() const;
    };

  private:
    std::list<Entry> list;          //! list of rows
    size_t           labelWidth;    //! Maximum width of labels
    size_t           objectWidth;   //! Maximum width of objectss
    bool             landscapeMode; //! If true, arrange objects for landscape, else portrait

  public:
    Label();
    ~Label();

    bool VisitChildren(Visitor &visitor, bool onlyVisible);

    Label* AddLabel(Object* label, Object* object);
    Label* AddLabel(const std::wstring& label, Object* object);
    Label* AddLabel(const std::wstring& label, const std::wstring& object);

    void CalcSize();
    void Layout();

    static Label* Create(bool horizontalFlex=false, bool verticalFlex=false);
  };
}

#endif
