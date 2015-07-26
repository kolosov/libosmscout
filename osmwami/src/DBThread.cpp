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

#include "DBThread.h"

#include <cmath>
#include <iostream>

#include <QApplication>
#include <QMutexLocker>
#include <QDebug>
#include <QDir>

#include <osmscout/util/StopClock.h>

QBreaker::QBreaker()
  :osmscout::Breaker()
  ,aborted(false)
{
}

bool QBreaker::Break(){
  QMutexLocker locker(&mutex);
  aborted = true;
  return true;
}

bool QBreaker::IsAborted() const
{
  QMutexLocker locker(&mutex);
  return aborted;
}

void QBreaker::Reset()
{
  QMutexLocker locker(&mutex);
  aborted = false;
}


DBThread::DBThread()
 : database(new osmscout::Database(databaseParameter)),
   locationService(new osmscout::LocationService(database)),
   mapService(new osmscout::MapService(database)),
   painter(NULL),
   iconDirectory(),
   currentImage(NULL),
   currentLat(0.0),
   currentLon(0.0),
   currentAngle(0.0),
   currentMagnification(0),
   finishedImage(NULL),
   finishedLat(0.0),
   finishedLon(0.0),
   finishedMagnification(0),
   currentRenderRequest(),
   doRender(false),
   renderBreaker(new QBreaker()),
   renderBreakerRef(renderBreaker)
{
    QScreen *srn = QApplication::screens().at(0);

    dpi = (double)srn->physicalDotsPerInch();
}

void DBThread::FreeMaps()
{
  delete currentImage;
  currentImage=NULL;

  delete finishedImage;
  finishedImage=NULL;
}

bool DBThread::AssureRouter(osmscout::Vehicle vehicle)
{
  if (!database->IsOpen()) {
    return false;
  }

  if (!router ||
      (router && router->GetVehicle()!=vehicle)) {
    if (router) {
      if (router->IsOpen()) {
        router->Close();
      }
      router=NULL;
    }

    router=std::make_shared<osmscout::RoutingService>(database,
                                                      routerParameter,
                                                      vehicle);

    if (!router->Open()) {
      return false;
    }
  }

  return true;
}

void DBThread::Initialize()
{
#ifdef __ANDROID__
    QStringList docPaths=QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);

    QString databaseDirectory;

    // look for standard.oss in each directory
    for(int i=0; i < docPaths.size(); i++) {
        QStringList list_filters;
        list_filters << "osmscout";

        QDir path(docPaths[i]);
        QStringList list_files = path.entryList(list_filters,QDir::NoDotAndDotDot | QDir::Dirs);

        if(!(list_files.size() == 1)) {
            continue;
        }

        databaseDirectory=path.canonicalPath()+"/osmscout";
    }

    if(databaseDirectory.length() == 0) {
        qDebug() << "ERROR: map database directory not found";
    }
    else {
        qDebug() << "Loading database from " << databaseDirectory;
    }

    QString stylesheetFilename=databaseDirectory+"/standard.oss";

    qDebug() << "Loading style sheet from " << stylesheetFilename;

#else
  QStringList cmdLineArgs = QApplication::arguments();
  QString databaseDirectory = cmdLineArgs.size() > 1 ? cmdLineArgs.at(1) : QDir::currentPath();
  QString stylesheetFilename = cmdLineArgs.size() > 2 ? cmdLineArgs.at(2) : databaseDirectory + QDir::separator() + "standard.oss";
  iconDirectory = cmdLineArgs.size() > 3 ? cmdLineArgs.at(3) : databaseDirectory + QDir::separator() + "icons" + QDir::separator();
