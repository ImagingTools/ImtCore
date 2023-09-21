#include "CTileProvider.h"

// STL includes
#include <cmath>

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


int CTileProvider::long2tilex(double lon, int z) const
{
    return (int)(floor((lon + 180.0) / 360.0 * (1 << z)));
}


int CTileProvider::lat2tiley(double lat, int z) const
{
    const double pi = acos(-1.0);

    double latrad = lat * pi / 180.0;
    return (int)(floor((1.0 - asinh(tan(latrad)) / pi) / 2.0 * (1 << z)));
}


double CTileProvider::tilex2long(int x, int z) const
{
    return x / (double)(1 << z) * 360.0 - 180;
}


double CTileProvider::tiley2lat(int y, int z) const
{
    const double pi = acos(-1.0);

    double n = pi - 2.0 * pi * y / (double)(1 << z);
    return 180.0 / pi * atan(0.5 * (exp(n) - exp(-n)));
}


QList<QPair<QPair<int, int>, int> > CTileProvider::getTileSet(QGeoCoordinate coordLeftTop, QGeoCoordinate coordRightBottom, int z) const
{
    QList<QPair<QPair<int, int>, int>> tileList;

    int minX = long2tilex(coordLeftTop.longitude(), z);
    int minY = lat2tiley(coordLeftTop.latitude(), z);
    int maxX = long2tilex(coordRightBottom.longitude(), z);
    int maxY = lat2tiley(coordRightBottom.latitude(), z);

    for (int x = minX; x <= maxX; x++){
        for (int y = minY; y <= maxY; y++){
            QPair<int, int> pairX_Y = qMakePair(x, y);
            QPair<QPair<int, int>, int> pairXY_Z = qMakePair(pairX_Y, z);
            tileList.append(pairXY_Z);
        }
    }

    return tileList;

}


QPair<QPair<int, int>, int> CTileProvider::getTile(QGeoCoordinate coord, int z) const
{
    QPair<QPair<int, int>, int> tileData;

    int x = long2tilex(coord.longitude(), z);
    int y = lat2tiley(coord.latitude(), z);
    QPair<int, int> coordPair = qMakePair(x, y);
    tileData = qMakePair(coordPair, z);

    return tileData;
}

// public methods



} // namespace imtgeo



