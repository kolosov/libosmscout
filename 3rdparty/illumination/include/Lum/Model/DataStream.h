#ifndef LUM_MODEL_DATASTREAM_H
#define LUM_MODEL_DATASTREAM_H

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

#include <cassert>

#include <string>
#include <vector>

#include <Lum/Base/Model.h>

namespace Lum {
  namespace Model {

    /**
      A model for stream-like nummerical data input.
    */
    template <class V>
    class LUMAPI DataStream : public Base::Model
    {
    public:
      enum NotificationMode
      {
        notifyExplicit, //< Do not notify on data change
        notifyImplicit  //< On every change to the data, implictely notify
      };

    private:
      std::vector<V>            values;
      std::vector<std::wstring> names;
      bool                      isNull;
      NotificationMode          notificationMode;

    public:
      DataStream();

      // Setter
      void SetChannels(size_t channels);
      void SetNotificationMode(NotificationMode mode);
      void SetNull();
      void Set(size_t channel, const V& value);
      void SetName(size_t channel, const std::wstring& name);

      // Getter
      size_t GetChannels() const;
      bool IsNull() const;
      const V& Get(size_t channel) const;
      const std::wstring GetName(size_t channel) const;
    };

    template <class V>
    DataStream<V>::DataStream()
    : isNull(true),
      notificationMode(notifyImplicit)
    {
      values.resize(1);
      names.resize(1);
    }

    /**
      Sets the number of data stream channels. You assign a channel
      count to the model before you can assign any values to it.

      You can resize the model but currently all
      previous stored data will be lost.
    */
    template <class V>
    void DataStream<V>::SetChannels(size_t channels)
    {
      values.resize(channels);
      names.resize(channels);
      isNull=true;
    }

    /**
      Sets the notifycation mode of the model.
    */
    template <class V>
    void DataStream<V>::SetNotificationMode(NotificationMode mode)
    {
      notificationMode=mode;
    }

    /**
      Set the state of the data stream to inactive. A viewer object may use special
      visualisation of data in this case.
    */
    template <class V>
    void DataStream<V>::SetNull()
    {
      if (!isNull) {
        isNull=true;
        if (notificationMode==notifyImplicit) {
          Notify();
        }
      }
    }

    /**
      Assigns a value to a given channel of the model. Note that the point of
      notification of the assignment will be determinented by the notification
      mode of the model.
    */
    template <class V>
    void DataStream<V>::Set(size_t channel, const V& value)
    {
      assert(channel<values.size());

      isNull=false;
      values[channel]=value;
      if (notificationMode==notifyImplicit) {
        Notify();
      }
    }

    /**
      Assigns a name to a given channel of the model. There is no notification send
      when a new name is assigned.
    */
    template <class V>
    void DataStream<V>::SetName(size_t channel, const std::wstring& name)
    {
      assert(channel<values.size());

      names[channel]=name;
    }


    template <class V>
    size_t DataStream<V>::GetChannels() const
    {
      return values.size();
    }

    template <class V>
    bool DataStream<V>::IsNull() const
    {
      return isNull;
    }

    /**
      Returns the current value of the specified channel.
    */
    template <class V>
    const V& DataStream<V>::Get(size_t channel) const
    {
      assert(!isNull);
      assert(channel<values.size());

      return values[channel];
    }

    /**
      Returns the name of the specified channel.
    */
    template <class V>
    const std::wstring DataStream<V>::GetName(size_t channel) const
    {
      assert(channel<values.size());

      return names[channel];
    }

    class LUMAPI IntDataStream : public DataStream<int>
    {
    };

#if defined(LUM_INSTANTIATE_TEMPLATES)
    LUM_EXPTEMPL template class LUMAPI Base::Reference<IntDataStream>;
#endif

    typedef Base::Reference<IntDataStream> IntDataStreamRef;

    class LUMAPI DoubleDataStream : public DataStream<double>
    {
    };

#if defined(LUM_INSTANTIATE_TEMPLATES)
    LUM_EXPTEMPL template class LUMAPI Base::Reference<DoubleDataStream>;
#endif

    typedef Base::Reference<DoubleDataStream> DoubleDataStreamRef;
  }
}

#endif
