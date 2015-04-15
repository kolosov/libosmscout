#ifndef LUM_BASE_SINGLETON_H
#define LUM_BASE_SINGLETON_H

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

#include <stdio.h> // for NULL

#include <Lum/Private/ImportExport.h>

namespace Lum {
  namespace Base {

    class Cleanup;

    /**
      CleanupManager is a central manager assure freeing of object on
      program shutdown. In diffeence to auto_ptr classes CleanupManager
      assures that all registered object are freed in reversed order
      on shutdown. Registered object must implement the Cleanup interface.
    */
    class CleanupManager
    {
    public:
      CleanupManager();
      ~CleanupManager();

    public:
      static void Register(Cleanup* cleanup);

      void DeleteAll();
    };

    /**
      Simple interface class. All class that want to register to the
      CleanupManager must inherit from class Cleanup and implement the
      Free method.
    */
    class LUMAPI Cleanup
    {
      friend class CleanupManager;

    private:
      Cleanup* next;

    protected:
      Cleanup();
      virtual ~Cleanup();

      virtual void Free() = 0;
    };

    /**
      Singleton template to guarantee a single instance of a certain class T.
      The singleton registers to the cleanup manager and thus assures not only
      instance of the class but also that this class will be destroyed on program
      shutdown.
    */

    template <class T>
    class LUMAPI Singleton
    {
    private:
      class SingletonCleanup : public Cleanup
      {
        T *data;

      public:
        SingletonCleanup(T* data)
        : data(data)
        {
          // no code
        }

        void Free()
        {
          delete data;
          data=NULL;
        }
      };

    private:
      T** externalInstanceStore;

      static T*& GetInstance()
      {
        static T* instance=NULL;

        return instance;
      }

    public:
      /**
        Constructor of the singleton class.

        Normally the Singleton class uses a static member
        for holding a reference to the single instance. However
        there seem to be problems with shared libraries, templates
        and static members that result in multiple instance allocated
        (but only the last one freed) because of compiler/loader
        allocating storage for the static member variable twice.

        For this pupose you can assign a pointer to a external pointer of
        class T which is assured to be allocated statically in the
        shared library context.
      */
      Singleton(T** externalInstanceStore=NULL)
      : externalInstanceStore(externalInstanceStore)
      {
        // no code
      }

      virtual ~Singleton()
      {
        // no code
      }

    public:
      virtual T* AllocateInstance() const
      {
        return new T;
      }

      T* Instance() const
      {
        if (externalInstanceStore!=NULL) {
          if (*externalInstanceStore==NULL) {
            *externalInstanceStore=AllocateInstance();
            new SingletonCleanup(*externalInstanceStore);
          }

          return *externalInstanceStore;
        }
        else {
          T* &tmp=GetInstance();

          if (tmp==NULL) {
            tmp=AllocateInstance();
            new SingletonCleanup(tmp);
          }

          return tmp;
        }
      }

      T* operator->() const
      {
        return Instance();
      }
    };
  }
}

#endif
