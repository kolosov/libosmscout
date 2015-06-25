#include "ownpoi.h"

#include <QApplication>
#include <QDebug>
#include <QDir>

OwnPOI::OwnPOI()
 : database(new osmscout::Database(databaseParameter))
{
    //test poi

//    startLoc = new Location(QString("StartName"), QString("StartLabel"),startCoord);
//    endLoc = new Location(QString("EndName"), QString("EndLabel"),destCoord);

    //Location startLocation(QString("StartName"), QString("StartLabel"),startCoord);
    //Location endLocation(QString("EndName"), QString("EndLabel"),destCoord);

    //all here

    QStringList cmdLineArgs = QApplication::arguments();
    QString databaseDirectory = cmdLineArgs.size() > 1 ? cmdLineArgs.at(1) : QDir::currentPath();
    QString stylesheetFilename = cmdLineArgs.size() > 2 ? cmdLineArgs.at(2) : databaseDirectory + QDir::separator() + "standard.oss";
    QString iconDirectory = cmdLineArgs.size() > 3 ? cmdLineArgs.at(3) : databaseDirectory + QDir::separator() + "icons";

    //osmscout::DatabaseRef       database(new osmscout::Database(databaseParameter));
    //osmscout::DatabaseRef         database;


    osmscout::TypeConfigRef styleConfig;
    if (database->Open(databaseDirectory.toLocal8Bit().data())) {
      osmscout::TypeConfigRef typeConfig=database->GetTypeConfig();


      /*if (typeConfig) {
        styleConfig=new osmscout::StyleConfig(typeConfig);

        delete painter;
        painter=NULL;

        if (styleConfig->Load(stylesheetFilename.toLocal8Bit().data())) {
            painter=new osmscout::MapPainterQt(styleConfig);
        }
        else {
          qDebug() << "Cannot load style sheet!";
          styleConfig=NULL;
        }
      }
      else {
        qDebug() << "TypeConfig invalid!";
        styleConfig=NULL;
      }*/
    }
    else {
      qDebug() << "Cannot open database!";
      return;
    }

    osmscout::Vehicle                   vehicle=osmscout::vehicleFoot;//settings->GetRoutingVehicle();
    //osmscout::FastestPathRoutingProfile routingProfile(database->GetTypeConfig());
    //routingProfile = osmscout::FastestPathRoutingProfile(database->GetTypeConfig());


    router = std::make_shared<osmscout::RoutingService>(database, routerParameter, vehicle);


    //router = osmscout::RoutingService(database, routerParameter, vehicle);

    //router = new osmscout::RoutingService(database, routerParameter, vehicle));

    qDebug() << "Router is initialized" ;



    std::cout << "OwnPOI is initialized" << std::endl;
}

/*
osmscout::TypeConfigRef OwnPOI::GetTypeConfig() const
{
  return database->GetTypeConfig();
}
*/
void OwnPOI::LoadPOI()
{
    double myPlaceX = 55.74352, myPlaceY = 37.60600;
    double myDestX = 55.71046, myDestY = 37.47212;

    //osmscout::GeoCoord startCoord(myPlaceX, myPlaceY);
    //osmscout::GeoCoord destCoord(myDestX, myDestY);

    osmscout::FastestPathRoutingProfile routingProfile(database->GetTypeConfig());
    osmscout::TypeConfigRef             typeConfig=database->GetTypeConfig();
    osmscout::RouteDescription          description;
    std::list<osmscout::Point> points;
    osmscout::Way                       routeWay;


    osmscout::Vehicle                   vehicle=osmscout::vehicleFoot;//settings->GetRoutingVehicle();

    routingProfile.ParametrizeForFoot(*typeConfig, 5.0);

    if (!router->Open()) {
      std::cerr << "Cannot open routing database" << std::endl;
      return;
    }
	//routingModel.setStartAndTarget(startLocation,
	//                                   endLocation);

	//std::cout << "LoadPOI, start routing" << std::endl;
	if (!router->GetClosestRoutableNode(myPlaceX,
			myPlaceY,
			vehicle,
			1000,
			startObject,
			startNodeIndex)) {
		std::cerr << "Error while searching for routing node near start location!" << std::endl;
		return;
	}

	if (!router->GetClosestRoutableNode(myDestX,
			myDestY,
			vehicle,
			1000,
			targetObject,
			targetNodeIndex)) {
		std::cerr << "Error while searching for routing node near destonation location!" << std::endl;
		return;
	}


	qDebug() << "Start routing process...";

	if (!router->CalculateRoute(routingProfile,
			startObject,
			startNodeIndex,
			targetObject,
			targetNodeIndex,
			data)) {
		std::cerr << "There was an error while calculating the route!" << std::endl;
		router->Close();
		return;
	}

	if (data.IsEmpty()) {
		std::cout << "No Route found!" << std::endl;

		router->Close();

		return;
	}

	router->TransformRouteDataToRouteDescription(data, description);

	router->TransformRouteDataToPoints(data, points);

	qDebug() << "Points len is " << points.size();

	qDebug() << "Draw route ... " << points.size();
	router->TransformRouteDataToWay(data, routeWay);


}
