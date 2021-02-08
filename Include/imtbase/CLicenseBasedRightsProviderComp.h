#pragma once

// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QMap>

// Acf includes
#include <iauth/IRightsProvider.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/ILicenseInfoProvider.h>


namespace imtbase
{


class CLicenseBasedRightsProviderComp:
			public icomp::CComponentBase,
			public iauth::IRightsProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CLicenseBasedRightsProviderComp);
		I_REGISTER_INTERFACE(IRightsProvider);
		I_ASSIGN_MULTI_0(m_rightIdAttrPtr, "RightIds", "List of rights id", false);
		I_ASSIGN_MULTI_0(m_licenseIdAttrPtr, "LicenseIds", "List of the corresponds license id", false);
		I_ASSIGN(m_defaultRightAttrPtr, "DefaultRight", "Default rights enabled", false, false);
		I_ASSIGN(m_licenseInfoProviderCompPtr, "LicenseInfoProvider", "License info provider", false, "LicenseInfoProvider");
		I_ASSIGN(m_slaveProviderCompPtr, "SlaveProvider", "Slave rights provider", false, "SlaveProvider");
	I_END_COMPONENT;

	// reimplemented (iauth::IRightsProvider)
	virtual bool HasRight(
				const QByteArray& operationId,
				bool beQuiet = false) const;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_MULTIATTR(QByteArray, m_rightIdAttrPtr);
	I_MULTIATTR(QByteArray, m_licenseIdAttrPtr);
	I_ATTR(bool, m_defaultRightAttrPtr);
	I_REF(ILicenseInfoProvider, m_licenseInfoProviderCompPtr);
	I_REF(IRightsProvider, m_slaveProviderCompPtr);

	QMap<QByteArray, QByteArray> m_rightsMap;
};


} // namespace imtbase