#endif

  if (database->Open(databaseDirectory.toLocal8Bit().data())) {
    osmscout::TypeConfigRef typeConfig=database->GetTypeConfig();

    if (typeConfig) {
      styleConfig=std::make_shared<osmscout::StyleConfig>(typeConfig);

      delete painter;
      painter=NULL;

      if (styleConfig->Load(stylesheetFilename.toLocal8Bit().data())) {
          painter=new osmscout::MapPainterQt(styleConfig);
    	  //painter=new osmscout::POIMapPainter(styleConfig);
      }
      else {
        qDebug() << "Cannot load style sheet!";
        styleConfig=NULL;
      }
    }
    else {
      qDebug() << "TypeConfig invalid!";
      styleConfig=NULL;
    }
  }
  else {
    qDebug() << "Cannot open database!";
    return;
  }


  DatabaseLoadedResponse response;

  if (!database->GetBoundingBox(response.boundingBox)) {
    qDebug() << "Cannot read initial bounding box";
    return;
  }

  emit InitialisationFinished(response);
}

void DBThread::Finalize()
{
  FreeMaps();

  if (router && router->IsOpen()) {
    router->Close();
  }

  if (database->IsOpen()) {
    database->Close();
  }
}

bool DBThread::LoadPOI(osmscout::GeoCoord coord)
{
	//mutex

	//osmscout::Node aNode;
	//osmscout::Node aNode;

	double aLat =  currentRenderRequest.lat;
	double aLon =  currentRenderRequest.lon;

	double deltaLat = 0.00354;
	double deltaLon = 0.00927;

	double placeLat = aLat + deltaLat/2;
	double placeLon = aLon + deltaLon/2;
	std::cout << "Place lat: " << placeLat << " lon: " << placeLon << std::endl;

	//prepare Node TypeInfo
	const osmscout::TypeInfoRef curNodeType = std::make_shared<osmscout::TypeInfo>();
	//osmscout::TypeId NodeId = 0xAAAA;
	//const std::string& NodeGroupName = "MyCafe";
	//curNodeType->SetNodeId(NodeId);
	//curNodeType->AddGroup(NodeGroupName);



	//prepare features
	//osmscout::FeatureValueBuffer featureBuf;
	//FeatureValueBuffer buffer = way->GetFeatureValueBuffer();
	//osmscout::TypeConfigRef aTextConf = std::make_shared<osmscout::TypeConfig>();
	//osmscout::TextStyleRef aTextStyle = new osmscout::TextStyle();
	//osmscout::IconStyleRef aIconStyle = new osmscout::IconStyle();

	//prepare icon name
	//const std::string& Pin1IconName = "Pin_10_1";
	const std::string& Pin1IconName = "amenity_parking";

	//aIconStyle->SetIconName(Pin1IconName);
	curNodeType->SetType(Pin1IconName);

	//osmscout::TypeConfigRef MainTypeConig = database->GetTypeConfig();

	//featureBuf.SetType(aTypeConig);

	//prepare Node
	osmscout::NodeRef aNode = new osmscout::Node();
	//aNode.Get()->SetCoords(coord);
	osmscout::GeoCoord newCoord;
	newCoord.lat = placeLat;
	newCoord.lon = placeLon;
	aNode.Get()->SetCoords(newCoord);

	aNode.Get()->SetType(curNodeType);

	//const osmscout::TypeInfoRef aTypeInfo = std::make_shared<osmscout::TypeInfo>();

	//aTypeInfo->SetType("aaa");

	//featureBuf.SetType(aTypeInfo);

	//osmscout::TypeInfoRef type = featureBuf.GetType();

	//aNode.Get()->SetFeatures(featureBuf);

	//osmscout::TypeInfoRef curNodeType = aNode.Get()->GetType();

	data.poiNodes.push_back(aNode);
/*
	osmscout::MapParameter        drawParameter;
	osmscout::AreaSearchParameter searchParameter;

	searchParameter.SetBreaker(renderBreakerRef);
	searchParameter.SetMaximumAreaLevel(4);
	searchParameter.SetUseMultithreading(currentMagnification.GetMagnification()<=osmscout::Magnification::magCity);

	std::list<std::string>        paths;

	paths.push_back(iconDirectory.toLocal8Bit().data());

	drawParameter.SetIconPaths(paths);
	drawParameter.SetPatternPaths(paths);
	drawParameter.SetDebugPerformance(true);
	drawParameter.SetOptimizeWayNodes(osmscout::TransPolygon::quality);
	drawParameter.SetOptimizeAreaNodes(osmscout::TransPolygon::quality);
	drawParameter.SetRenderBackground(true);
	drawParameter.SetRenderSeaLand(true);
	drawParameter.SetBreaker(renderBreakerRef);

	osmscout::MercatorProjection projection;
	GetProjection(projection);


	painter->AddPOILabel(projection, drawParameter, coord.lat, coord.lon, "HHHHHHHEEEEEELLLLLLLLLLOOOOOOO");
*/
	return true;
//	painter->DrawIcon();
}

