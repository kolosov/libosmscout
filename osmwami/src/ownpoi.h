#ifndef OWNPOI_H
#define OWNPOI_H

#include <QObject>

#include "RoutingModel.h"
#include "SearchLocationModel.h"

class OwnPOI : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Location* startLoc READ getStartLoc)
    Q_PROPERTY(Location* endLoc READ getEndLoc)
public:
    explicit OwnPOI(QObject *parent = 0);
    Q_INVOKABLE void LoadPOI();
    Location *startLoc;
    Location *endLoc;

    Location *getStartLoc() {
        return startLoc;
    }
    Location *getEndLoc() {
        return endLoc;
    }

signals:

public slots:

};

#endif // OWNPOI_H
