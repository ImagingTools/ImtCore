#include <imtbase/CLicenseBasedRightsProviderComp.h>


// ImtCore includes
#include <imtbase/ILicenseInfo.h>


namespace imtbase
{


// reimplemented (iauth::IRightsProvider)

bool CLicenseBasedRightsProviderComp::HasRight(
			const QByteArray& operationId,
			bool beQuiet) const
{
	if (m_licenseInfoProviderCompPtr.IsValid() && m_rightsMap.contains(operationId)){
		const ILicenseInfo* licenseInfoPtr = m_licenseInfoProviderCompPtr->GetLicenseInfo(m_rightsMap[operationId]);
		if (licenseInfoPtr != nullptr){
			return licenseInfoPtr->GetExpiration() < QDateTime::currentDateTime();
		}
	}

	if (m_slaveProviderCompPtr.IsValid()){
		return m_slaveProviderCompPtr->HasRight(operationId, beQuiet);
	}

	return m_defaultRightAttrPtr.IsValid() && *m_defaultRightAttrPtr;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CLicenseBasedRightsProviderComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	int count = qMin(m_rightIdAttrPtr.GetCount(), m_licenseIdAttrPtr.GetCount());
	for (int i = 0; i < count; i++){
		Q_ASSERT(!m_rightsMap.contains(m_rightIdAttrPtr[i]));
		m_rightsMap[m_rightIdAttrPtr[i]] = m_licenseIdAttrPtr[i];
	}
}


} // namespace imtbase


