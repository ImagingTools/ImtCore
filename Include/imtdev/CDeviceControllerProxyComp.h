// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QTimer>

// ACF includes
#include <icomp/CComponentBase.h>
#include <imod/CMultiModelDispatcherBase.h>
#include <ilog/TLoggerCompWrap.h>
#include <imod/TModelWrap.h>
#include <imod/IModel.h>
#include <imod/CModelUpdateBridge.h>

// ImtCore includes
#include <imtbase/CCollectionInfo.h>
#include <imtbase/TModelUpdateBinder.h>
#include <imtdev/IDeviceController.h>
#include <imtdev/IDeviceInstance.h>
#include <imtdev/IDeviceSpecification.h>


namespace imtdev
{


/**
	@brief Proxy controller component aggregating multiple device controllers
	
	CDeviceControllerProxyComp implements the **Proxy Pattern** to aggregate multiple
	IDeviceController instances into a unified interface. It provides seamless access
	to devices from different controllers through a single entry point.
	
	This component is essential for systems with multiple device types managed by
	different controllers, enabling:
	- Unified device enumeration across all controllers
	- Automatic routing of operations to appropriate controller
	- Aggregated device type and instance lists
	- Centralized state tracking
	- Automatic periodic enumeration
	
	@par Key Features:
	- **Multi-Controller Aggregation**: Manages multiple IDeviceController instances
	- **Automatic Enumeration**: Configurable periodic enumeration (default: 5 seconds)
	- **Smart Routing**: Routes device operations to correct underlying controller
	- **Unified Device List**: Combines devices from all controllers
	- **State Aggregation**: Unified state provider across all controllers
	- **Change Tracking**: Multi-model dispatcher for tracking all controller changes
	
	@par Component Structure:
	The component exposes multiple sub-elements:
	- **DeviceTypeList**: Collection of all supported device types from all controllers
	- **DeviceList**: Aggregated list of devices from all controllers (ICollectionInfo)
	- **DeviceStateProvider**: Unified state provider (IDeviceStateProvider)
	- **OverriddenDeviceInfo**: Collection of custom names/descriptions
	
	@par Configuration:
	- **DeviceControllers**: Multi-reference to IDeviceController instances to aggregate
	- **EnableEnumerationControl**: Enable automatic enumeration (default: false)
	- **EnumerationInterval**: Time between enumeration cycles in seconds (default: 5, 0 = continuous)
	
	@par Enumeration Control:
	When enabled, the proxy automatically:
	1. Enumerates all controllers periodically
	2. Updates aggregated device lists
	3. Handles enumeration failures gracefully
	4. Respects the configured interval
	
	@par Usage Pattern:
	@code{.cpp}
	// Obtain proxy controller instance (created via component system)
	CDeviceControllerProxyComp* pProxy = // get from component system
	
	// Obtain controller references (configured via component system)
	IDeviceController* pController1 = // USB device controller reference
	IDeviceController* pController2 = // Network device controller reference
	pProxy->AddDeviceController(pController1);
	pProxy->AddDeviceController(pController2);
	
	// Enable automatic enumeration every 5 seconds
	pProxy->SetEnumerationControl(true);
	pProxy->SetEnumerationInterval(5);
	
	// Use as unified controller
	const ICollectionInfo& allDevices = pProxy->GetDeviceInstanceList();
	DeviceAccessorPtr pAccessor = pProxy->OpenDevice(deviceId, nullptr);
	@endcode
	
	@par Thread Safety:
	- Inherits thread safety from CMultiModelDispatcherBase
	- Uses Qt timer for periodic enumeration (thread-safe signal/slot)
	- Controller operations are delegated with proper synchronization
	
	@see IDeviceController
	@see CDeviceControllerCompBase
	@see imod::CMultiModelDispatcherBase
	@ingroup imtdev
*/
class CDeviceControllerProxyComp:
			public QObject,
			public ilog::CLoggerComponentBase,
			public imod::CMultiModelDispatcherBase,
			virtual public IDeviceController,
			virtual public IDeviceEnumerator::IResultHandler
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDeviceControllerProxyComp);
		I_REGISTER_SUBELEMENT(DeviceTypeList);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceTypeList, imtbase::ICollectionInfo, ExtractDeviceTypeList);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceTypeList, imod::IModel, ExtractDeviceTypeList);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceTypeList, istd::IChangeable, ExtractDeviceTypeList);
		I_REGISTER_SUBELEMENT(DeviceList);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceList, imtbase::ICollectionInfo, ExtractDeviceList);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceList, imod::IModel, ExtractDeviceList);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceList, istd::IChangeable, ExtractDeviceList);
		I_REGISTER_SUBELEMENT(DeviceStateProvider);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceStateProvider, IDeviceStateProvider, ExtractDeviceStateProvider);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceStateProvider, imod::IModel, ExtractDeviceStateProvider);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceStateProvider, istd::IChangeable, ExtractDeviceStateProvider);
		I_REGISTER_SUBELEMENT(OverriddenDeviceInfo);
		I_REGISTER_SUBELEMENT_INTERFACE(OverriddenDeviceInfo, imtbase::ICollectionInfo, ExtractOverriddenDeviceInfo);
		I_REGISTER_SUBELEMENT_INTERFACE(OverriddenDeviceInfo, imod::IModel, ExtractOverriddenDeviceInfo);
		I_REGISTER_SUBELEMENT_INTERFACE(OverriddenDeviceInfo, istd::IChangeable, ExtractOverriddenDeviceInfo);
		I_REGISTER_INTERFACE(IDeviceController);
		I_ASSIGN_MULTI_0(m_deviceControllerCompPtr, "DeviceControllers", "Device controllers", false);
		I_ASSIGN_TO(m_deviceEnumeratorCompPtr, m_deviceControllerCompPtr, true);
		I_ASSIGN(m_enableAttrPtr, "EnableEnumerationControl", "Enable automatic enumeration control if true", true, false);
		I_ASSIGN(m_intervalAttrPtr, "EnumerationInterval", "Interval between starts of successive enumeration cycles (secs)\n0 - Continuous enumeration", true, 5);
	I_END_COMPONENT;

	CDeviceControllerProxyComp();

	// reimplemented (IDeviceController)
	virtual const QByteArrayList& GetSupportedDeviceTypeIds() const override;
	virtual const IDeviceSpecification* GetDeviceStaticInfo(const QByteArray& deviceTypeId) const override;
	virtual const imtbase::ICollectionInfo& GetDeviceInstanceList() const override;
	virtual bool SetDeviceInstanceName(const QByteArray& deviceId, const QString& name) override;
	virtual bool SetDeviceInstanceDescription(const QByteArray& deviceId, const QString& description) override;
	virtual DeviceInstancePtr GetDeviceInstance(const QByteArray& deviceId) const override;
	virtual const IDeviceStateProvider& GetDeviceStateProvider() const override;
	virtual imtdev::DeviceAccessorPtr OpenDevice(
				const QByteArray& deviceId,
				const iprm::IParamsSet* paramsPtr) override;
	virtual bool CloseDevice(const QByteArray& deviceId) override;

	// reimplemented (IDeviceEnumerator)
	virtual IDeviceEnumerator::StartResult StartEnumeration(IDeviceEnumerator::IResultHandler* resultHandlerPtr) override;
	virtual void CancelEnumeration() override;

	// reimplemented (IDeviceEnumerationResultHandler)
	virtual void OnEnumerationResult(
				EnumerationResult result,
				const IDeviceEnumerator* deviceEnumeratorPtr) override;

