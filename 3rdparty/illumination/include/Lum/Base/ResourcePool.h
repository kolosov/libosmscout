#ifndef LUM_BASE_RESOURCEPOOL_H
#define LUM_BASE_RESOURCEPOOL_H

/*
  This source is part of the Illumination library
  Copyright (C) 2007  Tim Teulings

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

#include <stdio.h> // for NULL

#include <Lum/Private/ImportExport.h>

namespace Lum {
  namespace Base {

    class Resource;

    /**
      ResourcePool is a central resource manager that assures that assigned
      resources will be freed on call of FreeResources or at latest during shutdown
      of the program. Resources will be free in reverse order of registration.
      Registered objects must implement the Resource interface.
    */
    class ResourcePool
    {
    private:
      Resource *first;

    public:
      ResourcePool();
      ~ResourcePool();

      void Register(Resource* resource);
      void FreeResources();
    };

    /**
      Simple interface class for resources. All class that want to register to
      a ResourcePool must inherit from class Resoure and may implement the
      Free method.
    */
    class LUMAPI Resource
    {
      friend class ResourcePool;

    private:
      Resource* next;

    protected:
      Resource(ResourcePool &resourcePool);
      virtual ~Resource();

      virtual void Free();
    };
  }
}

#endif
