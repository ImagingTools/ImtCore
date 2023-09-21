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

    int long2tilex(double lon, int z) const;
    int lat2tiley(double lat, int z) const;
    double tilex2long(int x, int z) const;
    double tiley2lat(int y, int z) const;

    QList<QPair<QPair<int, int>, int>> getTileSet(QGeoCoordinate coordLeftTop, QGeoCoordinate coordRightBottom, int z) const;
    QPair<QPair<int, int>, int> getTile(QGeoCoordinate coord, int z) const;

private:

};

} // namespace imtgeo



