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



