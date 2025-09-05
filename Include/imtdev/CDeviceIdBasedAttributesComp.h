#pragma once


// ACF includes
#include <iattr/CAttributesManager.h>
#include <ilog/TLoggerCompWrap.h>
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtbase/ISelection.h>
#include <imtbase/TModelUpdateBinder.h>
#include <imtdev/IDeviceController.h>


namespace imtdev
{


class CDeviceIdBasedAttributesComp: public ilog::CLoggerComponentBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDeviceIdBasedAttributesComp)
		I_REGISTER_INTERFACE(iattr::IAttributesManager);
		I_REGISTER_SUBELEMENT(StaticAttributes)
		I_REGISTER_SUBELEMENT_INTERFACE(StaticAttributes, iattr::IAttributesProvider, ExtractStaticAttributes);
		I_REGISTER_SUBELEMENT_INTERFACE(StaticAttributes, imod::IModel, ExtractStaticAttributes);
		I_REGISTER_SUBELEMENT_INTERFACE(StaticAttributes, istd::IChangeable, ExtractStaticAttributes);
		I_REGISTER_SUBELEMENT(InstanceAttributes)
		I_REGISTER_SUBELEMENT_INTERFACE(InstanceAttributes, iattr::IAttributesProvider, ExtractInstanceAttributes);
		I_REGISTER_SUBELEMENT_INTERFACE(InstanceAttributes, imod::IModel, ExtractInstanceAttributes);
		I_REGISTER_SUBELEMENT_INTERFACE(InstanceAttributes, istd::IChangeable, ExtractInstanceAttributes);
		I_ASSIGN(m_selectionCompPtr, "DeviceSelection", "ID of the selected device", false, "DeviceSelection");
		I_ASSIGN(m_controllerCompPtr, "DeviceController", "Device controller", false, "DeviceController");
		I_ASSIGN(m_stateProviderCompPtr, "DeviceStateProvider", "Device state provider", false, "DeviceStateProvider");
	I_END_COMPONENT;

	CDeviceIdBasedAttributesComp();

protected:
	// reimplemented (icomp::CComponentBase)
	void OnComponentCreated() override;
	void OnComponentDestroyed() override;

private:
	DeviceInstancePtr GetDeviceInstance(const QByteArray& deviceId) const;
	void UpdateModel();

	void OnSelectionChanged(const istd::IChangeable::ChangeSet& changeSet, const imtbase::ISelection* objectPtr);
	void OnDeviceStateChanged(const istd::IChangeable::ChangeSet& changeSet, const IDeviceStateProvider* objectPtr);
	void OnInstanceAttributesChanged(const istd::IChangeable::ChangeSet& changeSet, const iattr::IAttributesProvider* objectPtr);

	template<class Interface>
	static Interface* ExtractStaticAttributes(CDeviceIdBasedAttributesComp& parent)
	{
		return dynamic_cast<Interface*>(&parent.m_staticAttributes);
	}

	template<class Interface>
	static Interface* ExtractInstanceAttributes(CDeviceIdBasedAttributesComp& parent)
	{
		return dynamic_cast<Interface*>(&parent.m_instanceAttributes);
	}

private:
	I_REF(imtbase::ISelection, m_selectionCompPtr);
	I_REF(IDeviceController, m_controllerCompPtr);
	I_REF(IDeviceStateProvider, m_stateProviderCompPtr);

	imod::TModelWrap<iattr::CAttributesManager> m_staticAttributes;
	imod::TModelWrap<iattr::CAttributesManager> m_instanceAttributes;

	imtbase::TModelUpdateBinder<imtbase::ISelection, CDeviceIdBasedAttributesComp> m_selectionObserver;
	imtbase::TModelUpdateBinder<IDeviceStateProvider, CDeviceIdBasedAttributesComp> m_stateProviderObserver;
	imtbase::TModelUpdateBinder<iattr::IAttributesProvider, CDeviceIdBasedAttributesComp> m_instanceAttributesObserver;

	bool m_isConfigurationStoreBlocked = false;
};


} // namespace imtdev


