// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtgeo/CClusterCreator.h>


namespace imtgeo
{


// protected methods

QPair<double, double> CClusterCreator::coordToMercator(const QGeoCoordinate &coord) const
{
	const double pi = std::acos(-1);

	double lat = coord.latitude();
	double lon = coord.longitude() / 360.0 + 0.5;

	lat = 0.5 - (std::log(std::tan((pi / 4.0) + (pi / 2.0) * lat / 180.0)) / pi) / 2.0;
	lat = qBound(0.0, lat, 1.0);

	return { lon, lat };
}


double CClusterCreator::getDistanceBetweenObjects(const QGeoCoordinate &coord1, const QGeoCoordinate &coord2) const
{
	double dist = 0;

	double x_1 = coordToMercator(coord1).first;
	double y_1 = coordToMercator(coord1).second;
	double x_2 = coordToMercator(coord2).first;
	double y_2 = coordToMercator(coord2).second;

	dist = std::sqrtf((x_1 - x_2)*(x_1 - x_2) + (y_1 - y_2)*(y_1 - y_2));

	return dist;
}


double CClusterCreator::getDistanceLimitCoeff(double zoomLevel) const
{
	return std::pow(2, 19 - static_cast<int>(zoomLevel)) * 1.0536710607088955e-8 / 2.15;
}


QJsonArray CClusterCreator::createMapClusterModel(const QJsonArray& model, double zoomLevel, double limitInPixels) const
{
	QJsonArray retModel;

	if (model.isEmpty()) {
		return retModel;
	}

	const int itemsCount = model.size();

	// Structures to manage clusters
	struct ClusterData
	{
		int tmp = 0;
		int num = 0;
		double latitude = 0.0;
		double longitude = 0.0;
		QStringList objectIds;
		QString clusterState = QStringLiteral("offVis");
	};

	// Struct to hold precomputed/unpacked data for items to maximize inner loop performance
	struct PrecomputedItem
	{
		double x = 0.0;
		double y = 0.0;
		double lat = 0.0;
		double lon = 0.0;
		QString id;
		QJsonObject originalObj;
	};

	QList<PrecomputedItem> precomputedItems;
	precomputedItems.reserve(itemsCount);

	for (const QJsonValue& val : model) {
		const QJsonObject rawObj = val.toObject();
		const double lat = rawObj.value(QStringLiteral("latitude")).toDouble();
		const double lon = rawObj.value(QStringLiteral("longitude")).toDouble();

		const QGeoCoordinate coor(lat, lon);
		const QPair<double, double> mercator = coordToMercator(coor);

		PrecomputedItem item;
		item.x = mercator.first;
		item.y = mercator.second;
		item.lat = lat;
		item.lon = lon;
		item.id = rawObj.value(QStringLiteral("id")).toVariant().toString();
		item.originalObj = rawObj;
		precomputedItems.append(item);
	}

	QList<int> clusterIds(itemsCount, 0);
	QList<ClusterData> clusters(itemsCount);

	const double limit = limitInPixels * getDistanceLimitCoeff(zoomLevel);
	const double limitSq = limit * limit;
	int clusterCounter = 0;

	for (int i = 0; i < itemsCount; ++i) {
		if (clusterIds.at(i) != 0) {
			continue;
		}

		const PrecomputedItem& currentItem = precomputedItems.at(i);
		const double currentX = currentItem.x;
		const double currentY = currentItem.y;

		for (int j = i + 1; j < itemsCount; ++j) {
			if (clusterIds.at(j) != 0) {
				continue;
			}

			const PrecomputedItem& neighborItem = precomputedItems.at(j);
			const double dx = currentX - neighborItem.x;
			const double dy = currentY - neighborItem.y;

			// Optimization: avoid slow square root and std::fabs in inner loop by doing squared check
			const double distSq = dx * dx + dy * dy;
			if (distSq >= limitSq) {
				continue;
			}

			if (clusterIds.at(i) == 0) {
				clusterCounter++;
				clusterIds[i] = clusterCounter;

				clusters[i].num = clusterCounter;
				clusters[i].tmp = 1;
				clusters[i].latitude = currentItem.lat;
				clusters[i].longitude = currentItem.lon;
			}

			clusterIds[j] = clusterIds.at(i);
			clusters[i].tmp++;

			const QString& currentId = currentItem.id;
			const QString& neighborId = neighborItem.id;

			QStringList& cluster_objectIds = clusters[i].objectIds;
			if (!cluster_objectIds.contains(currentId)) {
				cluster_objectIds.append(currentId);
			}
			if (!cluster_objectIds.contains(neighborId)) {
				cluster_objectIds.append(neighborId);
			}
		}
	}

	for (ClusterData& cluster : clusters) {
		if (cluster.num > 0) {
			cluster.clusterState = QStringLiteral("onVis");
		}
		else {
			cluster.clusterState = QStringLiteral("offVis");
		}
	}

	// fill the returned model - items NOT in clusters
	for (int i = 0; i < itemsCount; ++i) {
		if (clusterIds.at(i) == 0) {
			QJsonObject obj = precomputedItems.at(i).originalObj;
			obj.insert(QStringLiteral("isCluster"), false);
			retModel.append(obj);
		}
	}

	// fill the returned model - visible clusters
	for (const ClusterData& cluster : std::as_const(clusters)) {
		if (cluster.clusterState == QStringLiteral("offVis")) {
			continue;
		}

		QJsonObject clusterObj;
		clusterObj.insert(QStringLiteral("tmp"), cluster.tmp);
		clusterObj.insert(QStringLiteral("num"), cluster.num);
		clusterObj.insert(QStringLiteral("latitude"), cluster.latitude);
		clusterObj.insert(QStringLiteral("longitude"), cluster.longitude);
		clusterObj.insert(QStringLiteral("clusterState"), cluster.clusterState);
		clusterObj.insert(QStringLiteral("numberChilds"), cluster.tmp);
		clusterObj.insert(QStringLiteral("objectIds"), cluster.objectIds.join(','));
		clusterObj.insert(QStringLiteral("isCluster"), true);

		retModel.append(clusterObj);
	}

	return retModel;
}


QList<ClusterSharedPtr> CClusterCreator::createMapClusters(const QList<CPositionIdentifiable*>& objectList, double zoomLevel, double limitInPixels) const
{
	QList<ClusterSharedPtr> clusterList;
	if (objectList.isEmpty()) {
		return clusterList;
	}

	const int itemsCount = objectList.size();

	struct PrecomputedItemData
	{
		double x = 0.0;
		double y = 0.0;
		double lat = 0.0;
		double lon = 0.0;
		QByteArray id;
	};

	struct ClusterItemData
	{
		int clusterId = 0;
		int childCount = 0;
		double latitude = 0.0;
		double longitude = 0.0;
		QByteArrayList objectIds;
	};

	QList<PrecomputedItemData> precomputedItems;
	precomputedItems.reserve(itemsCount);

	for (const CPositionIdentifiable* const object : objectList) {
		const double lat = object->GetLatitude();
		const double lon = object->GetLongitude();
		const auto mercator = coordToMercator(QGeoCoordinate(lat, lon));

		PrecomputedItemData item;
		item.x = mercator.first;
		item.y = mercator.second;
		item.lat = lat;
		item.lon = lon;
		item.id = object->GetObjectUuid();
		precomputedItems.append(item);
	}

	QList<int> clusterIds(itemsCount, 0);
	QList<ClusterItemData> localClusters(itemsCount);

	const double limit = limitInPixels * getDistanceLimitCoeff(zoomLevel);
	const double limitSq = limit * limit;
	int clusterCounter = 0;

	for (int i = 0; i < itemsCount; ++i) {
		if (clusterIds.at(i) != 0) {
			continue;
		}

		const auto& currentItem = precomputedItems.at(i);
		const double currentX = currentItem.x;
		const double currentY = currentItem.y;

		for (int j = i + 1; j < itemsCount; ++j) {
			if (clusterIds.at(j) != 0) {
				continue;
			}

			const auto& neighborItem = precomputedItems.at(j);
			const double dx = currentX - neighborItem.x;
			const double dy = currentY - neighborItem.y;

			// Optimization: avoid slow square root and std::fabs in inner loop by doing squared check
			const double distSq = dx * dx + dy * dy;
			if (distSq >= limitSq) {
				continue;
			}

			if (clusterIds.at(i) == 0) {
				clusterCounter++;
				clusterIds[i] = clusterCounter;

				localClusters[i].clusterId = clusterCounter;
				localClusters[i].childCount = 1;
				localClusters[i].latitude = currentItem.lat;
				localClusters[i].longitude = currentItem.lon;
			}

			clusterIds[j] = clusterIds.at(i);
			localClusters[i].childCount++;

			const QByteArray& currentId = currentItem.id;
			const QByteArray& neighborId = neighborItem.id;

			auto& cluster_objectIds = localClusters[i].objectIds;
			if (!cluster_objectIds.contains(currentId)) {
				cluster_objectIds.append(currentId);
			}
			if (!cluster_objectIds.contains(neighborId)) {
				cluster_objectIds.append(neighborId);
			}
		}
	}

	// fill the returned list - items NOT in clusters
	for (int i = 0; i < itemsCount; ++i) {
		if (clusterIds.at(i) == 0) {
			ClusterSharedPtr cluster = ClusterSharedPtr::create();
			cluster->SetLatitude(precomputedItems.at(i).lat);
			cluster->SetLongitude(precomputedItems.at(i).lon);
			cluster->SetZoom(zoomLevel);
			cluster->SetChildIds({ precomputedItems.at(i).id });
			clusterList.append(cluster);
		}
	}

	// fill the returned list - visible clusters
	for (const ClusterItemData& localCluster : std::as_const(localClusters)) {
		if (localCluster.clusterId > 0) {
			ClusterSharedPtr cluster = ClusterSharedPtr::create();
			cluster->SetLatitude(localCluster.latitude);
			cluster->SetLongitude(localCluster.longitude);
			cluster->SetZoom(zoomLevel);
			cluster->SetChildIds(localCluster.objectIds);
			clusterList.append(cluster);
		}
	}

	return clusterList;
}


QList<ClusterSharedPtr> CClusterCreator::convertToMapClusters(const QList<CPositionIdentifiable*>& objectList, double zoomLevel) const
{
	QList<ClusterSharedPtr> clusterList;
	clusterList.reserve(objectList.size());

	for (const CPositionIdentifiable* const object : objectList) {
		ClusterSharedPtr cluster = ClusterSharedPtr::create();
		cluster->SetLatitude(object->GetLatitude());
		cluster->SetLongitude(object->GetLongitude());
		cluster->SetZoom(zoomLevel);
		cluster->SetChildIds({ object->GetObjectUuid() });

		clusterList.append(cluster);
	}

	return clusterList;
}


} // namespace imtgeo

