#pragma once

#include <QGeoCoordinate>


// ImtCore includes
#include <imtguigql/CObjectCollectionControllerCompBase.h>


namespace imtgeo
{


class CDeviceMapClusterCollectionControllerComp: public imtguigql::CObjectCollectionControllerCompBase
{
public:
	typedef imtguigql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CDeviceMapClusterCollectionControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (imtguigql::CObjectCollectionControllerCompBase)
	virtual QVariant GetObjectInformation(const QByteArray& informationId, const QByteArray& objectId) const;
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



