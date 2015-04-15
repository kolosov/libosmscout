#ifndef LUM_MANAGER_NETWORK_H
#define LUM_MANAGER_NETWORK_H

/*
  This source is part of the Illumination library
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

#include <Lum/Private/ImportExport.h>

#include <Lum/Base/Model.h>

#include <Lum/Model/Action.h>

namespace Lum {
  namespace Manager {
    /**
      Manager for handling of advanced display aspects.
    */
    class LUMAPI Network : public Lum::Base::MsgObject
    {
    public:
      enum NetworkState
      {
        NetworkStateUnknown,
        NetworkStateConnecting,
        NetworkStateConnected,
        NetworkStateDisconnecting,
        NetworkStateDisconnected
      };

    protected:
      Network();

    public:
      virtual ~Network();

      /**
        Return the global NetworkManager instance.

        An instance is optional.
        */
      static Network* Instance();

      /**
        Return the current state of network connection.
        */
      virtual NetworkState GetNetworkState() const = 0;

      /**
        Register an action that gets triggered if the state of the network
        changes.
        */
      virtual Model::Action* GetNetworkStateChangeAction() const = 0;

      /**
        Request connection to the network.

        Connecting to the network is asynchronous, may take a while and may fail.
        */
      virtual bool Connect() = 0;

      /**
        Disconnect from network.
        */
      virtual bool Disconnect() = 0;
    };
  }
}

#endif
