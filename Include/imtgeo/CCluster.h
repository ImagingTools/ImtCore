#pragma once

// Qt includes
#include <QtPositioning/QGeoCoordinate>

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

    CCluster();

    ~CCluster();

    // reimplemented (imtgeo::ICluster)
    /**
        Get id list of objects in cluster.
    */
    virtual QStringList GetChildrenIds() const override;

    /**
        Set id list of objects in cluster.
    */
    virtual void SetChildrenIds(QStringList &list) override;

    // reimplemented (iser::ISerializable)
    virtual bool Serialize(iser::IArchive& archive) override;

private:

    QStringList m_childrenIds = QStringList();

};



} // namespace imtgeo


