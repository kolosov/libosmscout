#ifndef OSMSCOUT_TURNRESTRICTION_H
#define OSMSCOUT_TURNRESTRICTION_H

/*
  This source is part of the libosmscout library
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

#include <memory>

#include <osmscout/Types.h>

#include <osmscout/util/FileScanner.h>
#include <osmscout/util/FileWriter.h>

namespace osmscout {

  class OSMSCOUT_API TurnRestriction
  {
  public:
    enum Type
    {
      Allow  = 0,
      Forbit = 1
    };

  private:
    Type  type;
    OSMId from; // Way id
    OSMId via;  // Node id
    OSMId to;   // Way id

  public:
    inline TurnRestriction()
    {
      // no code
    }

    inline TurnRestriction(Type type,
                           OSMId from,
                           OSMId via,
                           OSMId to)
    : type(type),
      from(from),
      via(via),
      to(to)
    {
      // no code
    }

    inline Type GetType() const
    {
      return type;
    }

    void inline SetFrom(Id from)
    {
      this->from=from;
    }

    inline OSMId GetFrom() const
    {
      return from;
    }

    inline OSMId GetVia() const
    {
      return via;
    }

    inline OSMId GetTo() const
    {
      return to;
    }

    void inline SetTo(OSMId to)
    {
      this->to=to;
    }

    bool Read(FileScanner& scanner);
    bool Write(FileWriter& writer) const;
  };

  typedef std::shared_ptr<TurnRestriction> TurnRestrictionRef;
}

#endif
