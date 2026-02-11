// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#if QT_VERSION < 0x060000
	#include <QtCore/QMutex>
#else
	#include <QtCore/QRecursiveMutex>
#endif

// ACF includes
#include <imod/TModelWrap.h>
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/CCollectionInfo.h>
#include <imtbase/TModelUpdateBinder.h>
#include <imtdev/IDeviceController.h>


namespace imtdev
{


/**
	@brief Base controller component implementing IDeviceController interface
	
	CDeviceControllerCompBase provides a complete, reusable implementation of the
	IDeviceController interface with thread-safe device management, enumeration support,
	and state tracking capabilities.
	
	This class serves as the foundation for device controller implementations throughout
	the system, handling common concerns like device list management, device name/description
	overrides, and state provider functionality.
	
	@par Key Features:
	- **Thread-Safe Device Management**: Mutex-protected device list and opened devices map
	- **Device Enumeration**: Support for asynchronous device discovery
	- **State Tracking**: Built-in device state provider for monitoring device states
	- **Name/Description Overrides**: Customizable device names and descriptions
	- **Auto-Close**: Automatically close devices not found during enumeration
	- **Collection Info**: Provides device list as ICollectionInfo for UI binding
	- **Logging Support**: Integrated with logging framework (CLoggerComponentBase)
	
	@par Component Structure:
	The component exposes multiple sub-elements:
	- **DeviceInfoList**: Collection of discovered devices (ICollectionInfo)
	- **OverriddenDeviceInfo**: Collection of custom names/descriptions
	- **DeviceStateProvider**: State tracking for all devices (IDeviceStateProvider)
	
	@par Configuration:
	- **AutoClose**: Boolean attribute to enable/disable automatic device closing (default: true)
	
	@par Thread Safety:
	Uses QMutex (Qt5) or QRecursiveMutex (Qt6) to protect:
	- Device list updates
	- Opened devices map
	- State queries
	
	@par Usage Pattern:
	Derived classes should:
	1. Implement device-specific enumeration logic
	2. Call UpdateDeviceList() with discovered devices
	3. Override OpenDevice() and CloseDevice() for device-specific operations
	4. Provide device specifications via GetDeviceStaticInfo()
	
	@see IDeviceController
	@see CDeviceControllerProxyComp
	@ingroup imtdev
*/
class CDeviceControllerCompBase:
			public QObject,
			public ilog::CLoggerComponentBase,
			virtual public imtdev::IDeviceController
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CDeviceControllerCompBase)
		I_REGISTER_SUBELEMENT(DeviceInfoList);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceInfoList, imtbase::ICollectionInfo, ExtractDeviceInfoList);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceInfoList, istd::IChangeable, ExtractDeviceInfoList);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceInfoList, imod::IModel, ExtractDeviceInfoList);
		I_REGISTER_SUBELEMENT(OverriddenDeviceInfo);
		I_REGISTER_SUBELEMENT_INTERFACE(OverriddenDeviceInfo, imtbase::ICollectionInfo, ExtractOverriddenDeviceInfo);
		I_REGISTER_SUBELEMENT_INTERFACE(OverriddenDeviceInfo, imod::IModel, ExtractOverriddenDeviceInfo);
		I_REGISTER_SUBELEMENT_INTERFACE(OverriddenDeviceInfo, istd::IChangeable, ExtractOverriddenDeviceInfo);
		I_REGISTER_INTERFACE(IDeviceController);
		I_REGISTER_INTERFACE(IDeviceEnumerator);
		I_ASSIGN(m_isAutoCloseEnabledAttrPtr, "AutoClose", "Automatically close an opened device if it is not found during the next enumeration", true, "true");
	I_END_COMPONENT;

	CDeviceControllerCompBase();

	// reimplemented (IDeviceController)
	virtual const imtbase::ICollectionInfo& GetDeviceInstanceList() const override;
	virtual bool SetDeviceInstanceName(const QByteArray& deviceId, const QString& name) override;
	virtual bool SetDeviceInstanceDescription(const QByteArray& deviceId, const QString& description) override;
	virtual const IDeviceStateProvider& GetDeviceStateProvider() const override;

protected:
	struct EnumeratedDeviceInfo
	{
		QByteArray id;
		QString name;
		QString description;
	};
	typedef QVector<EnumeratedDeviceInfo> EnumeratedDeviceList;

	class DeviceStateProvider : virtual public IDeviceStateProvider
	{
	public:
		DeviceStateProvider();
		void SetParent(CDeviceControllerCompBase& parent);

		// reimplemented (IDeviceStateProvider)
		virtual DeviceState GetDeviceState(const QByteArray& deviceId) const override;

	private:
		CDeviceControllerCompBase* m_parentPtr;
	};

protected:
	// Must be called from the main thread!!!
	void UpdateDeviceList(EnumeratedDeviceList& enumeratedDeviceList);
	void UpdateDeviceList();
	void AutoCloseDisconnectedDevices();

	template <class InteraceType>
	static InteraceType* ExtractDeviceInfoList(CDeviceControllerCompBase& parent)
	{
		return &parent.m_deviceList;
	}

	template <class InteraceType>
	static InteraceType* ExtractOverriddenDeviceInfo(CDeviceControllerCompBase& parent)
	{
		return &parent.m_overriddenDeviceInfo;
	}

	// reimplemened (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

protected:
	I_ATTR(bool, m_isAutoCloseEnabledAttrPtr);

	imod::TModelWrap<imtbase::CCollectionInfo> m_deviceList;
	imod::TModelWrap<imtbase::CCollectionInfo> m_overriddenDeviceInfo;
	imod::TModelWrap<DeviceStateProvider> m_deviceStateProvider;
	QMap<QByteArray, DeviceAccessorPtr> m_openedDevices;

#if QT_VERSION < 0x060000
	mutable QMutex m_deviceListMutex;
	mutable QMutex m_openedDevicesMutex;
#else
	mutable QRecursiveMutex m_deviceListMutex;
	mutable QRecursiveMutex m_openedDevicesMutex;
#endif

private:
	void OnOverriddenDeviceInfoUpdated(const istd::IChangeable::ChangeSet& changeset, const imtbase::ICollectionInfo* objectPtr);

private:
	imtbase::TModelUpdateBinder<imtbase::ICollectionInfo, CDeviceControllerCompBase> m_overriddenDeviceInfoObserver;
};


} // namespace imtdev


