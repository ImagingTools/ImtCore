#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/ISelection.h>
#include <imtbase/TModelUpdateBinder.h>
#include <imtdev/IDeviceConfigurationManager.h>
#include <imtdev/IDeviceController.h>
#include <imtdev/IDeviceConnectionState.h>


namespace imtdev
{


class CDeviceIdBasedConfigurationComp:
			public ilog::CLoggerComponentBase,
			public iprm::CParamsSet
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDeviceIdBasedConfigurationComp)
		I_REGISTER_INTERFACE(iprm::IParamsSet);
		I_ASSIGN(m_deviceSelectionCompPtr, "DeviceSelection", "ID of the selected device", false, "DeviceSelection");
		I_ASSIGN(m_deviceControllerCompPtr, "DeviceController", "Device controller", false, "DeviceController");
		I_ASSIGN(m_configurationManagerCompPtr, "DeviceConfigurationManager", "Configuration manager of the devices", false, "DeviceConfigurationManager");
		I_ASSIGN(m_connectionStateCompPtr, "DeviceConnectionState", "Device connection state", false, "DeviceConnectionState");
	I_END_COMPONENT;

	CDeviceIdBasedConfigurationComp();

protected:
	// reimplemented (icomp::CComponentBase)
	void OnComponentCreated() override;
	void OnComponentDestroyed() override;

private:
	DeviceInstanceInfoPtr GetDeviceInstanceInfo(const QByteArray& deviceId) const;
	void UpdateModel();

	void OnSelectionChanged(const istd::IChangeable::ChangeSet& changeSet, const imtbase::ISelection* objectPtr);
	void OnConfigurationChanged(const istd::IChangeable::ChangeSet& changeSet, const iprm::IParamsSet* objectPtr);
	void OnConnectionStateChanged(const istd::IChangeable::ChangeSet& changeSet, const IDeviceConnectionState* objectPtr);

private:
	class FlagLocker
	{
	public:
		FlagLocker()
			:m_flagPtr(nullptr),
			m_controlAllowed(false)
		{
		};

		FlagLocker(bool &flag)
			:m_flagPtr(&flag)
		{
			m_controlAllowed = !flag;

			flag = true;
		}

		~FlagLocker()
		{
			if (m_controlAllowed){
				*m_flagPtr = false;
			}
		}

	private:
		bool* m_flagPtr;
		bool m_controlAllowed;
	};

private:
	I_REF(imtbase::ISelection, m_deviceSelectionCompPtr);
	I_REF(imtdev::IDeviceController, m_deviceControllerCompPtr);
	I_REF(imtdev::IDeviceConfigurationManager, m_configurationManagerCompPtr);
	I_REF(IDeviceConnectionState, m_connectionStateCompPtr);

	imtbase::TModelUpdateBinder<imtbase::ISelection, CDeviceIdBasedConfigurationComp> m_selectionObserver;
	imtbase::TModelUpdateBinder<iprm::IParamsSet, CDeviceIdBasedConfigurationComp> m_configurationObserver;
	imtbase::TModelUpdateBinder<IDeviceConnectionState, CDeviceIdBasedConfigurationComp> m_connectionStateObserver;

	bool m_isConfigurationStoreBlocked = false;
};


} // namespace imtdev


