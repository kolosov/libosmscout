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

#include <Lum/Object.h>

namespace Lum {
  /**
    A Float object is an scrollable object that shows it children in a
    matrix like arrangement where the number of coluns and rows is depending
    on the actual width and height of the Float object. If not all
    objects can be shown scorllbars would be shown.

    A Float object is designe dto be placed into a scrollable control
    like View.
    */
  class LUMAPI Float : public Lum::Scrollable
  {
  private:
    std::list<Lum::Object*> objects;          //! List of object to show
    size_t                  objectMinWidth;   //! Biggest minimum width of all objects
    size_t                  objectMinHeight;  //! Biggest minimum height of all objects
    size_t                  objectWidth;      //! Biggest width of all objects
    size_t                  objectHeight;     //! Biggest height of all objects

    size_t                  preferredColumns; //! Numbers of columns we prefer
    size_t                  columns;          //! Last shown number of columns
    size_t                  rows;             //! Last shown number of rows

  public:
    Float();
    ~Float();

    bool RequiresKnobs() const;

    Float* SetPreferredColumns(size_t columns);
    Float* Add(Lum::Object* object);

    bool VisitChildren(Visitor &visitor, bool onlyVisible);

    void GetDimension(size_t& width, size_t& height);

    void CalcSize();
    void Layout();

    void Resync(Lum::Base::Model* model, const Lum::Base::ResyncMsg& msg);
  };
}

