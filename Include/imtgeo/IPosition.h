#pragma once

// ACF includes
#include <iser/IObject.h>

namespace imtgeo
{

/**
    Interface for describing the coordinates of an object.
*/
class IPosition: virtual public iser::ISerializable
{
public:

	/**
        Get object latitude .
	*/
	virtual double GetLatitude() const = 0;

	/**
        Set object latitude.
	*/
	virtual void SetLatitude(double lat) = 0;

	/**
        Get object longitude.
	*/
	virtual double GetLongitude() const = 0;

	/**
        Set object longitude.
	*/
	virtual void SetLongitude(double lon) = 0;

};

} // namespace imtgeo


