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

#include <Lum/OS/Hildon/Network.h>

#include <iostream>

namespace Lum {
  namespace OS {
    namespace Hildon {

      static void ConnectionHandler(ConIcConnection *connection,
                                    ConIcConnectionEvent *event,
                                    gpointer data)
      {
        Network  *manager=static_cast<Network*>(data);

        assert(manager!=NULL);

        ConIcConnectionStatus status=con_ic_connection_event_get_status(event);
        ConIcConnectionError error=con_ic_connection_event_get_error(event);

        switch (status) {
        case CON_IC_STATUS_NETWORK_UP:
          std::cout << "Connecting..."<<  std::endl;
          manager->SetNetworkState(Network::NetworkStateConnecting);
          break;
        case CON_IC_STATUS_CONNECTED:
          std::cout << "Connected!" << std::endl;
          manager->SetNetworkState(Network::NetworkStateConnected);
          break;
        case CON_IC_STATUS_DISCONNECTING:
          std::cout << "Disconnecting..." <<std::endl;
          manager->SetNetworkState(Network::NetworkStateDisconnecting);
          break;
        case CON_IC_STATUS_DISCONNECTED:
          std::cout << "Disconnected (" << error << ")!" << std::endl;
          manager->SetNetworkState(Network::NetworkStateDisconnected);
          break;
        }
      }

      Network::Network()
       : networkState(NetworkStateUnknown),
         networkStateChangeAction(new Model::Action()),
         connection(NULL)
      {
        connection=con_ic_connection_new();

        if (connection!=NULL) {
          g_signal_connect(G_OBJECT(connection),
                           "connection-event",
                           G_CALLBACK(ConnectionHandler),
                           this);
          g_object_set(G_OBJECT(connection),
                       "automatic-connection-events",
                       true,
                       NULL);

          con_ic_connection_connect(connection,CON_IC_CONNECT_FLAG_AUTOMATICALLY_TRIGGERED);
        }
        else {
          std::cerr << "Could not create ConIcConnection instance!" << std::endl;
        }
      }

      Network::~Network()
      {

        g_object_unref(connection);
      }

      void Network::SetNetworkState(NetworkState state)
      {
        if (state!=networkState) {
          networkState=state;
          networkStateChangeAction->Trigger();
        }
      }

      Network::NetworkState Network::GetNetworkState() const
      {
        return networkState;
      }

      Model::Action* Network::GetNetworkStateChangeAction() const
      {
        return networkStateChangeAction;
      }

      bool Network::Connect()
      {
        std::cout << "Requesting connection..." << std::endl;
        if (con_ic_connection_connect(connection,CON_IC_CONNECT_FLAG_NONE)) {
          return true;
        }
        else {
          std::cerr << "Connection request failed!" << std::endl;
          return false;
        }
      }

      bool Network::Disconnect()
      {
        return con_ic_connection_disconnect(connection);
      }
    }
  }
}