protected:
	// reimplemented (imod::CMultiModelDispatcherBase)
	virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

protected Q_SLOTS:
	virtual void OnIntervalTimer();

protected:
	virtual void StartEnumeration();
	virtual IDeviceEnumerator* GetCurrentDeviceEnumerator();
	virtual IDeviceEnumerator* GetNextDeviceEnumerator();

	virtual void UpdateDeviceTypeIdList();
	virtual void UpdateDeviceList();
	virtual IDeviceController* FindDeviceController(const QByteArray& deviceId) const;
	virtual QByteArray GetDeviceTypeId(const QByteArray& deviceId) const;

	template <class InteraceType>
	static InteraceType* ExtractDeviceTypeList(CDeviceControllerProxyComp& parent)
	{
		return &parent.m_deviceTypeList;
	}

	template <class InteraceType>
	static InteraceType* ExtractDeviceList(CDeviceControllerProxyComp& parent)
	{
		return &parent.m_deviceList;
	}

	template <class InteraceType>
	static InteraceType* ExtractDeviceStateProvider(CDeviceControllerProxyComp& parent)
	{
		return &parent.m_deviceStateProvider;
	}

	template <class InteraceType>
	static InteraceType* ExtractOverriddenDeviceInfo(CDeviceControllerProxyComp& parent)
	{
		return &parent.m_overriddenDeviceInfo;
	}

protected:
	enum ModelId
	{
		MI_DEVICE_LIST_BASE = 0
	};

	class DeviceStateProvider : virtual public IDeviceStateProvider
	{
	public:
		DeviceStateProvider();
		void SetParent(CDeviceControllerProxyComp& parent);

		// reimplemented (IDeviceStateProvider)
		virtual DeviceState GetDeviceState(const QByteArray& deviceId) const override;

	private:
		CDeviceControllerProxyComp* m_parentPtr;
	};

protected:
	I_MULTIREF(IDeviceController, m_deviceControllerCompPtr);
	I_MULTIREF(IDeviceEnumerator, m_deviceEnumeratorCompPtr);
	I_ATTR(bool, m_enableAttrPtr);
	I_ATTR(int, m_intervalAttrPtr);

	QByteArrayList m_supportedDeviceTypeIds;

	QMap<QByteArray, IDeviceController*> m_deviceControllerMap;

	imod::TModelWrap<imtbase::CCollectionInfo> m_deviceTypeList;
	imod::TModelWrap<imtbase::CCollectionInfo> m_deviceList;
	imod::TModelWrap<DeviceStateProvider> m_deviceStateProvider;
	imod::TModelWrap<imtbase::CCollectionInfo> m_overriddenDeviceInfo;

	imod::CModelUpdateBridge m_deviceStateProviderUpdateBridge;

	int m_enumeratorIndex;
	QTimer m_intervalTimer;
	IDeviceEnumerator::IResultHandler* m_resultHandlerPtr;

private:
	void OnOverriddenDeviceInfoUpdated(const istd::IChangeable::ChangeSet& changeset, const imtbase::ICollectionInfo* objectPtr);

private:
	imtbase::TModelUpdateBinder<imtbase::ICollectionInfo, CDeviceControllerProxyComp> m_overriddenDeviceInfoObserver;
};


} // namespace imtdev