void DBThread::GetProjection(osmscout::MercatorProjection& projection)
{
    QMutexLocker locker(&mutex);

    projection=this->projection;
}

void DBThread::UpdateRenderRequest(const RenderMapRequest& request)
{
  QMutexLocker locker(&mutex);

  currentRenderRequest=request;
  doRender=true;

  renderBreaker->Break();
}

void DBThread::TriggerMapRendering()
{
  RenderMapRequest request;
  {
    QMutexLocker locker(&mutex);

    request=currentRenderRequest;
    if (!doRender) {
      return;
    }

    doRender=false;

    renderBreaker->Reset();
  }

  if (currentImage==NULL ||
      currentImage->width()!=(int)request.width ||
      currentImage->height()!=(int)request.height) {
    delete currentImage;

    currentImage=new QImage(QSize(request.width,request.height),QImage::Format_RGB32);
  }

  currentLon=request.lon;
  currentLat=request.lat;
  currentAngle=request.angle;
  currentMagnification=request.magnification;

  if (database->IsOpen() &&
      styleConfig) {
    osmscout::MapParameter        drawParameter;
    osmscout::AreaSearchParameter searchParameter;

    searchParameter.SetBreaker(renderBreakerRef);
    searchParameter.SetMaximumAreaLevel(4);
    searchParameter.SetUseMultithreading(currentMagnification.GetMagnification()<=osmscout::Magnification::magCity);

    std::list<std::string>        paths;

    paths.push_back(iconDirectory.toLocal8Bit().data());

    drawParameter.SetIconPaths(paths);
    drawParameter.SetPatternPaths(paths);
    drawParameter.SetDebugPerformance(true);
    drawParameter.SetOptimizeWayNodes(osmscout::TransPolygon::quality);
    drawParameter.SetOptimizeAreaNodes(osmscout::TransPolygon::quality);
    drawParameter.SetRenderBackground(true);
    drawParameter.SetRenderSeaLand(true);
    drawParameter.SetBreaker(renderBreakerRef);

    std::cout << std::endl;

    osmscout::StopClock overallTimer;

    projection.Set(currentLon,
                   currentLat,
                   currentAngle,
                   currentMagnification,
                   dpi,
                   request.width,
                   request.height);

    osmscout::StopClock dataRetrievalTimer;

    mapService->GetObjects(searchParameter,
                           *styleConfig,
                           projection,
                           data);

    if (drawParameter.GetRenderSeaLand()) {
      mapService->GetGroundTiles(projection,
                                 data.groundTiles);
    }

    dataRetrievalTimer.Stop();

    osmscout::StopClock drawTimer;

    QPainter p;

    p.begin(currentImage);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);

    painter->DrawMap(projection,
                     drawParameter,
                     data,
                     &p);

    p.end();

    drawTimer.Stop();
    overallTimer.Stop();

    std::cout << "All: " << overallTimer << " Data: " << dataRetrievalTimer << " Draw: " << drawTimer << std::endl;
  }
  else {
    std::cout << "Cannot draw map: " << database->IsOpen() << " " << styleConfig << std::endl;

    QPainter p;

    p.begin(currentImage);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);

    p.fillRect(0,0,request.width,request.height,
               QColor::fromRgbF(0.0,0.0,0.0,1.0));

    p.setPen(QColor::fromRgbF(1.0,1.0,1.0,1.0));

    QString text("not initialized (yet)");

    p.drawText(QRect(0,0,request.width,request.height),
               Qt::AlignCenter|Qt::AlignVCenter,
               text,
               NULL);

    p.end();
  }

  QMutexLocker locker(&mutex);

  if (renderBreaker->IsAborted()) {
    return;
  }

  std::swap(currentImage,finishedImage);
  std::swap(currentLon,finishedLon);
  std::swap(currentLat,finishedLat);
  std::swap(currentAngle,finishedAngle);
  std::swap(currentMagnification,finishedMagnification);

  emit HandleMapRenderingResult();
}

