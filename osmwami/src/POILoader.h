#ifndef POILOADER_H
#define POILOADER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSettings>
#include <QStringList>
#include <QMutex>
#include <QMap>

#include "RoutingModel.h"
#include "SearchLocationModel.h"
#include "POICategoryModel.h"

class POILoader : public QObject
{
    Q_OBJECT
	Q_PROPERTY(QStringList categories READ getCatNames)
public:
    static bool InitializeInstance();
    static POILoader* GetInstance();
    static void FreeInstance();

    bool m_Ok;
    //QStringList getCategories();
    QStringList getCatNames();

    void loadPOICatFromDB();
    void loadPOICat(POICategoryListModel *catModel);
    void loadPOISubCat(int catId, POICategoryListModel *catModel);
    void loadPOIPlacesFromDB(int catID, QString catName, int subwayID);
    QStringList loadPOIPlacesFromDBToStringList(int catID, QString catName, int subwayID);
    void loadPOIPlacesFromDBbyName(QString name);
    QStringList loadPOIPlacesFromDBbyNameToStringList(QString name);
    void loadAllPOIs();

signals:

private:
	POILoader();

	mutable QMutex mutex;
    QSqlDatabase m_sqldb;
    //QString m_sSettingsFile;
    QSettings *m_Settings;
    QString m_getCatReq;
    QMap<QString, int> m_CatList;
    QStringList m_CatNames;


public slots:

};

#endif // POILOADER_H
