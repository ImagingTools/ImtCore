#pragma once


// ACF includes
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
		IT_MAC
	};

	enum VersionIds
	{
		VI_FIRMWARE_REVISION = iser::IVersionInfo::UserVersionId,
		VI_HARDWARE_REVISION
	};

	/**
		Get device static info associated with device instance
	*/
	virtual const IDeviceStaticInfo& GetDeviceStaticInfo() const = 0;

	/**
		Get the device id for the given IdType
	*/
	virtual QByteArray GetDeviceIdentifier(int idType) const = 0;

	/**
		Get device instance version
	*/
	virtual const iser::IVersionInfo& GetVersion() const = 0;
};


} // namespace imtdev