bool DBThread::RenderMap(QPainter& painter,
                         const RenderMapRequest& request)
{
  QMutexLocker locker(&mutex);

  if (finishedImage==NULL || !styleConfig) {
    painter.fillRect(0,0,request.width,request.height,
                     QColor::fromRgbF(0.0,0.0,0.0,1.0));

    painter.setPen(QColor::fromRgbF(1.0,1.0,1.0,1.0));

    QString text("no map available");

    painter.drawText(QRectF(0,0,request.width,request.height),
                     text,
                     QTextOption(Qt::AlignCenter));

    return false;
  }

  projection.Set(finishedLon,finishedLat,
                 finishedAngle,
                 finishedMagnification,
                 finishedImage->width(),
                 finishedImage->height());

  osmscout::GeoBox boundingBox;

  projection.GetDimensions(boundingBox);

  double d=boundingBox.GetWidth()*2*M_PI/360;
  double scaleSize;
  size_t minScaleWidth=request.width/20;
  size_t maxScaleWidth=request.width/10;
  double scaleValue=d*180*60/M_PI*1852.216/(request.width/minScaleWidth);

  //std::cout << "1/10 screen (" << width/10 << " pixels) are: " << scaleValue << " meters" << std::endl;

  scaleValue=pow(10,floor(log10(scaleValue))+1);
  scaleSize=scaleValue/(d*180*60/M_PI*1852.216/request.width);

  if (scaleSize>minScaleWidth && scaleSize/2>minScaleWidth && scaleSize/2<=maxScaleWidth) {
    scaleValue=scaleValue/2;
    scaleSize=scaleSize/2;
  }
  else if (scaleSize>minScaleWidth && scaleSize/5>minScaleWidth && scaleSize/5<=maxScaleWidth) {
    scaleValue=scaleValue/5;
    scaleSize=scaleSize/5;
  }
  else if (scaleSize>minScaleWidth && scaleSize/10>minScaleWidth && scaleSize/10<=maxScaleWidth) {
    scaleValue=scaleValue/10;
    scaleSize=scaleSize/10;
  }

  //std::cout << "VisualScale: value: " << scaleValue << " pixel: " << scaleSize << std::endl;

  double dx=0;
  double dy=0;
  if (request.lon!=finishedLon || request.lat!=finishedLat) {
      double rx,ry,fx,fy;

      if (projection.GeoToPixel(request.lon,
                                request.lat,
                                rx,
                                ry) &&
          projection.GeoToPixel(finishedLon,
                                finishedLat,
                                fx,
                                fy)) {
          dx=fx-rx;
          dy=fy-ry;
      }
  }

  if (dx!=0 ||
      dy!=0) {
    osmscout::FillStyleRef unknownFillStyle;
    osmscout::Color        backgroundColor;

    styleConfig->GetUnknownFillStyle(projection,
                                     unknownFillStyle);

    if (unknownFillStyle) {
      backgroundColor=unknownFillStyle->GetFillColor();
    }
    else {
        backgroundColor=osmscout::Color(0,0,0);
    }

    painter.fillRect(0,
                     0,
                     projection.GetWidth(),
                     projection.GetHeight(),
                     QColor::fromRgbF(backgroundColor.GetR(),
                                      backgroundColor.GetG(),
                                      backgroundColor.GetB(),
                                      backgroundColor.GetA()));
  }

  painter.drawImage(dx,dy,*finishedImage);

  //QString testText("TEST TEST TEST");
  //double myPlaceX = 55.74352, myPlaceY = 37.60600, myX, myY;
  //projection.GeoToPixel(myPlaceX, myPlaceY,myX, myY);

//  qDebug() << "Screen coord: " << "x=" << myX << " y=" << myY <<
//		  " width=" << request.width << " height=" << request.height;
  //painter.drawText(QRectF(0,0,request.width,request.height),
  //painter.drawText(QRectF(100,100,200.0,200.0),
//                       testText,
//                       QTextOption(Qt::AlignCenter));

  return finishedImage->width()==(int)request.width &&
         finishedImage->height()==(int)request.height &&
         finishedLon==request.lon &&
         finishedLat==request.lat &&
         finishedAngle==request.angle &&
         finishedMagnification==request.magnification;
}

