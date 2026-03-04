// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CTileProvider.h"


using namespace std;

namespace imtgeo
{

// public methods

CTileProvider::CTileProvider()
{
}


CTileProvider::~CTileProvider()
{
}


qint64 CTileProvider::long2tilex(double lon, int z) const
{
	return (qint64)(floor((lon + 180.0) / 360.0 * (1 << z)));
}


qint64 CTileProvider::lat2tiley(double lat, int z) const
{
    const double pi = acos(-1.0);

    double latrad = lat * pi / 180.0;
	return (qint64)(floor((1.0 - asinh(tan(latrad)) / pi) / 2.0 * (1 << z)));
}


double CTileProvider::tilex2long(qint64 x, int z) const
{
    return x / (double)(1 << z) * 360.0 - 180;
}


double CTileProvider::tiley2lat(qint64 y, int z) const
{
    const double pi = acos(-1.0);

    double n = pi - 2.0 * pi * y / (double)(1 << z);
    return 180.0 / pi * atan(0.5 * (exp(n) - exp(-n)));
}


QList<QPair<QPair<qint64, qint64>, int> > CTileProvider::getTileSet(const QGeoCoordinate& coordLeftTop, const QGeoCoordinate& coordRightBottom, int z) const
{
	QList<QPair<QPair<qint64, qint64>, int>> tileList;

	qint64 minX = long2tilex(coordLeftTop.longitude(), z);
	qint64 minY = lat2tiley(coordLeftTop.latitude(), z);
	qint64 maxX = long2tilex(coordRightBottom.longitude(), z);
	qint64 maxY = lat2tiley(coordRightBottom.latitude(), z);

	for (qint64 x = minX; x <= maxX; x++){
		for (qint64 y = minY; y <= maxY; y++){
			QPair<qint64, qint64> pairX_Y = qMakePair(x, y);
			QPair<QPair<qint64, qint64>, int> pairXY_Z = qMakePair(pairX_Y, z);
            tileList.append(pairXY_Z);
        }
    }

    return tileList;

}


QPair<QPair<qint64, qint64>, int> CTileProvider::getTile(const QGeoCoordinate& coord, int z) const
{
	QPair<QPair<qint64, qint64>, int> tileData;

	qint64 x = long2tilex(coord.longitude(), z);
	qint64 y = lat2tiley(coord.latitude(), z);
	QPair<qint64, qint64> coordPair = qMakePair(x, y);
    tileData = qMakePair(coordPair, z);

	return tileData;
}


qint64 CTileProvider::getTileNumber(const QGeoCoordinate& coord, int z) const
{
	qint64 tileNumber = 0;

	qint64 x = long2tilex(coord.longitude(), z);
	qint64 y = lat2tiley(coord.latitude(), z);

	qint64 tileCountInRow = (1 << z);

	tileNumber = tileCountInRow * y + x;

	return tileNumber;
}

// public methods



} // namespace imtgeo



