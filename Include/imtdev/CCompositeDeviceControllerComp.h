#pragma once


// Qt includes
#include <QtCore/QTimer>

// ACF includes
#include <icomp/CComponentBase.h>
#include <imod/CMultiModelDispatcherBase.h>
#include <ilog/TLoggerCompWrap.h>
#include <imod/TModelWrap.h>
#include <imod/IModel.h>

// ImtCore includes
#include <imtbase/CCollectionInfo.h>
#include <imtdev/IDeviceController.h>
#include <imtdev/IDeviceInstanceInfo.h>
#include <imtdev/IDeviceStaticInfo.h>
#include <imtdev/IDeviceConnectionState.h>


namespace imtdev
{


class CCompositeDeviceControllerComp:
			public QObject,
			public ilog::CLoggerComponentBase,
			public imod::CMultiModelDispatcherBase,
			virtual public IDeviceController,
			virtual public IDeviceEnumerator::IResultHandler
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CCompositeDeviceControllerComp);
		I_REGISTER_SUBELEMENT(DeviceTypeList);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceTypeList, imtbase::ICollectionInfo, ExtractDeviceTypeList);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceTypeList, istd::IChangeable, ExtractDeviceTypeList);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceTypeList, imod::IModel, ExtractDeviceTypeList);
		I_REGISTER_SUBELEMENT(DeviceList);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceList, imtbase::ICollectionInfo, ExtractDeviceList);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceList, istd::IChangeable, ExtractDeviceList);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceList, imod::IModel, ExtractDeviceList);
		I_REGISTER_SUBELEMENT(ExtendedDeviceList);
		I_REGISTER_SUBELEMENT_INTERFACE(ExtendedDeviceList, imtbase::ICollectionInfo, ExtractExtendedDeviceList);
		I_REGISTER_SUBELEMENT_INTERFACE(ExtendedDeviceList, istd::IChangeable, ExtractExtendedDeviceList);
		I_REGISTER_SUBELEMENT_INTERFACE(ExtendedDeviceList, imod::IModel, ExtractExtendedDeviceList);
		I_REGISTER_SUBELEMENT(DeviceConnectionState);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceConnectionState, IDeviceConnectionState, ExtractDeviceConnectionState);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceConnectionState, istd::IChangeable, ExtractDeviceConnectionState);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceConnectionState, imod::IModel, ExtractDeviceConnectionState);
		I_REGISTER_INTERFACE(IDeviceController);
		I_ASSIGN_MULTI_0(m_deviceControllerCompPtr, "DeviceControllers", "Device controllers", false);
		I_ASSIGN_TO(m_deviceEnumeratorCompPtr, m_deviceControllerCompPtr, true);
		I_ASSIGN(m_enableAttrPtr, "EnableEnumerationControl", "Enable automatic enumeration control if true", true, false);
		I_ASSIGN(m_intervalAttrPtr, "EnumerationInterval", "Interval between starts of successive enumeration cycles (secs)\n0 - Continuous enumeration", true, 5);
	I_END_COMPONENT;

	CCompositeDeviceControllerComp();

	// reimplemented (IDeviceController)
	virtual const QByteArrayList& GetSupportedDeviceTypeIds() const override;
	virtual const IDeviceStaticInfo* GetDeviceStaticInfo(const QByteArray& deviceTypeId) const override;
	virtual const imtbase::ICollectionInfo& GetAvailableDeviceList() const override;
	virtual DeviceState GetDeviceState(const QByteArray& deviceId) const override;
	virtual DeviceInstanceInfoPtr GetDeviceInstanceInfo(const QByteArray& deviceId) const override;
	virtual imtdev::DeviceAccessorPtr OpenDevice(
				const QByteArray& deviceTypeId,
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
	virtual void OnComponentCreated();
	virtual void OnComponentDestroyed();

protected Q_SLOTS:
	virtual void OnIntervalTimer();

protected:
	virtual void StartEnumeration();
	virtual IDeviceEnumerator* GetCurrentDeviceEnumerator();
	virtual IDeviceEnumerator* GetNextDeviceEnumerator();

	virtual void UpdateDeviceTypeIdList();
	virtual void UpdateDeviceList();
	virtual void UpdateExtendedDeviceList();
	virtual QByteArray GetDeviceTypeId(const QByteArray& deviceId) const;

	template <class InteraceType>
	static InteraceType* ExtractDeviceTypeList(CCompositeDeviceControllerComp& parent)
	{
		return &parent.m_deviceTypeList;
	}

	template <class InteraceType>
	static InteraceType* ExtractDeviceList(CCompositeDeviceControllerComp& parent)
	{
		return &parent.m_deviceList;
	}

	template <class InteraceType>
	static InteraceType* ExtractExtendedDeviceList(CCompositeDeviceControllerComp& parent)
	{
		return &parent.m_extendedDeviceList;
	}

	template <class InteraceType>
	static InteraceType* ExtractDeviceConnectionState(CCompositeDeviceControllerComp& parent)
	{
		return &parent.m_deviceConnectionState;
	}

protected:
	enum ModelId
	{
		MI_DEVICE_LIST_BASE = 0
	};

	class DeviceConnectionState: virtual public IDeviceConnectionState
	{
	public:
		DeviceConnectionState();
		void SetParent(CCompositeDeviceControllerComp& parent);

		// reimplemented (IDeviceState)
		virtual bool IsDeviceConnected(const QByteArray& deviceId) override;

	private:
		CCompositeDeviceControllerComp* m_parentPtr;
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
	imod::TModelWrap<imtbase::CCollectionInfo> m_extendedDeviceList;
	imod::TModelWrap<DeviceConnectionState> m_deviceConnectionState;

	int m_enumeratorIndex;
	QTimer m_intervalTimer;
	IDeviceEnumerator::IResultHandler* m_resultHandlerPtr;
};


} // namespace imtdev