osmscout::TypeConfigRef DBThread::GetTypeConfig() const
{
  return database->GetTypeConfig();
}

bool DBThread::GetNodeByOffset(osmscout::FileOffset offset,
                               osmscout::NodeRef& node) const
{
  QMutexLocker locker(&mutex);

  return database->GetNodeByOffset(offset,node);
}

bool DBThread::GetAreaByOffset(osmscout::FileOffset offset,
                               osmscout::AreaRef& area) const
{
  QMutexLocker locker(&mutex);

  return database->GetAreaByOffset(offset,area);
}

bool DBThread::GetWayByOffset(osmscout::FileOffset offset,
                              osmscout::WayRef& way) const
{
  QMutexLocker locker(&mutex);

  return database->GetWayByOffset(offset,way);
}

bool DBThread::ResolveAdminRegionHierachie(const osmscout::AdminRegionRef& adminRegion,
                                           std::map<osmscout::FileOffset,osmscout::AdminRegionRef >& refs) const
{
  QMutexLocker locker(&mutex);

  return locationService->ResolveAdminRegionHierachie(adminRegion,
                                                      refs);
}

bool DBThread::SearchForLocations(const std::string& searchPattern,
                                  size_t limit,
                                  osmscout::LocationSearchResult& result) const
{
  QMutexLocker locker(&mutex);


  osmscout::LocationSearch search;

  search.limit=limit;

  if (!locationService->InitializeLocationSearchEntries(searchPattern,
                                                        search)) {
      return false;
  }

  return locationService->SearchForLocations(search,
                                             result);
}

bool DBThread::CalculateRoute(osmscout::Vehicle vehicle,
                              const osmscout::RoutingProfile& routingProfile,
                              const osmscout::ObjectFileRef& startObject,
                              size_t startNodeIndex,
                              const osmscout::ObjectFileRef targetObject,
                              size_t targetNodeIndex,
                              osmscout::RouteData& route)
{
  QMutexLocker locker(&mutex);

  if (!AssureRouter(vehicle)) {
    return false;
  }

  return router->CalculateRoute(routingProfile,
                                startObject,
                                startNodeIndex,
                                targetObject,
                                targetNodeIndex,
                                route);
}

