#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QMap>

// ACF includes
#include <iauth/IRightsProvider.h>
#include <imod/IModel.h>
#include <icomp/CComponentBase.h>
#include <imod/CModelUpdateBridge.h>

// ImtCore includes
#include <imtlic/ILicenseInstanceProvider.h>


namespace imtlic
{


/**
	Realization of the acess right logic based on the license management.
	\ingroup LicenseManagement
*/
class CLicenseBasedRightsProviderComp:
			public icomp::CComponentBase,
			virtual public iauth::IRightsProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CLicenseBasedRightsProviderComp);
		I_REGISTER_INTERFACE(IRightsProvider);
		I_ASSIGN_MULTI_0(m_rightIdAttrPtr, "RightIds", "List of rights IDs", false);
		I_ASSIGN_MULTI_0(m_licenseIdAttrPtr, "LicenseIds", "List of the corresponds license IDs", false);
		I_ASSIGN(m_licenseInfoProviderCompPtr, "LicenseInfoProvider", "License info provider", false, "LicenseInfoProvider");
		I_ASSIGN_TO(m_licenseInfoProviderModelCompPtr, m_licenseInfoProviderCompPtr, false);
		I_ASSIGN(m_slaveProviderCompPtr, "SlaveProvider", "Slave rights provider", false, "SlaveProvider");
		I_ASSIGN_TO(m_slaveProviderModelCompPtr, m_slaveProviderCompPtr, false);
	I_END_COMPONENT;

	CLicenseBasedRightsProviderComp();

	// reimplemented (iauth::IRightsProvider)
	virtual bool HasRight(
				const QByteArray& operationId,
				bool beQuiet = false) const;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	I_MULTIATTR(QByteArray, m_rightIdAttrPtr);
	I_MULTIATTR(QByteArray, m_licenseIdAttrPtr);
	I_REF(imtlic::ILicenseInstanceProvider, m_licenseInfoProviderCompPtr);
	I_REF(imod::IModel, m_licenseInfoProviderModelCompPtr);
	I_REF(iauth::IRightsProvider, m_slaveProviderCompPtr);
	I_REF(imod::IModel, m_slaveProviderModelCompPtr);

	QMap<QByteArray, QByteArray> m_rightsMap;
	imod::CModelUpdateBridge m_updateBridge;
};


} // namespace imtlic


