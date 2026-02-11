// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
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

	QList<QPair<QPair<qint64, qint64>, int>> getTileSet(const QGeoCoordinate& coordLeftTop, const QGeoCoordinate& coordRightBottom, int z) const;
	QPair<QPair<qint64, qint64>, int> getTile(const QGeoCoordinate& coord, int z) const;
	qint64 getTileNumber(const QGeoCoordinate& coord, int z) const;

private:

};

} // namespace imtgeo



