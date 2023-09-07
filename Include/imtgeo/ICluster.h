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
class ICluster: virtual public imtgeo::IPosition
{
public:
	/**
		Get number of the positions in the cluster.
	*/
	virtual int GetChildrenCount() const = 0;
};


} // namespace imtgeo


