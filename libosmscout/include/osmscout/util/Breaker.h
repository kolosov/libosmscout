#ifndef OSMSCOUT_UTIL_BREAKER_H
#define OSMSCOUT_UTIL_BREAKER_H

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

#include <osmscout/CoreFeatures.h>

#if defined(OSMSCOUT_HAVE_THREAD) && defined(OSMSCOUT_HAVE_ATOMIC)
#include <atomic>
#include <thread>
#endif

#include <memory>

#include <osmscout/private/CoreImportExport.h>

namespace osmscout {

  class OSMSCOUT_API Breaker
  {
  public:
    Breaker();
    virtual ~Breaker();

    virtual bool Break() = 0;
    virtual bool IsAborted() const = 0;
  };

  typedef std::shared_ptr<Breaker> BreakerRef;

  class OSMSCOUT_API DummyBreaker : public Breaker
  {
  public:
    DummyBreaker();

    virtual bool Break();
    virtual bool IsAborted() const;
  };

#if defined(OSMSCOUT_HAVE_THREAD) && defined(OSMSCOUT_HAVE_ATOMIC)
  class OSMSCOUT_API ThreadedBreaker : public Breaker
  {
  private:
    std::atomic<bool> aborted;
  public:
    ThreadedBreaker();

    virtual bool Break();
    virtual bool IsAborted() const;
  };
#endif
}

#endif