bool DBThread::TransformRouteDataToRouteDescription(osmscout::Vehicle vehicle,
                                                    const osmscout::RoutingProfile& routingProfile,
                                                    const osmscout::RouteData& data,
                                                    osmscout::RouteDescription& description)
{
	qDebug() << "Don't call me!!!";
	return false;

  QMutexLocker locker(&mutex);

  if (!AssureRouter(vehicle)) {
    return false;
  }

  if (!router->TransformRouteDataToRouteDescription(data,description)) {
    return false;
  }

  /*
  osmscout::TypeConfigRef typeConfig=router->GetTypeConfig();

  std::list<osmscout::RoutePostprocessor::PostprocessorRef> postprocessors;

  postprocessors.push_back(std::make_shared<osmscout::RoutePostprocessor::DistanceAndTimePostprocessor>());
  postprocessors.push_back(std::make_shared<osmscout::RoutePostprocessor::StartPostprocessor>(start));
  postprocessors.push_back(std::make_shared<osmscout::RoutePostprocessor::TargetPostprocessor>(target));
  postprocessors.push_back(std::make_shared<osmscout::RoutePostprocessor::WayNamePostprocessor>());
  postprocessors.push_back(std::make_shared<osmscout::RoutePostprocessor::CrossingWaysPostprocessor>());
  postprocessors.push_back(std::make_shared<osmscout::RoutePostprocessor::DirectionPostprocessor>());

  osmscout::RoutePostprocessor::InstructionPostprocessorRef instructionProcessor=std::make_shared<osmscout::RoutePostprocessor::InstructionPostprocessor>();

  instructionProcessor->AddMotorwayType(typeConfig->GetTypeInfo("highway_motorway"));
  instructionProcessor->AddMotorwayLinkType(typeConfig->GetTypeInfo("highway_motorway_link"));
  instructionProcessor->AddMotorwayType(typeConfig->GetTypeInfo("highway_motorway_trunk"));
  instructionProcessor->AddMotorwayType(typeConfig->GetTypeInfo("highway_trunk"));
  instructionProcessor->AddMotorwayLinkType(typeConfig->GetTypeInfo("highway_trunk_link"));
  instructionProcessor->AddMotorwayType(typeConfig->GetTypeInfo("highway_motorway_primary"));
  postprocessors.push_back(instructionProcessor);

  if (!routePostprocessor.PostprocessRouteDescription(description,
                                                      routingProfile,
                                                      *database,
                                                      postprocessors)) {
    return false;
  }*/

  return true;
}


bool DBThread::TransformRouteDataToRouteDescription(osmscout::Vehicle vehicle,
                                                    const osmscout::RoutingProfile& routingProfile,
                                                    const osmscout::RouteData& data,
                                                    osmscout::RouteDescription& description,
                                                    const std::string& start,
                                                    const std::string& target)
{
  QMutexLocker locker(&mutex);

  if (!AssureRouter(vehicle)) {
    return false;
  }

  if (!router->TransformRouteDataToRouteDescription(data,description)) {
    return false;
  }

  osmscout::TypeConfigRef typeConfig=router->GetTypeConfig();

  std::list<osmscout::RoutePostprocessor::PostprocessorRef> postprocessors;

  postprocessors.push_back(std::make_shared<osmscout::RoutePostprocessor::DistanceAndTimePostprocessor>());
  postprocessors.push_back(std::make_shared<osmscout::RoutePostprocessor::StartPostprocessor>(start));
  postprocessors.push_back(std::make_shared<osmscout::RoutePostprocessor::TargetPostprocessor>(target));
  postprocessors.push_back(std::make_shared<osmscout::RoutePostprocessor::WayNamePostprocessor>());
  postprocessors.push_back(std::make_shared<osmscout::RoutePostprocessor::CrossingWaysPostprocessor>());
  postprocessors.push_back(std::make_shared<osmscout::RoutePostprocessor::DirectionPostprocessor>());

  osmscout::RoutePostprocessor::InstructionPostprocessorRef instructionProcessor=std::make_shared<osmscout::RoutePostprocessor::InstructionPostprocessor>();

  instructionProcessor->AddMotorwayType(typeConfig->GetTypeInfo("highway_motorway"));
  instructionProcessor->AddMotorwayLinkType(typeConfig->GetTypeInfo("highway_motorway_link"));
  instructionProcessor->AddMotorwayType(typeConfig->GetTypeInfo("highway_motorway_trunk"));
  instructionProcessor->AddMotorwayType(typeConfig->GetTypeInfo("highway_trunk"));
  instructionProcessor->AddMotorwayLinkType(typeConfig->GetTypeInfo("highway_trunk_link"));
  instructionProcessor->AddMotorwayType(typeConfig->GetTypeInfo("highway_motorway_primary"));
  postprocessors.push_back(instructionProcessor);

  if (!routePostprocessor.PostprocessRouteDescription(description,
                                                      routingProfile,
                                                      *database,
                                                      postprocessors)) {
    return false;
  }

  return true;
}

