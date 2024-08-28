#pragma once


// Qt includes
#include <QtPositioning/QGeoCoordinate>

// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtgeo/CPosition.h>
#include <imtgeo/IPosition.h>
#include <imtgeo/CCluster.h>

namespace imtgeo
{


class CTileProvider: virtual public istd::IPolymorphic
{
public:

    CTileProvider();
    ~CTileProvider();


protected:

	qint64 long2tilex(double lon, int z) const;
	qint64 lat2tiley(double lat, int z) const;
	double tilex2long(qint64 x, int z) const;
	double tiley2lat(qint64 y, int z) const;

	QList<QPair<QPair<qint64, qint64>, int>> getTileSet(QGeoCoordinate coordLeftTop, QGeoCoordinate coordRightBottom, int z) const;
	QPair<QPair<qint64, qint64>, int> getTile(QGeoCoordinate coord, int z) const;
	qint64 getTileNumber(QGeoCoordinate coord, int z) const;

private:

};

} // namespace imtgeo



