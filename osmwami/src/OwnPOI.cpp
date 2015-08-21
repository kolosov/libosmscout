#include "OwnPOI.h"
#include "POILoader.h"

#include <QApplication>
#include <QDebug>
#include <QDir>

OwnPOI::OwnPOI()
{

}

void OwnPOI::loadPOICatFromDB()
{
	POILoader *poiLoader = POILoader::GetInstance();

	poiLoader->loadPOICatFromDB();
}

/*
void OwnPOI::loadPOIPlacesFromDB(int catID, int subwayID)
{
	POILoader *poiLoader = POILoader::GetInstance();

	poiLoader->loadPOIPlacesFromDB(catID, subwayID);
}*/

void OwnPOI::loadPOIPlacesFromDBbyName(QString name)
{
	POILoader *poiLoader = POILoader::GetInstance();

	poiLoader->loadPOIPlacesFromDBbyName(name);
}

void OwnPOI::loadAllPOIs()
{
	POILoader *poiLoader = POILoader::GetInstance();

	poiLoader->loadAllPOIs();
}

QStringList OwnPOI::getCatNames()
{
	POILoader *poiLoader = POILoader::GetInstance();

	return poiLoader->getCatNames();
}

void OwnPOI::clearPOI()
{
	DBThread *db = DBThread::GetInstance();

	db->ClearPOI();
}

