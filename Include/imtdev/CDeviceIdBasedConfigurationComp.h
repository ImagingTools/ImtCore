#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/ISelection.h>
#include <imtbase/TModelUpdateBinder.h>
#include <imtdev/IDeviceConfigurationManager.h>
#include <imtdev/IDeviceController.h>


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
		I_ASSIGN(m_controllerCompPtr, "DeviceController", "Device controller", false, "DeviceController");
		I_ASSIGN(m_configurationManagerCompPtr, "DeviceConfigurationManager", "Configuration manager of the devices", false, "DeviceConfigurationManager");
		I_ASSIGN(m_stateProviderCompPtr, "DeviceStateProvider", "Device state provider", false, "DeviceStateProvider");
	I_END_COMPONENT;

	CDeviceIdBasedConfigurationComp();

protected:
	// reimplemented (icomp::CComponentBase)
	void OnComponentCreated() override;
	void OnComponentDestroyed() override;

private:
	DeviceInstancePtr GetDeviceInstance(const QByteArray& deviceId) const;
	void UpdateModel();

	void OnSelectionChanged(const istd::IChangeable::ChangeSet& changeSet, const imtbase::ISelection* objectPtr);
	void OnConfigurationChanged(const istd::IChangeable::ChangeSet& changeSet, const iprm::IParamsSet* objectPtr);
	void OnDeviceStateChanged(const istd::IChangeable::ChangeSet& changeSet, const IDeviceStateProvider* objectPtr);

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
	I_REF(imtdev::IDeviceController, m_controllerCompPtr);
	I_REF(imtdev::IDeviceConfigurationManager, m_configurationManagerCompPtr);
	I_REF(IDeviceStateProvider, m_stateProviderCompPtr);

	imtbase::TModelUpdateBinder<imtbase::ISelection, CDeviceIdBasedConfigurationComp> m_selectionObserver;
	imtbase::TModelUpdateBinder<iprm::IParamsSet, CDeviceIdBasedConfigurationComp> m_configurationObserver;
	imtbase::TModelUpdateBinder<IDeviceStateProvider, CDeviceIdBasedConfigurationComp> m_stateProviderObserver;

	bool m_isConfigurationStoreBlocked = false;
};


} // namespace imtdev


