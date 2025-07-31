#pragma once


// ACF includes
#include <istd/IChangeable.h>
#include <imtdev/IUsbDeviceIdProvider.h>


namespace imtdev
{


class IUsbDeviceConnectionMonitor : virtual public istd::IChangeable
{
public:
	enum ChangeFlags
	{
		CF_DEVICE_ATTACHED = 0xCEEA12A0,
		CF_DEVICE_DETACHED = 0xCEEA1BA1
	};

	inline static const QByteArray CN_DEVICE_ATTACHED = QByteArrayLiteral("imtdev::IUsbDeviceConnectionMonitor::CN_DEVICE_ATTACHED");
	inline static const QByteArray CN_DEVICE_DETACHED = QByteArrayLiteral("imtdev::IUsbDeviceConnectionMonitor::CN_DEVICE_DETACHED");


	virtual const IUsbDeviceIdProvider::UsbDeviceIdList& GetUsbAttachedDevicesList() const = 0;
};


} // namespace imtdev
