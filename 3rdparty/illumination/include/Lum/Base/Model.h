#ifndef LUM_BASE_MODEL_H
#define LUM_BASE_MODEL_H

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

#include <list>
#include <string>

#include <assert.h>

#include <Lum/Private/ImportExport.h>

#include <Lum/Base/Object.h>
#include <Lum/Base/Reference.h>

namespace Lum {
  namespace Base {

    /**
      This message is the abstract baseclass for all
      resync-messages. A model can send to its viewers
      when the contents of the model have changed.
    */
    class LUMAPI ResyncMsg
    {
    public:
      virtual ~ResyncMsg();
    };

    class MsgObject;

    /**
      Abstract base class for all models.

      Models hold data that is modified by viewers/controls as
      defined by the model viewer controller concept (MVC).
     */

    class LUMAPI Model : public Referencable
    {
    public:
      friend class MsgObject;

    private:
      class WatcherEntry
      {
      public:
        MsgObject* object;
        size_t     depth;
      };

    private:
      std::list<WatcherEntry> watchers;
      size_t                  depth;
      size_t                  off;      //< If off == 0, notification is active
      bool                    enabled;

    private:
      void PrependObject(MsgObject* object);
      void AddObject(MsgObject* object);
      void RemoveObject(MsgObject* object);

    public:
      Model();
      virtual ~Model();

      virtual void Enable();
      virtual void Disable();

      virtual bool IsEnabled() const;
      virtual bool IsDisabled() const;

      virtual bool IsNull() const;

      virtual void Push();
      virtual void Undo();
      virtual void Save();
      virtual void Pop();

      virtual void On(bool notify=true);
      virtual void Off();
      virtual bool IsOn();

      virtual bool IsObserved() const;

      virtual void Notify(const ResyncMsg& msg);
      void Notify();
    };

#if defined(LUM_INSTANTIATE_TEMPLATES)
    LUM_EXPTEMPL template class LUMAPI Reference<Model>;
#endif

    typedef Reference<Model> ModelRef;

    /**
      This is a base class derived from Object that defines
      objects that are able to listen to models.
    */
    class LUMAPI MsgObject : public Object
    {
    private:
      unsigned long       id;
      std::wstring        name;
      std::list<ModelRef> models;

    public:
      MsgObject();
      ~MsgObject();

      // Setter
      void SetId(int id);
      void SetName(std::wstring name);

      // Getter
      int GetId() const;
      std::wstring GetName() const;

      void Observe(Model* model);
      void ObservePrior(Model* model);
      void Forget(Model* model);
      virtual void Resync(Model* model, const ResyncMsg& msg);
    };
  }
}

#endif
