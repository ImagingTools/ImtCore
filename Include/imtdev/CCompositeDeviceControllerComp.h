#pragma once


// Qt includes
#include <QtCore/QTimer>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <imod/TModelWrap.h>
#include <imod/IModel.h>
#include <imod/CMultiModelDispatcherBase.h>
#include <icomp/CComponentBase.h>
#include <iprm/COptionsManager.h>
#include <icomp/CComponentBase.h>

#include <imtdev/IDeviceInstanceInfo.h>
#include <imtdev/IDeviceStaticInfo.h>
#include <imtdev/IDeviceController.h>


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
		I_REGISTER_SUBELEMENT(DeviceInfoList);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceInfoList, iprm::IOptionsList, ExtractDeviceInfoList);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceInfoList, istd::IChangeable, ExtractDeviceInfoList);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceInfoList, imod::IModel, ExtractDeviceInfoList);
		I_REGISTER_INTERFACE(IDeviceController);
		I_ASSIGN_MULTI_0(m_deviceControllerCompPtr, "DeviceControllers", "Device controllers", false);
		I_ASSIGN_TO(m_deviceEnumeratorCompPtr, m_deviceControllerCompPtr, true);
		I_ASSIGN(m_enumerationIntervalAttrPtr, "DeviceEnumerationInterval", "Interval between starts of successive enumeration cycles (secs).\nNegative value or zero for continuous enumeration", true, 5);
	I_END_COMPONENT;

	CCompositeDeviceControllerComp();

	// reimplemented (IDeviceController)
	virtual QByteArrayList GetSupportedDeviceTypeIds() const override;
	virtual const IDeviceStaticInfo* GetDeviceStaticInfo(const QByteArray& deviceTypeId) const override;
	virtual const iprm::IOptionsList& GetAvailableDeviceList() const override;
	virtual DeviceState GetDeviceState(const QByteArray& deviceId) const override;
	virtual const IDeviceInstanceInfo* GetDeviceInstanceInfo(const QByteArray& deviceId) const override;
	virtual imtdev::DeviceAccessorPtr OpenDevice(const QByteArray& deviceId, const iprm::IParamsSet* paramsPtr) override;
	virtual bool CloseDevice(const QByteArray& deviceId) override;

	// reimplemented (IDeviceEnumerator)
	virtual StartResult StartEnumeration(IResultHandler* resultHandlerPtr) override;
	virtual void CancelEnumeration() override;

	// reimplemented (IDeviceEnumerationResultHandler)
	virtual void OnEnumerationResult(
				EnumerationResult result,
				const IDeviceEnumerator* deviceEnumeratorPtr) override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated();
	virtual void OnComponentDestroyed();

private Q_SLOTS:
	void OnTimer();

private:
	IDeviceEnumerator* GetCurrentDeviceEnumerator();
	IDeviceEnumerator* GetNextDeviceEnumerator();
	bool StartEnumeration();
	void CacnelEnumeration();
	void OnDeviceListChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet);

private:
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

	typedef imod::TModelWrap<iprm::COptionsManager> DeviceInfoList;

	template <class InteraceType>
	static InteraceType* ExtractDeviceInfoList(CCompositeDeviceControllerComp& parent)
	{
		return &parent.m_deviceList;
	}

private:
	I_MULTIREF(IDeviceController, m_deviceControllerCompPtr);
	I_MULTIREF(IDeviceEnumerator, m_deviceEnumeratorCompPtr);
	I_ATTR(int, m_enumerationIntervalAttrPtr);

	QMap<QByteArray, IDeviceController*> m_deviceControllerMap;
	DeviceInfoList m_deviceList;
	DeviceListObserver m_deviceListObserver;

	int m_enumeratorIndex;
	QTimer m_enumerationTimer;
};


} // namespace imtdev


