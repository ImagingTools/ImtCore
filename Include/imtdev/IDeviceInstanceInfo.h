#pragma once


// ACF includes
#include <iattr/IAttributesProvider.h>
#include <iser/IVersionInfo.h>


namespace imtdev
{


class IDeviceStaticInfo;


class IDeviceInstanceInfo: virtual public istd::IChangeable
{
public:
	enum IdTypes
	{
		IT_SERIAL,
		IT_MAC,
		IT_IPv4,
		IT_IPv6
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
	virtual const IDeviceStaticInfo& GetDeviceStaticInfo() const = 0;

	/**
		Get the device ID for the given IdType
	*/
	virtual QByteArray GetDeviceIdentifier(int idType) const = 0;

	/**
		Get device instance version
	*/
	virtual const iser::IVersionInfo& GetVersion() const = 0;

	/**
		Get device instance attributes provider
	*/
	virtual const iattr::IAttributesProvider* GetDeviceAttributes() const = 0;
};


typedef QSharedPointer<IDeviceInstanceInfo> DeviceInstanceInfoPtr;


} // namespace imtdev


