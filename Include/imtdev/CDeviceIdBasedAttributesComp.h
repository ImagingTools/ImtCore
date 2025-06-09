#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iattr/CAttributesManager.h>

// ImtCore includes
#include <imtbase/ISelection.h>
#include <imtbase/TModelUpdateBinder.h>
#include <imtdev/IDeviceController.h>


namespace imtdev
{


class CDeviceIdBasedAttributesComp:
			public ilog::CLoggerComponentBase,
			public iattr::CAttributesManager
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDeviceIdBasedAttributesComp)
		I_REGISTER_INTERFACE(iattr::IAttributesManager);
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
	DeviceInstanceInfoPtr GetDeviceInstanceInfo(const QByteArray& deviceId) const;
	void UpdateModel();

	void OnSelectionChanged(const istd::IChangeable::ChangeSet& changeSet, const imtbase::ISelection* objectPtr);
	void OnDeviceStateChanged(const istd::IChangeable::ChangeSet& changeSet, const IDeviceStateProvider* objectPtr);

private:
	I_REF(imtbase::ISelection, m_selectionCompPtr);
	I_REF(IDeviceController, m_controllerCompPtr);
	I_REF(IDeviceStateProvider, m_stateProviderCompPtr);

	imtbase::TModelUpdateBinder<imtbase::ISelection, CDeviceIdBasedAttributesComp> m_selectionObserver;
	imtbase::TModelUpdateBinder<IDeviceStateProvider, CDeviceIdBasedAttributesComp> m_stateProviderObserver;

	bool m_isConfigurationStoreBlocked = false;
};


} // namespace imtdev


