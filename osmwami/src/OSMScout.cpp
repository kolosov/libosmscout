/*
  OSMScout - a Qt backend for libosmscout and libosmscout-map
  Copyright (C) 2010  Tim Teulings

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

// Qt includes
#include <QGuiApplication>
#include <QQuickView>

// Custom QML objects
#include "MapWidget.h"
#include "SearchLocationModel.h"
#include "RoutingModel.h"

#include "ownpoi.h"

// Application settings
#include "Settings.h"

// Application theming
#include "Theme.h"

// Main Window
#include "MainWindow.h"

#include <osmscout/util/Logger.h>

static QObject *ThemeProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    Theme *theme = new Theme();

    return theme;
}

int main(int argc, char* argv[])
{
#ifdef Q_WS_X11
  QCoreApplication::setAttribute(Qt::AA_X11InitThreads);
#endif

  QGuiApplication app(argc,argv);
  SettingsRef     settings;
  MainWindow      *window;
  int             result;

  app.setOrganizationName("libosmscout");
  app.setOrganizationDomain("libosmscout.sf.net");
  app.setApplicationName("WhereAmI");

  settings=std::make_shared<Settings>();

  settings->SetRoutingVehicle(osmscout::vehicleFoot);

  //qRegisterMetaType<RenderMapRequest>();
  qRegisterMetaType<DatabaseLoadedResponse>();

  qmlRegisterType<MapWidget>("net.sf.libosmscout.map", 1, 0, "Map");
  qmlRegisterType<Location>("net.sf.libosmscout.map", 1, 0, "Location");
  qmlRegisterType<LocationListModel>("net.sf.libosmscout.map", 1, 0, "LocationListModel");
  qmlRegisterType<RouteStep>("net.sf.libosmscout.map", 1, 0, "RouteStep");
  qmlRegisterType<RoutingListModel>("net.sf.libosmscout.map", 1, 0, "RoutingListModel");

  qmlRegisterSingletonType<Theme>("net.sf.libosmscout.map", 1, 0, "Theme", ThemeProvider);

  qmlRegisterType<OwnPOI>("net.sf.libosmscout.map", 1, 0, "POI");

  osmscout::log.Debug(true);

  QThread thread;

  if (!DBThread::InitializeInstance()) {
    std::cerr << "Cannot initialize DBThread" << std::endl;
  }

  DBThread* dbThread=DBThread::GetInstance();

  window=new MainWindow(settings,
                        dbThread);
  dbThread->connect(&thread, SIGNAL(started()), SLOT(Initialize()));
  dbThread->connect(&thread, SIGNAL(finished()), SLOT(Finalize()));

  dbThread->moveToThread(&thread);
  thread.start();

  result=app.exec();

  delete window;

  thread.quit();
  thread.wait();

  DBThread::FreeInstance();

  return result;
}

#if defined(__WIN32__) || defined(WIN32)
int CALLBACK WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/){
	main(0, NULL);
}
#endif
