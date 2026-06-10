// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtgeo/CTileProvider.h>


namespace imtgeo
{


// protected methods

qint64 CTileProvider::long2tilex(double lon, int z) const
{
	return static_cast<qint64>(std::floor((lon + 180.0) / 360.0 * (1 << z)));
}


qint64 CTileProvider::lat2tiley(double lat, int z) const
{
	const double pi = std::acos(-1.0);
	double latrad = lat * pi / 180.0;

	return static_cast<qint64>(std::floor((1.0 - std::asinh(std::tan(latrad)) / pi) / 2.0 * (1 << z)));
}


double CTileProvider::tilex2long(qint64 x, int z) const
{
	return x / static_cast<double>(1 << z) * 360.0 - 180;
}


double CTileProvider::tiley2lat(qint64 y, int z) const
{
	const double pi = std::acos(-1.0);
	double n = pi - 2.0 * pi * y / static_cast<double>(1 << z);

	return 180.0 / pi * std::atan(0.5 * (std::exp(n) - std::exp(-n)));
}


QList<Tile> CTileProvider::getTileSet(
			const QGeoCoordinate& coordLeftTop,
			const QGeoCoordinate& coordRightBottom,
			int z) const
{
	QList<Tile> tileList;

	qint64 minX = long2tilex(coordLeftTop.longitude(), z);
	qint64 minY = lat2tiley(coordLeftTop.latitude(), z);
	qint64 maxX = long2tilex(coordRightBottom.longitude(), z);
	qint64 maxY = lat2tiley(coordRightBottom.latitude(), z);

	for (qint64 x = minX; x <= maxX; x++){
		for (qint64 y = minY; y <= maxY; y++){
			tileList.append(Tile{ x, y, z });
		}
	}

	return tileList;

}


Tile CTileProvider::getTile(const QGeoCoordinate& coord, int z) const
{
	qint64 x = long2tilex(coord.longitude(), z);
	qint64 y = lat2tiley(coord.latitude(), z);

	return Tile{ x, y, z };
}


qint64 CTileProvider::getTileNumber(const QGeoCoordinate& coord, int z) const
{
	qint64 x = long2tilex(coord.longitude(), z);
	qint64 y = lat2tiley(coord.latitude(), z);
	qint64 tileCountInRow = (1 << z);

	qint64 tileNumber = tileCountInRow * y + x;

	return tileNumber;
}


} // namespace imtgeo

