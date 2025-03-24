#pragma once


// ACF includes
#include <iattr/IAttributesProvider.h>
#include <iser/IVersionInfo.h>

// ImtCore includes
#include <imtdev/IDeviceStaticInfo.h>


namespace imtdev
{


/**
	Interface for describing information about a device instance
*/
class IDeviceInstanceInfo: virtual public istd::IChangeable
{
public:
	enum IdentifierTypes
	{
		IT_SERIAL
	};

	enum VersionIds
	{
		VI_FIRMWARE_VERSION_MAJOR = iser::IVersionInfo::UserVersionId,
		VI_FIRMWARE_VERSION_MINOR,
		VI_FIRMWARE_VERSION_PATCH,
		VI_HARDWARE_VERSION_MAJOR,
		VI_HARDWARE_VERSION_MINOR,
		VI_HARDWARE_VERSION_PATCH,
	};

	/**
		Get device static info associated with device instance
	*/
	virtual const IDeviceStaticInfo& GetStaticInfo() const = 0;

	/**
		Get the device ID for the given IdType
	*/
	virtual QByteArray GetIdentifier(int idType) const = 0;

	/**
		Get device instance version
	*/
	virtual const iser::IVersionInfo& GetVersion() const = 0;

	/**
		Get device instance attributes provider
	*/
	virtual const iattr::IAttributesProvider* GetAttributes() const = 0;
};


typedef std::shared_ptr<IDeviceInstanceInfo> DeviceInstanceInfoPtr;


} // namespace imtdev


