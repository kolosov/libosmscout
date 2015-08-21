#ifndef OWNPOI_H
#define OWNPOI_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSettings>
#include <QStringList>
#include <QMutex>
#include <QMap>

#include "RoutingModel.h"
#include "SearchLocationModel.h"

class OwnPOI : public QObject
{
    Q_OBJECT
	Q_PROPERTY(QStringList categories READ getCatNames)
public:
    OwnPOI();

    //QStringList getCategories();
    QStringList getCatNames();
    void clearPOI();

signals:

private:

public slots:
	void loadAllPOIs();
	void loadPOICatFromDB();
	//void loadPOIPlacesFromDB(int catID, int subwayID);
	void loadPOIPlacesFromDBbyName(QString name);

};

#endif // OWNPOI_H
