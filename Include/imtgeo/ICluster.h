#pragma once

// ACF includes
#include <iser/IObject.h>
#include <iser/ISerializable.h>

// ImtCore includes
#include <imtgeo/IPosition.h>


namespace imtgeo
{
/**
    Interface for describing a cluster of objects on the map.
*/

class ICluster:  virtual public imtgeo::IPosition
{
public:

    /**
        Get id list of objects in cluster.
    */
    virtual QByteArrayList GetChildrenIds() const = 0;

    /**
        Set id list of objects in cluster.
    */
    virtual void SetChildrenIds(QByteArrayList &list) = 0;

    /**
        Get zoom level of cluster.
    */
    virtual double GetZoom() const = 0;

    /**
        Set zoom level of cluster.
    */
    virtual void SetZoom(double zoom) = 0;

};


} // namespace imtgeo


