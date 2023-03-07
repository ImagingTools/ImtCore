#pragma once


// ACF includes
#include <iser/IVersionInfo.h>
#include <idoc/IDocumentMetaInfo.h>


namespace imtdev
{


class IDeviceInfo: virtual public iser::IVersionInfo
{
public:
	enum DeviceMetaInfo
	{
		DMI_DEVICE_TYPE_NAME = idoc::IDocumentMetaInfo::MIT_USER,
		DMI_OEM_NAME,
		DMI_DEVICE_SERIAL_NUMBER,
		DMI_HARDWARE_REVISION
	};

	/**
		Get device meta informations.
	*/
	virtual const idoc::IDocumentMetaInfo* GetDeviceMetaInfo() const = 0;
};


} // namespace imtdev


