// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <imtlic/ILicenseDefinition.h>



namespace imtlic
{


/**
	Interface for an activated license instance.
	
	A License Instance represents an activated license based on a License Definition.
	When a customer purchases a license, it is activated as a License Instance on their
	Product Instance. The License Instance extends the License Definition with instance-
	specific information such as activation time and expiration date.
	
	Key characteristics:
	- Inherits all properties from ILicenseDefinition (license ID, features, dependencies)
	- Adds temporal information (expiration date)
	- Represents an actual, activated license rather than just a license type
	
	\sa ILicenseDefinition, IProductInstanceInfo
	\ingroup LicenseManagement
*/
class ILicenseInstance: virtual public ILicenseDefinition
{
public:
	/**
		Get the expiration time for this license instance.
		\return Expiration date/time, or an invalid QDateTime if the license has no time limitation (perpetual license)
	*/
	virtual QDateTime GetExpiration() const = 0;

	/**
		Set the expiration time for this license instance.
		\param expirationTime Date/time when the license expires, or invalid QDateTime for perpetual license
	*/
	virtual void SetExpiration(const QDateTime& expirationTime) = 0;
};


} // namespace imtlic


