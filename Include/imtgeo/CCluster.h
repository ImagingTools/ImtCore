#pragma once


// Qt includes
#include <QtPositioning/QGeoCoordinate>

// Acf includes
#include <iser/CPrimitiveTypesSerializer.h>


// ImtCore includes
#include <imtgeo/ICluster.h>
#include <imtgeo/CPosition.h>

namespace imtgeo
{
/**
    Class for describing a cluster of objects on the map.

*/
class CCluster: public CPositionIdentifiable, virtual public imtgeo::ICluster
{
public:
    typedef CPositionIdentifiable BaseClass;

    CCluster();

    ~CCluster();

    // reimplemented (imtgeo::ICluster)
    /**
        Get id list of objects in cluster.
    */
    virtual QByteArrayList GetChildrenIds() const override;

    /**
        Set id list of objects in cluster.
    */
    virtual void SetChildrenIds(QByteArrayList &list) override;

    /**
        Get zoom level of cluster.
    */
    virtual double GetZoom() const  override;

    /**
        Set zoom level of cluster.
    */
    virtual void SetZoom(double zoom)  override;

    // reimplemented (iser::ISerializable)
    virtual bool Serialize(iser::IArchive& archive) override;

private:

    QByteArrayList m_childrenIds = QByteArrayList();
    double m_zoom = 0;

};



} // namespace imtgeo


