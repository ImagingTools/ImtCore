// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

#include <QtPositioning/QGeoCoordinate>


// ImtCore includes
#include <imtservergql/CObjectCollectionControllerCompBase.h>


namespace imtgeo
{


class CDeviceMapClusterCollectionControllerComp: public imtservergql::CObjectCollectionControllerCompBase
{
public:
	typedef imtservergql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CDeviceMapClusterCollectionControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (imtservergql::CObjectCollectionControllerCompBase)
	virtual QVariant GetObjectInformation(const QByteArray& informationId, const QByteArray& objectId) const override;
	virtual imtbase::CTreeItemModel* GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* ListObjects(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	int long2tilex(double lon, int z) const;
	int lat2tiley(double lat, int z) const;
	double tilex2long(int x, int z) const;
	double tiley2lat(int y, int z) const;

	QList<QPair<QPair<int, int>, int>> getTileSet(QGeoCoordinate coordLeftTop, QGeoCoordinate coordRightBottom, int z) const;
	QPair<QPair<int, int>, int> getTile(QGeoCoordinate coord, int z) const;
};


} // namespace imtgeo



