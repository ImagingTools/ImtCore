// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtlic/IPartProductionInfo.h>

namespace imtbase
{
	class IObjectCollection;
}


namespace imtlic
{


class IProductInfo;


/**
	Interface for a licensed hardware product instance.
	
	This interface represents a hardware product instance that is linked to
	the licensing system through a software product. It extends IPartProductionInfo
	with hardware-specific licensing information including serial number and
	software linkage.
	
	\sa IHardwareInstanceInfo, IPartProductionInfo, IProductInstanceInfo
	\ingroup LicenseManagement
*/
class ILicensedHardwareInstanceInfo: virtual public IPartProductionInfo
{
public:
	enum MetaInfoTypes
	{
		/**
			Serial number of the hardware product.
		*/
		MIT_SERIAL_NUMBER = idoc::IDocumentMetaInfo::MIT_USER + 2000,

	};

	/**
		Get the serial number of this hardware product instance.
		\return Hardware serial number
	*/
	virtual QByteArray GetSerialNumber() const = 0;

	/**
		Set the serial number of this hardware product instance.
		\param serialNumber Hardware serial number
	*/
	virtual void SetSerialNumber(const QByteArray& serialNumber) = 0;

	/**
		Get the ID of the linked software product for licensing.
		Hardware products are licensed through associated software products.
		\return Software product ID
	*/
	virtual QByteArray GetSoftwareId() const = 0;

	/**
		Set the ID of the linked software product for licensing.
		\param softwareId Software product identifier
	*/
	virtual void SetSoftwareId(const QByteArray& softwareId) = 0;
};


} // namespace imtlic


