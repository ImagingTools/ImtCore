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
    virtual QStringList GetChildrenIds() const = 0;

    /**
        Set id list of objects in cluster.
    */
    virtual void SetChildrenIds(QStringList &list) = 0;

};


} // namespace imtgeo


