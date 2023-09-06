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

class ICluster: virtual public iser::ISerializable, virtual public imtgeo::IPosition
{
public:

    /**
        Get longitude address.
    */
    virtual int GetChilderenCount() const = 0;

};


} // namespace nphandbooksgql