bool DBThread::TransformRouteDataToWay(osmscout::Vehicle vehicle,
                                       const osmscout::RouteData& data,
                                       osmscout::Way& way)
{
  QMutexLocker locker(&mutex);

  if (!AssureRouter(vehicle)) {
    return false;
  }

  return router->TransformRouteDataToWay(data,way);
}


void DBThread::ClearRoute()
{
  QMutexLocker locker(&mutex);

  data.poiWays.clear();

  FreeMaps();

  emit Redraw();
}

void DBThread::AddRoute(const osmscout::Way& way)
{
  QMutexLocker locker(&mutex);

  data.poiWays.push_back(new osmscout::Way(way));

  FreeMaps();

  emit Redraw();
}

bool DBThread::GetClosestRoutableNode(const double lat, const double lon,
                                      const osmscout::Vehicle& vehicle,
                                      double radius,
                                      osmscout::ObjectFileRef& object,
                                      size_t& nodeIndex)
{
  QMutexLocker locker(&mutex);

  if (!AssureRouter(vehicle)) {
    return false;
  }

  object.Invalidate();

  return router->GetClosestRoutableNode(lat, lon,
                                            vehicle,
                                            radius,
                                            object,
                                            nodeIndex);

}

bool DBThread::GetClosestRoutableNode(const osmscout::ObjectFileRef& refObject,
                                      const osmscout::Vehicle& vehicle,
                                      double radius,
                                      osmscout::ObjectFileRef& object,
                                      size_t& nodeIndex)
{
  QMutexLocker locker(&mutex);

  if (!AssureRouter(vehicle)) {
    return false;
  }

  object.Invalidate();

  if (refObject.GetType()==osmscout::refNode) {
    osmscout::NodeRef node;

    if (!database->GetNodeByOffset(refObject.GetFileOffset(),
                                   node)) {
      return false;
    }

    return router->GetClosestRoutableNode(node->GetCoords().GetLat(),
                                          node->GetCoords().GetLon(),
                                          vehicle,
                                          radius,
                                          object,
                                          nodeIndex);
  }
  else if (refObject.GetType()==osmscout::refArea) {
    osmscout::AreaRef area;

    if (!database->GetAreaByOffset(refObject.GetFileOffset(),
                                   area)) {
      return false;
    }

    osmscout::GeoCoord center;

    area->GetCenter(center);

    return router->GetClosestRoutableNode(center.GetLat(),
                                          center.GetLon(),
                                          vehicle,
                                          radius,
                                          object,
                                          nodeIndex);
  }
  else if (refObject.GetType()==osmscout::refWay) {
    osmscout::WayRef way;

    if (!database->GetWayByOffset(refObject.GetFileOffset(),
                                  way)) {
      return false;
    }

    return router->GetClosestRoutableNode(way->nodes[0].GetLat(),
                                          way->nodes[0].GetLon(),
                                          vehicle,
                                          radius,
                                          object,
                                          nodeIndex);
  }
  else {
    return true;
  }
}

static DBThread* dbThreadInstance=NULL;

bool DBThread::InitializeInstance()
{
  if (dbThreadInstance!=NULL) {
    return false;
  }

  dbThreadInstance=new DBThread();

  return true;
}

DBThread* DBThread::GetInstance()
{
  return dbThreadInstance;
}

void DBThread::FreeInstance()
{
  delete dbThreadInstance;

  dbThreadInstance=NULL;
}
