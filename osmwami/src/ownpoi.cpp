#include "ownpoi.h"

OwnPOI::OwnPOI(QObject *parent) :
    QObject(parent)
{
    //test poi
    double myPlaceX = 55.74352, myPlaceY = 37.60600;
    double myDestX = 55.71046, myDestY = 37.47212;

    osmscout::GeoCoord startCoord(myPlaceX, myPlaceY);
    osmscout::GeoCoord destCoord(myDestX, myDestY);

    startLoc = new Location(QString("StartName"), QString("StartLabel"),startCoord);
    endLoc = new Location(QString("EndName"), QString("EndLabel"),destCoord);

    //Location startLocation(QString("StartName"), QString("StartLabel"),startCoord);
    //Location endLocation(QString("EndName"), QString("EndLabel"),destCoord);

    std::cout << "OwnPOI initialized" << std::endl;
}

void OwnPOI::LoadPOI()
{
    //routingModel.setStartAndTarget(startLocation,
    //                                   endLocation);

    //std::cout << "LoadPOI, start routing" << std::endl;

}
