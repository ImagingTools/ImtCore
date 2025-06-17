#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <imod/TModelWrap.h>
#include <imod/CMultiModelDispatcherBase.h>
#include <imod/CModelUpdateBridge.h>

// ImtCore includes
#include <imtbase/CCollectionInfo.h>
#include <imtdev/IDeviceFramework.h>


namespace imtdev
{


class CDeviceFrameworkProxyComp : public icomp::CComponentBase
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDeviceFrameworkProxyComp);
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
		I_REGISTER_SUBELEMENT(DeviceStateProvider);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceStateProvider, IDeviceStateProvider, ExtractDeviceStateProvider);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceStateProvider, istd::IChangeable, ExtractDeviceStateProvider);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceStateProvider, imod::IModel, ExtractDeviceStateProvider);
		I_REGISTER_SUBELEMENT(DeviceController);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceController, IDeviceController, ExtractDeviceController);
		I_ASSIGN(m_deviceFrameworkCompPtr, "DeviceFramework", "Device framework", false, "DeviceFramework");
	I_END_COMPONENT;

	CDeviceFrameworkProxyComp();

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	enum ModelId
	{
		MI_TYPE_LIST = 0,
		MI_DEVICE_LIST,
		MI_EXTENDED_DEVICE_LIST,
	};

	class Observer : public imod::CMultiModelDispatcherBase
	{
	public:
		Observer(CDeviceFrameworkProxyComp& parent);

	protected:
		virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

	private:
		CDeviceFrameworkProxyComp& m_parent;
	};

	class DeviceStateProvider : virtual public IDeviceStateProvider
	{
	public:
		void SetParent(CDeviceFrameworkProxyComp* parentPtr);

		// reimplemented (IDeviceStateProvider)
		virtual DeviceState GetDeviceState(const QByteArray& deviceId) const override;

	private:
		CDeviceFrameworkProxyComp* m_parentPtr = nullptr;
	};

	class DeviceController : virtual public IDeviceController
	{
	public:
		DeviceController(CDeviceFrameworkProxyComp& parent);

		// reimplemented (IDeviceController)
		virtual const QByteArrayList& GetSupportedDeviceTypeIds() const override;
		virtual const IDeviceStaticInfo* GetDeviceStaticInfo(const QByteArray& deviceTypeId) const override;
		virtual const imtbase::ICollectionInfo& GetDeviceInstanceList() const override;
		virtual DeviceInstanceInfoPtr GetDeviceInstanceInfo(const QByteArray& deviceId) const override;
		virtual const IDeviceStateProvider& GetDeviceStateProvider() const override;
		virtual DeviceAccessorPtr OpenDevice(
			const QByteArray& deviceId,
			const iprm::IParamsSet* paramsPtr) override;
		virtual bool CloseDevice(const QByteArray& deviceId) override;

		// reimplemented (IDeviceEnumerator)
		virtual StartResult StartEnumeration(IResultHandler* resultHandlerPtr) override;
		virtual void CancelEnumeration() override;

	private:
		IDeviceController* GetDeviceController() const;

	private:
		CDeviceFrameworkProxyComp& m_parent;
	};

private:
	template<class Interface>
	static Interface* ExtractDeviceTypeList(CDeviceFrameworkProxyComp& parent)
	{
		return &parent.m_deviceTypeList;
	}

	template<class Interface>
	static Interface* ExtractDeviceList(CDeviceFrameworkProxyComp& parent)
	{
		return &parent.m_deviceList;
	}

	template<class Interface>
	static Interface* ExtractExtendedDeviceList(CDeviceFrameworkProxyComp& parent)
	{
		return &parent.m_extendedDeviceList;
	}

	template<class Interface>
	static Interface* ExtractDeviceStateProvider(CDeviceFrameworkProxyComp& parent)
	{
		return &parent.m_deviceStateProvider;
	}

	template<class Interface>
	static Interface* ExtractDeviceController(CDeviceFrameworkProxyComp& parent)
	{
		return &parent.m_deviceController;
	}

private:
	I_REF(IDeviceFramework, m_deviceFrameworkCompPtr);

	imod::TModelWrap<imtbase::CCollectionInfo> m_deviceTypeList;
	imod::TModelWrap<imtbase::CCollectionInfo> m_deviceList;
	imod::TModelWrap<imtbase::CCollectionInfo> m_extendedDeviceList;
	imod::TModelWrap<DeviceStateProvider> m_deviceStateProvider;
	DeviceController m_deviceController;

	Observer m_observer;
	imod::CModelUpdateBridge m_updateBridge;
};


} // namespace imtdev


