// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CClusterCreator.h"


using namespace std;

namespace imtgeo
{

// public methods

CClusterCreator::CClusterCreator()
{
}


CClusterCreator::~CClusterCreator()
{
}


QPair<double, double> CClusterCreator::coordToMercator(const QGeoCoordinate &coord) const
{
	const double pi = std::acos(-1);

	double lon = coord.longitude() / 360.0 + 0.5;

	double lat = coord.latitude();
	lat = 0.5 - (std::log(std::tan((pi / 4.0) + (pi / 2.0) * lat / 180.0)) / pi) / 2.0;
	lat = qBound(0.0, lat, 1.0);

	return qMakePair(lon, lat);

}


double CClusterCreator::getDistanceBetweenObjects(const QGeoCoordinate &coord1, const QGeoCoordinate &coord2) const
{
	double dist = 0;

	double x_1 = coordToMercator(coord1).first;
	double y_1 = coordToMercator(coord1).second;
	double x_2 = coordToMercator(coord2).first;
	double y_2 = coordToMercator(coord2).second;

	dist = sqrtf((x_1 - x_2)*(x_1 - x_2) + (y_1 - y_2)*(y_1 - y_2));

	return dist;
}


double CClusterCreator::getDistanceLimitCoeff(double zoomLevel) const
{
	return std::pow(2, 19 - int(zoomLevel)) * 1.0536710607088955e-8 / 2.15;
}


imtbase::CTreeItemModel *CClusterCreator::createMapClusterModel(imtbase::CTreeItemModel *model, double zoomLevel, double limitInPixels) const
{
	imtbase::CTreeItemModel* itemsModel = nullptr;
	imtbase::CTreeItemModel* clusterModel = nullptr;
	imtbase::CTreeItemModel* retModel = nullptr;

	itemsModel = new imtbase::CTreeItemModel();
	clusterModel = new imtbase::CTreeItemModel();
	retModel = new imtbase::CTreeItemModel();

	itemsModel->Copy(model);

	for(int i = 0; i < itemsModel->GetItemsCount(); i++){

		int index = clusterModel->InsertNewItem();
		clusterModel->SetData("tmp", 0, index);
		clusterModel->SetData("num", 0, index);
		clusterModel->SetData("numberChilds", 0, index);
		clusterModel->SetData("clusterState", "offVis", index);
		clusterModel->SetData("ObjectIds", QStringList(), index);
	}

	double koeff = getDistanceLimitCoeff(zoomLevel);


	double x_i;
	double x_j;
	double y_i;
	double y_j;

	double limit = limitInPixels * koeff;

	int numClaster = 0;

	for(int i = 0; i < itemsModel->GetItemsCount(); i++){
		itemsModel->SetData("numClaster", 0, i);
	}

	for(int i = 0; i < itemsModel->GetItemsCount(); i++){

		for(int j = i+1; j < itemsModel->GetItemsCount(); j++){

			double lat_i = itemsModel->GetData("Latitude", i).toDouble();
			double lat_j = itemsModel->GetData("Latitude", j).toDouble();
			double lon_i = itemsModel->GetData("Longitude", i).toDouble();
			double lon_j = itemsModel->GetData("Longitude", j).toDouble();

			QGeoCoordinate coor_i = QGeoCoordinate(lat_i, lon_i);
			QGeoCoordinate coor_j = QGeoCoordinate(lat_j, lon_j);

			x_i = coordToMercator(coor_i).first;
			y_i = coordToMercator(coor_i).second;
			x_j = coordToMercator(coor_j).first;
			y_j = coordToMercator(coor_j).second;

			int num = itemsModel->GetData("numClaster", i).toInt();
			double dist = sqrtf((x_i - x_j)*(x_i - x_j) + (y_i - y_j)*(y_i - y_j));

			bool toCluster = std::fabs(dist) < limit;

			if(toCluster){

				if(itemsModel->GetData("numClaster", j).toInt() == 0){

					if(num == 0){

						numClaster++;
						num = numClaster;

						clusterModel->SetData("tmp", 1, i);
						clusterModel->SetData("Latitude", itemsModel->GetData("Latitude", i), i);
						clusterModel->SetData("Longitude", itemsModel->GetData("Longitude", i), i);

					}
					if(clusterModel->GetData("tmp", i).toInt() > 0){

						itemsModel->SetData("numClaster", num, i);
						itemsModel->SetData("numClaster", num, j);

						clusterModel->SetData("num", num, i);
						clusterModel->SetData("tmp", clusterModel->GetData("tmp", i).toInt()+1, i);

					}

					QStringList ids = clusterModel->GetData("ObjectIds",i).toStringList();
					QString id_i = itemsModel->GetData("Id", i).toString();
					QString id_j = itemsModel->GetData("Id", j).toString();

					if(!ids.contains(id_i)){
						ids.append(id_i);
					}
					if(!ids.contains(id_j)){
						ids.append(id_j);
					}

					clusterModel->SetData("ObjectIds", ids, i);
				}
			}

		}
	}

	for( int i = 0; i < itemsModel->GetItemsCount(); i++){

		if(itemsModel->GetData("numClaster", i).toInt() > 0){

			if(!itemsModel->GetData("isInClaster", i).toBool()){
				itemsModel->SetData("isInClaster", true, i);
			}
		}

		else {
			if(itemsModel->GetData("isInClaster", i).toBool()){
				itemsModel->SetData("isInClaster", false, i);
			}
		}
	}

	for( int i = 0; i < clusterModel->GetItemsCount(); i++){
		if(clusterModel->GetData("num", i).toInt() > 0){
			if(clusterModel->GetData("clusterState", i).toString() != "onVis"){
				clusterModel->SetData("clusterState", "onVis", i);
			}
			if(clusterModel->GetData("numberChilds", i).toInt() != clusterModel->GetData("tmp", i).toInt()){
				clusterModel->SetData("numberChilds", clusterModel->GetData("tmp", i).toInt(), i);
			}
		}

		else {
			if(clusterModel->GetData("clusterState", i).toString() != "offVis"){
				clusterModel->SetData("clusterState", "offVis", i);
			}
		}
	}

	bool noextraItem = false;
	bool noextraCluster = false;

	//remove elements which in clusters
	while(!noextraItem){
		int whileCount = 0;
		for( int i = 0; i < itemsModel->GetItemsCount(); i++){
			bool inCluster = itemsModel->GetData("isInClaster", i).toBool();
			if(inCluster){
				itemsModel->RemoveItem(i);
				whileCount = 0;
				break;
			}
			whileCount++;
		}
		if(whileCount == itemsModel->GetItemsCount()){
			noextraItem = true;
		}

	}

	//remove invisible  clusters
	while(!noextraCluster){
		int whileCount = 0;
		for( int i = 0; i < clusterModel->GetItemsCount(); i++){
			bool offVis = clusterModel->GetData("clusterState", i).toString() == "offVis";
			if(offVis){
				clusterModel->RemoveItem(i);
				whileCount = 0;
				break;
			}

			//change QStringList for QString in the model
			QString idsString = clusterModel->GetData("ObjectIds",i).toStringList().join(",");
			clusterModel->SetData("ObjectIds",idsString,i);

			whileCount++;

		}
		if(whileCount == clusterModel->GetItemsCount()){
			noextraCluster = true;
		}

	}

	//fill the returned model
	for( int i = 0; i < itemsModel->GetItemsCount(); i++){
		int index = retModel->InsertNewItem();
		retModel->CopyItemDataFromModel(index, itemsModel, i);
		retModel->SetData("IsCluster", false, index);
	}

	for( int i = 0; i < clusterModel->GetItemsCount(); i++){
		int index = retModel->InsertNewItem();
		retModel->CopyItemDataFromModel(index, clusterModel, i);
		retModel->SetData("IsCluster", true,index);
	}

	return retModel;
}

QList<CCluster*> CClusterCreator::createMapClusters(const QList<CPositionIdentifiable*>& objectList, double zoomLevel, double limitInPixels) const
{
	QList<CCluster*> clusterList;

	imtbase::CTreeItemModel* itemsModel = nullptr;
	imtbase::CTreeItemModel* clusterModel = nullptr;

	itemsModel = new imtbase::CTreeItemModel();
	clusterModel = new imtbase::CTreeItemModel();


	for(int i = 0; i < objectList.length(); i++){
		CPositionIdentifiable* object = objectList.at(i);
		double lat = object->GetLatitude();
		double lon = object->GetLongitude();
		QByteArray uuid = object->GetObjectUuid();

		int index = itemsModel->InsertNewItem();
		itemsModel->SetData("Latitude", lat, index);
		itemsModel->SetData("Longitude", lon, index);
		itemsModel->SetData("Id", uuid, index);
	}

	for(int i = 0; i < itemsModel->GetItemsCount(); i++){

		int index = clusterModel->InsertNewItem();
		clusterModel->SetData("tmp", 0, index);
		clusterModel->SetData("num", 0, index);
		clusterModel->SetData("ObjectIds", QStringList(), index);
	}

	double koeff = getDistanceLimitCoeff(zoomLevel);

	double x_i;
	double x_j;
	double y_i;
	double y_j;


	double limit = limitInPixels * koeff;

	int numClaster = 0;


	for(int i = 0; i < itemsModel->GetItemsCount(); i++){
		itemsModel->SetData("numClaster", 0, i);
	}

	for(int i = 0; i < itemsModel->GetItemsCount(); i++){

		for(int j = i+1; j < itemsModel->GetItemsCount(); j++){

			double lat_i = itemsModel->GetData("Latitude", i).toDouble();
			double lat_j = itemsModel->GetData("Latitude", j).toDouble();
			double lon_i = itemsModel->GetData("Longitude", i).toDouble();
			double lon_j = itemsModel->GetData("Longitude", j).toDouble();

			QGeoCoordinate coor_i = QGeoCoordinate(lat_i, lon_i);
			QGeoCoordinate coor_j = QGeoCoordinate(lat_j, lon_j);

			x_i = coordToMercator(coor_i).first;
			y_i = coordToMercator(coor_i).second;
			x_j = coordToMercator(coor_j).first;
			y_j = coordToMercator(coor_j).second;


			int num = itemsModel->GetData("numClaster", i).toInt();
			double dist = sqrtf((x_i - x_j)*(x_i - x_j) + (y_i - y_j)*(y_i - y_j));

			bool toCluster = std::fabs(dist) < limit;

			if(toCluster){

				if(itemsModel->GetData("numClaster", j).toInt() == 0){

					if(num == 0){

						numClaster++;
						num = numClaster;

						clusterModel->SetData("tmp", 1, i);
						clusterModel->SetData("Latitude", itemsModel->GetData("Latitude", i), i);
						clusterModel->SetData("Longitude", itemsModel->GetData("Longitude", i), i);

					}
					if(clusterModel->GetData("tmp", i).toInt() > 0){
						itemsModel->SetData("numClaster", num, i);
						itemsModel->SetData("numClaster", num, j);

						clusterModel->SetData("num", num, i);
						clusterModel->SetData("tmp", clusterModel->GetData("tmp", i).toInt()+1, i);
					}

					QStringList ids = clusterModel->GetData("ObjectIds",i).toStringList();
					QString id_i = itemsModel->GetData("Id", i).toString();
					QString id_j = itemsModel->GetData("Id", j).toString();

					if(!ids.contains(id_i)){
						ids.append(id_i);
					}
					if(!ids.contains(id_j)){
						ids.append(id_j);
					}

					clusterModel->SetData("ObjectIds", ids, i);
				}
			}

		}

	}// двойной цикл


	bool noextraItem = false;
	bool noextraCluster = false;

	//remove elements which in clusters
	while(!noextraItem){
		int whileCount = 0;
		for( int i = 0; i < itemsModel->GetItemsCount(); i++){
			bool inCluster = itemsModel->GetData("numClaster", i).toInt() > 0;
			if(inCluster){
				itemsModel->RemoveItem(i);
				whileCount = 0;
				break;
			}

			whileCount++;
		}

		if(whileCount == itemsModel->GetItemsCount()){
			noextraItem = true;
		}
	}

	//remove invisible  clusters
	while(!noextraCluster){
		int whileCount = 0;
		for( int i = 0; i < clusterModel->GetItemsCount(); i++){
			bool offVis = clusterModel->GetData("num", i).toInt() <= 0;
			if(offVis){
				clusterModel->RemoveItem(i);
				whileCount = 0;
				break;
			}

			whileCount++;
		}

		if(whileCount == clusterModel->GetItemsCount()){
			noextraCluster = true;
		}

	}

	//filling the returned list
	for( int i = 0; i < itemsModel->GetItemsCount(); i++){
		imtgeo::CCluster* cluster = new imtgeo::CCluster();
		cluster->SetLatitude(itemsModel->GetData("Latitude",i).toDouble());
		cluster->SetLongitude(itemsModel->GetData("Longitude",i).toDouble());
		cluster->SetZoom(zoomLevel);
		QByteArray id = itemsModel->GetData("Id", i).toByteArray();
		QByteArrayList listBA;
		listBA << id;
		cluster->SetChildrenIds(listBA);
		clusterList.append(cluster);
	}

	for( int i = 0; i < clusterModel->GetItemsCount(); i++){
		imtgeo::CCluster* cluster = new imtgeo::CCluster();
		cluster->SetLatitude(clusterModel->GetData("Latitude",i).toDouble());
		cluster->SetLongitude(clusterModel->GetData("Longitude",i).toDouble());
		cluster->SetZoom(zoomLevel);
		QStringList ids = clusterModel->GetData("ObjectIds", i).toStringList();
		QByteArrayList idsBA;
		for(const QString& id: std::as_const(ids)){
			idsBA.append(id.toUtf8());
		}
		cluster->SetChildrenIds(idsBA);
		clusterList.append(cluster);
	}

	return clusterList;
}


QList<CCluster *> CClusterCreator::convertToMapClusters(const QList<CPositionIdentifiable *> &objectList, double zoomLevel) const
{
	QList<CCluster*> clusterList;

	for(int i = 0; i < objectList.length(); i++){
		CPositionIdentifiable* object = objectList.at(i);
		double lat = object->GetLatitude();
		double lon = object->GetLongitude();
		QByteArray uuid = object->GetObjectUuid();

		imtgeo::CCluster* cluster = new imtgeo::CCluster();
		cluster->SetLatitude(lat);
		cluster->SetLongitude(lon);
		cluster->SetZoom(zoomLevel);
		QByteArrayList listBA;
		listBA << uuid;
		cluster->SetChildrenIds(listBA);
		clusterList.append(cluster);
	}

	return clusterList;
}


// public methods



} // namespace imtgeo



