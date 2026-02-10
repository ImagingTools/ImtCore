// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtgeo/CPosition.h>
#include <imtgeo/CCluster.h>

namespace imtgeo
{


class CClusterCreator: virtual public istd::IPolymorphic
{
public:

    CClusterCreator();
    ~CClusterCreator();

protected:
    QPair<double, double> coordToMercator(const QGeoCoordinate &coord) const;
    double getDistanceBetweenObjects(const QGeoCoordinate &coord1, const QGeoCoordinate &coord2) const;
    double getDistanceLimitCoeff(double zoomLevel) const;
    imtbase::CTreeItemModel* createMapClusterModel(imtbase::CTreeItemModel* model, double zoomLevel, double limitInPixels = 50)const;
    QList<CCluster*> createMapClusters(const QList<CPositionIdentifiable*>& objectList, double zoomLevel, double limitInPixels = 50)const;
    QList<CCluster*> convertToMapClusters(const QList<CPositionIdentifiable*>& objectList, double zoomLevel)const;

private:

};

} // namespace imtgeo



