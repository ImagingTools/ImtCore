#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iattr/CAttributesManager.h>

// ImtCore includes
#include <imtbase/ISelection.h>
#include <imtbase/TModelUpdateBinder.h>
#include <imtdev/IDeviceController.h>
#include <imtdev/IDeviceConnectionState.h>


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
		I_ASSIGN(m_connectionStateCompPtr, "DeviceConnectionState", "Device connection state", false, "DeviceConnectionState");
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
	void OnConnectionStateChanged(const istd::IChangeable::ChangeSet& changeSet, const IDeviceConnectionState* objectPtr);

private:
	I_REF(imtbase::ISelection, m_selectionCompPtr);
	I_REF(IDeviceController, m_controllerCompPtr);
	I_REF(IDeviceConnectionState, m_connectionStateCompPtr);

	imtbase::TModelUpdateBinder<imtbase::ISelection, CDeviceIdBasedAttributesComp> m_selectionObserver;
	imtbase::TModelUpdateBinder<IDeviceConnectionState, CDeviceIdBasedAttributesComp> m_connectionStateObserver;

	bool m_isConfigurationStoreBlocked = false;
};


} // namespace imtdev


