#ifndef LUM_DEF_PROPERTY_H
#define LUM_DEF_PROPERTY_H

/*
  This source is part of the Illumination library
  Copyright (C) 2011  Tim Teulings

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

#include <Lum/Private/ImportExport.h>

#include <Lum/Def/Desc.h>

namespace Lum {

  namespace Def {
    /**
     * Abstract definition base class for data elements, which are part of
     * a dialog, property page or similar.
     */
    class LUMAPI Property
    {
    private:
      Desc              description;

    protected:
      Property(const Desc& description);

    public:
      virtual ~Property();

      const Desc& GetDesc() const;

      virtual Property* Copy() const = 0;
    };
  }
}

#endif

