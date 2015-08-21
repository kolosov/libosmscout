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

#include <qqml.h>
#include <qqmlengine.h>
#include <qqmlcontext.h>


// Custom QML objects
#include "MapWidget.h"
#include "SearchLocationModel.h"
#include "RoutingModel.h"

#include "POILoader.h"
#include "OwnPOI.h"

// Application settings
#include "Settings.h"

// Application theming
#include "Theme.h"

// Main Window
#include "MainWindow.h"
#include "POICategoryModel.h"

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

  if (!POILoader::InitializeInstance()) {
      std::cerr << "Cannot initialize sql database" << std::endl;
  }
  POILoader* sqlThread=POILoader::GetInstance();

  //load places

  QStringList dataList;
  dataList.append("Apteka 1");
  dataList.append("Apteka 2");
  dataList.append("Apteka 3");

  //dataList = sqlThread->loadPOIPlacesFromDBbyNameToStringList("pharmacy");


  window=new MainWindow(settings,
                        dbThread);
  dbThread->connect(&thread, SIGNAL(started()), SLOT(Initialize()));
  dbThread->connect(&thread, SIGNAL(finished()), SLOT(Finalize()));

  dbThread->moveToThread(&thread);
  thread.start();

  //load models
  QQuickView view;
  QQmlContext *ctxt = window->rootContext();

  POICategoryListModel *categoriesModel = new POICategoryListModel();

  sqlThread->loadPOICat(categoriesModel);

  ctxt->setContextProperty("pharmacyModel", QVariant::fromValue(dataList));
  ctxt->setContextProperty("categoriesModel",categoriesModel);


  result=app.exec();

  delete window;

  thread.quit();
  thread.wait();

  DBThread::FreeInstance();
  POILoader::FreeInstance();

  return result;
}

#if defined(__WIN32__) || defined(WIN32)
int CALLBACK WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/){
	main(0, NULL);
}
#endif
