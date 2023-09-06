#pragma once

// ACF includes
#include <iser/IObject.h>

namespace imtgeo
{
/**
    Interface for describing the coordinates of an object.

*/
class IPosition: virtual public iser::IObject
{
public:

	/**
		Get latitude address.
	*/
	virtual double GetLatitude() const = 0;

	/**
		Set latitude address.
	*/
	virtual void SetLatitude(double lat) = 0;

	/**
		Get longitude address.
	*/
	virtual double GetLongitude() const = 0;

	/**
		Set longitude address.
	*/
	virtual void SetLongitude(double lon) = 0;

};


} // namespace imtgeo


