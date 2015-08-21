#include "POILoader.h"
#include "DBThread.h"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QMutexLocker>


POILoader::POILoader()
{
    m_sqldb = QSqlDatabase::addDatabase("QMYSQL");

    m_Settings = new QSettings("WAMISoft", "wami");
    m_Ok = false;

    //m_CategoriesList << "Музеи" << "Аптеки" << "Клубы";

    m_Settings->beginGroup("DB");

//    move(m_Settings.value("pos", QPoint(200, 200)).toPoint());

    //m_Settings.value("dbhost", "");
    //m_Settings->setValue("get_cat_req", "SELECT id,name FROM whereami.categories where id=70 or id=43 or id=59 or id=32 or id=11 order by name;");

    //qDebug() << m_Settings->value("get_cat_req","").toString();

    m_getCatReq = m_Settings->value("get_cat_req","").toString();

    m_sqldb.setHostName(m_Settings->value("dbhost","").toString());
    m_sqldb.setDatabaseName(m_Settings->value("dbname","").toString());
    m_sqldb.setUserName(m_Settings->value("username","").toString());
    m_sqldb.setPassword(m_Settings->value("userpass","").toString());
    m_Settings->endGroup();

    qDebug() << "Try open mysql database";
    m_Ok = m_sqldb.open();

    if(!m_Ok) {
    	qDebug() << "Cannot open mysql database";
    	return;
    } else {
    	loadPOICatFromDB();
    }

}

void POILoader::loadPOICatFromDB()
{
	if(!m_Ok)
		return;

	// m_CatNames << "Аптеки" << "Гостиницы" << "Музеи" << "Клубы" << "Рестораны";

	m_CatNames << "pharmacy" << "hotel" << "museum" << "nightclub" << "restaurant";

	m_CatList[m_CatNames[0]] = 70;
	m_CatList[m_CatNames[1]] = 45;
	m_CatList[m_CatNames[2]] = 0;
	m_CatList[m_CatNames[3]] = 0;
	m_CatList[m_CatNames[4]] = 0;

	/*QSqlQuery query;
	query.exec(m_getCatReq);

    while (query.next()) {
    	int id = query.value(0).toInt();
        QString name = query.value(1).toString();

        qDebug() << name << " "<< id;
    }*/

    //loadPOIPlacesFromDB(45,0);
	//m_CategoriesList
}

void POILoader::loadPOIPlacesFromDB(int catID, QString catName,int subwayID)
{
	QMutexLocker locker(&mutex);
	QSqlQuery query;
	// SELECT id, name, latitude, longitude FROM whereami.places where category_id in (SELECT id FROM whereami.categories where parent_id=43);
	//QString getPlacesReq = QString("SELECT id, name, latitude, longitude "
	//		"FROM whereami.places where category_id in "
	//		"(SELECT id FROM whereami.categories where parent_id=%1);").arg(catID);

	QString getPlacesReq = QString("SELECT id, name, latitude, longitude "
				"FROM whereami.places where category_id=%1;").arg(catID);

	qDebug() << "Running sql request: " << getPlacesReq;

	DBThread         *dbThread=DBThread::GetInstance();

	query.exec(getPlacesReq);
	while (query.next()) {
	    	int id = query.value(0).toInt();
	        QString name = query.value(1).toString();
	        double lat = query.value(2).toDouble();
	        double lon = query.value(3).toDouble();

	        qDebug() << name << " [" << lat << ":" << lon << "]";
	        dbThread->LoadPOIbyType(catName,lat,lon);

	    }
}

QStringList POILoader::loadPOIPlacesFromDBToStringList(int catID, QString catName,int subwayID)
{
	QMutexLocker locker(&mutex);
	QStringList dataList;
	QSqlQuery query;
	// SELECT id, name, latitude, longitude FROM whereami.places where category_id in (SELECT id FROM whereami.categories where parent_id=43);
	//QString getPlacesReq = QString("SELECT id, name, latitude, longitude "
	//		"FROM whereami.places where category_id in "
	//		"(SELECT id FROM whereami.categories where parent_id=%1);").arg(catID);

	QString getPlacesReq = QString("SELECT id, name, latitude, longitude "
				"FROM whereami.places where category_id=%1;").arg(catID);

	qDebug() << "Running sql request: " << getPlacesReq;

	DBThread         *dbThread=DBThread::GetInstance();

	query.exec(getPlacesReq);
	while (query.next()) {
	    	int id = query.value(0).toInt();
	        QString name = query.value(1).toString();
	        dataList << name;
	        double lat = query.value(2).toDouble();
	        double lon = query.value(3).toDouble();

	        qDebug() << name << " [" << lat << ":" << lon << "]";
	        dbThread->LoadPOIbyType(catName,lat,lon);

	    }
	return dataList;
}


void POILoader::loadPOIPlacesFromDBbyName(QString name)
{
	qDebug() << "Load POI by name: " << name;

	loadPOIPlacesFromDB(m_CatList[name],name,0);

}

QStringList POILoader::loadPOIPlacesFromDBbyNameToStringList(QString name)
{
	qDebug() << "Load POI by name: " << name;

	QStringList data = loadPOIPlacesFromDBToStringList(m_CatList[name],name,0);

	return data;
}

void POILoader::loadAllPOIs()
{
	foreach (QString v, m_CatNames){
		loadPOIPlacesFromDBbyName(v);
	}
}

static POILoader* ownPOIInstance=NULL;

bool POILoader::InitializeInstance()
{
	  if (ownPOIInstance!=NULL) {
	    return false;
	  }

	  ownPOIInstance=new POILoader();

	  return true;
}


POILoader* POILoader::GetInstance()
{
  return ownPOIInstance;
}

void POILoader::FreeInstance()
{
  delete ownPOIInstance;

  ownPOIInstance=NULL;
}

QStringList POILoader::getCatNames()
{
	QMutexLocker locker(&mutex);

	return m_CatNames;
}

void POILoader::loadPOICat(POICategoryListModel* catModel)
{
	QMutexLocker locker(&mutex);
	QSqlQuery query;

	if(!m_Ok)
		return;

	QString getCatReq = QString("SELECT id, name FROM whereami.categories where parent_id=0;");

	qDebug() << "Running sql request: " << getCatReq;

	DBThread         *dbThread=DBThread::GetInstance();

	query.exec(getCatReq);
	while (query.next()) {
	  	int id = query.value(0).toInt();
	    QString name = query.value(1).toString();
	    QString iconName = name + ".png";
	    catModel->addCategory(Category(name, iconName, id));
	}
}


void POILoader::loadPOISubCat(int catId, POICategoryListModel* catModel)
{
	QMutexLocker locker(&mutex);
	QSqlQuery query;

	if(!m_Ok)
		return;

	QString getSubCatReq = QString("SELECT id, name FROM whereami.categories where parent_id=%1;").arg(catId);

	qDebug() << "Running sql request: " << getSubCatReq;

	DBThread         *dbThread=DBThread::GetInstance();

	query.exec(getSubCatReq);

	while (query.next()) {
	  	int id = query.value(0).toInt();
	    QString name = query.value(1).toString();
	    //QString iconName = name + ".png";
	    catModel->addCategory(Category(name, nullptr, id));
	}

}
