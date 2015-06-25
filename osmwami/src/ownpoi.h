#ifndef OWNPOI_H
#define OWNPOI_H

#include <QObject>

#include "RoutingModel.h"
#include "SearchLocationModel.h"

#include <osmscout/Location.h>
#include <osmscout/Route.h>
#include <osmscout/Database.h>
#include <osmscout/RoutingProfile.h>
#include <osmscout/RoutingService.h>
#include <osmscout/RoutePostprocessor.h>


class OwnPOI : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Location* startLoc READ getStartLoc)
    Q_PROPERTY(Location* endLoc READ getEndLoc)
public:
    OwnPOI();
    //osmscout::TypeConfigRef GetTypeConfig() const;

    Q_INVOKABLE void LoadPOI();
    Location *startLoc;
    Location *endLoc;

    Location *getStartLoc() {
        return startLoc;
    }
    Location *getEndLoc() {
        return endLoc;
    }

    osmscout::DatabaseParameter   databaseParameter;
    osmscout::DatabaseRef         database;
    osmscout::StyleConfigRef      styleConfig;

    osmscout::RouterParameter           routerParameter;
    osmscout::RoutingServiceRef router;
    osmscout::ObjectFileRef                   startObject;
    size_t                                    startNodeIndex;

    osmscout::ObjectFileRef                   targetObject;
    size_t                                    targetNodeIndex;

    osmscout::RouteData                 data;

signals:

public slots:

};

#endif // OWNPOI_H
