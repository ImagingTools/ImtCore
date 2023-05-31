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
#include <imtdev/IDeviceState.h>


namespace imtdev
{


class CCompositeDeviceControllerComp:
			public QObject,
			public ilog::CLoggerComponentBase,
			virtual public IDeviceController,
			virtual public IDeviceEnumerator::IResultHandler
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CCompositeDeviceControllerComp);
		I_REGISTER_SUBELEMENT(DeviceList);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceList, imtbase::ICollectionInfo, ExtractDeviceList);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceList, istd::IChangeable, ExtractDeviceList);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceList, imod::IModel, ExtractDeviceList);
		I_REGISTER_SUBELEMENT(DeviceTypeList);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceTypeList, imtbase::ICollectionInfo, ExtractDeviceTypeList);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceTypeList, IDeviceState, ExtractDeviceTypeList);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceTypeList, istd::IChangeable, ExtractDeviceTypeList);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceTypeList, imod::IModel, ExtractDeviceTypeList);
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
	virtual imtdev::DeviceAccessorPtr OpenDevice(const QByteArray& deviceTypeId, const QByteArray& deviceId, const iprm::IParamsSet* paramsPtr) override;
	virtual bool CloseDevice(const QByteArray& deviceId) override;

	// reimplemented (IDeviceEnumerator)
	virtual IDeviceEnumerator::StartResult StartEnumeration(IDeviceEnumerator::IResultHandler* resultHandlerPtr) override;
	virtual void CancelEnumeration() override;

	// reimplemented (IDeviceEnumerationResultHandler)
	virtual void OnEnumerationResult(
				EnumerationResult result,
				const IDeviceEnumerator* deviceEnumeratorPtr) override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated();
	virtual void OnComponentDestroyed();

protected Q_SLOTS:
	virtual void OnIntervalTimer();

protected:
	virtual void StartEnumeration();
	virtual IDeviceEnumerator* GetCurrentDeviceEnumerator();
	virtual IDeviceEnumerator* GetNextDeviceEnumerator();
	virtual void OnDeviceListChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet);

protected:
	class DeviceListObserver: public imod::CMultiModelDispatcherBase
	{
	public:
		typedef imod::CMultiModelDispatcherBase BaseClass;

		DeviceListObserver(CCompositeDeviceControllerComp& parent);

		// reimplemented (imod::CMultiModelDispatcherBase)
		void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet);

	private:
		CCompositeDeviceControllerComp& m_parent;
	};

	class DeviceTypeList: public imtbase::CCollectionInfo, virtual public IDeviceState
	{
	public:
		DeviceTypeList();
		void SetParent(CCompositeDeviceControllerComp& parent);

		// reimplemented (IDeviceState)
		virtual bool IsDeviceConnected(const QByteArray& deviceTypeId) override;

	private:
		CCompositeDeviceControllerComp* m_parentPtr;
	};

	template <class InteraceType>
	static InteraceType* ExtractDeviceList(CCompositeDeviceControllerComp& parent)
	{
		return &parent.m_deviceList;
	}

	template <class InteraceType>
	static InteraceType* ExtractDeviceTypeList(CCompositeDeviceControllerComp& parent)
	{
		return &parent.m_deviceTypeList;
	}

protected:
	I_MULTIREF(IDeviceController, m_deviceControllerCompPtr);
	I_MULTIREF(IDeviceEnumerator, m_deviceEnumeratorCompPtr);
	I_ATTR(bool, m_enableAttrPtr);
	I_ATTR(int, m_intervalAttrPtr);

	QByteArrayList m_supportedDeviceTypeIds;

	QMap<QByteArray, IDeviceController*> m_deviceControllerMap;

	imod::TModelWrap<imtbase::CCollectionInfo> m_deviceList;
	imod::TModelWrap<DeviceTypeList> m_deviceTypeList;

	DeviceListObserver m_deviceListObserver;

	int m_enumeratorIndex;
	QTimer m_intervalTimer;
	IDeviceEnumerator::IResultHandler* m_resultHandlerPtr;
};


} // namespace imtdev


