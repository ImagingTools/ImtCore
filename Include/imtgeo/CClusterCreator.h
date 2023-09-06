#pragma once


// Qt includes
#include <QtPositioning/QGeoCoordinate>

// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>

namespace imtgeo
{


class CClusterCreator: virtual public istd::IPolymorphic
{
public:

    CClusterCreator();
    ~CClusterCreator();


protected:

    QPair<double, double> coordToMercator(const QGeoCoordinate &coord) const;
    imtbase::CTreeItemModel* createMapClusterModel(imtbase::CTreeItemModel* model, double zoomLevel)const;


private:


};


} // namespace imtgeo



