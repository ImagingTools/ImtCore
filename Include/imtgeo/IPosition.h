// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/ISerializable.h>


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
	virtual void SetLatitude(const double& lat) = 0;
	/**
		Get object longitude.
	*/
	virtual double GetLongitude() const = 0;

	/**
		Set object longitude.
	*/
	virtual void SetLongitude(const double& lon) = 0;
	/**
		Get object zoom level.
	*/
	virtual int GetZoomLevel() const = 0;

	/**
		Set object zoom level.
	*/
	virtual void SetZoomLevel(const double& zoom) = 0;
};


} // namespace imtgeo

