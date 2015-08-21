#ifndef POICATEGORYMODEL_H
#define POICATEGORYMODEL_H

/*
 OSMScout - a Qt backend for libosmscout and libosmscout-map
 Copyright (C) 2014  Tim Teulings

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

//#include <map>

#include <QObject>
#include <QAbstractListModel>
#include <QVariant>

//#include <osmscout/Location.h>
//#include <osmscout/Route.h>

//#include "SearchLocationModel.h"
//#include "DBThread.h"

class Category
{
public:
	Category(const QString &name, const QString &iconName, int id);
//![0]

    QString name() const;
    QString iconName() const;
    int id() const;

private:
    int m_id;
    QString m_name;
    QString m_iconName;
};

class POICategoryListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount)

public slots:


public:
    enum Roles {
        CatNameRole = Qt::UserRole +1,
		CatIconRole,
		CatIDRole
    };

public:
    POICategoryListModel(QObject* parent = 0);
    ~POICategoryListModel();

    void addCategory(const Category &category);

    QVariant data(const QModelIndex &index, int role) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    //Qt::ItemFlags flags(const QModelIndex &index) const;

    QHash<int, QByteArray> roleNames() const;

    //Q_INVOKABLE RouteStep* get(int row) const;

private:
    QList<Category> m_categories;
};



/*
class RouteStep : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString distance READ getDistance)
    Q_PROPERTY(QString distanceDelta READ getDistanceDelta)
    Q_PROPERTY(QString time READ getTime)
    Q_PROPERTY(QString timeDelta READ getTimeDelta)
    Q_PROPERTY(QString description READ getDescription)

public:
  QString distance;
  QString distanceDelta;
  QString time;
  QString timeDelta;
  QString description;

public:
  RouteStep();
  RouteStep(const RouteStep& other);

  RouteStep& operator=(const RouteStep& other);

  QString getDistance() const
  {
      return distance;
  }

  QString getDistanceDelta() const
  {
      return distanceDelta;
  }

  QString getTime() const
  {
      return time;
  }

  QString getTimeDelta() const
  {
      return timeDelta;
  }

  QString getDescription() const
  {
      return description;
  }
};

class RoutingListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount)

public slots:
	void showMe(double myPlaceLat, double myPlaceLon);
	void loadPOI(double myPlaceLat, double myPlaceLon);
	void setStartAndTargetByCoord(double startLat, double startLon, double targetLat, double targetLon);
	void setStartAndTarget(osmscout::GeoCoord start, osmscout::GeoCoord target);
    void setStartAndTarget(Location* start,
                           Location* target);
    void clear();


private:
    struct RouteSelection
    {
      osmscout::RouteData        routeData;
      osmscout::RouteDescription routeDescription;
      QList<RouteStep>           routeSteps;
    };

    RouteSelection route;

public:
    enum Roles {
        LabelRole = Qt::UserRole
    };

private:
    void GetCarSpeedTable(std::map<std::string,double>& map);

    void DumpStartDescription(const osmscout::RouteDescription::StartDescriptionRef& startDescription,
                              const osmscout::RouteDescription::NameDescriptionRef& nameDescription);
    void DumpTargetDescription(const osmscout::RouteDescription::TargetDescriptionRef& targetDescription);
    void DumpTurnDescription(const osmscout::RouteDescription::TurnDescriptionRef& turnDescription,
                             const osmscout::RouteDescription::CrossingWaysDescriptionRef& crossingWaysDescription,
                             const osmscout::RouteDescription::DirectionDescriptionRef& directionDescription,
                             const osmscout::RouteDescription::NameDescriptionRef& nameDescription);
    void DumpRoundaboutEnterDescription(const osmscout::RouteDescription::RoundaboutEnterDescriptionRef& roundaboutEnterDescription,
                                        const osmscout::RouteDescription::CrossingWaysDescriptionRef& crossingWaysDescription);
    void DumpRoundaboutLeaveDescription(const osmscout::RouteDescription::RoundaboutLeaveDescriptionRef& roundaboutLeaveDescription,
                                        const osmscout::RouteDescription::NameDescriptionRef& nameDescription);
    void DumpMotorwayEnterDescription(const osmscout::RouteDescription::MotorwayEnterDescriptionRef& motorwayEnterDescription,
                                      const osmscout::RouteDescription::CrossingWaysDescriptionRef& crossingWaysDescription);
    void DumpMotorwayChangeDescription(const osmscout::RouteDescription::MotorwayChangeDescriptionRef& motorwayChangeDescription);
    void DumpMotorwayLeaveDescription(const osmscout::RouteDescription::MotorwayLeaveDescriptionRef& motorwayLeaveDescription,
                                      const osmscout::RouteDescription::DirectionDescriptionRef& directionDescription,
                                      const osmscout::RouteDescription::NameDescriptionRef& nameDescription);
    void DumpNameChangedDescription(const osmscout::RouteDescription::NameChangedDescriptionRef& nameChangedDescription);

public:
    RoutingListModel(QObject* parent = 0);
    ~RoutingListModel();

    QVariant data(const QModelIndex &index, int role) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;

    QHash<int, QByteArray> roleNames() const;

    Q_INVOKABLE RouteStep* get(int row) const;
};
*/

#endif
