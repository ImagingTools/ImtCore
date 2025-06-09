#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtdev/IDeviceController.h>
#include <imtdev/IDeviceStateProvider.h>


namespace imtdev
{


class IDeviceFramework: virtual public istd::IPolymorphic
{
public:
	virtual imtbase::ICollectionInfo* GetDeviceTypeList() const = 0;
	virtual imtbase::ICollectionInfo* GetDeviceList() const = 0;
	virtual imtbase::ICollectionInfo* GetExtendedDeviceList() const = 0;
	virtual IDeviceStateProvider* GetDeviceStateProvider() const = 0;
	virtual IDeviceController* GetDeviceController() const= 0;
};


} // namespace imtdev


