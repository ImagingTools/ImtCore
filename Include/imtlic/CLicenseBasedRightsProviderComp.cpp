#include <imtlic/CLicenseBasedRightsProviderComp.h>


// ImtCore includes
#include <imtlic/ILicenseInstance.h>


namespace imtlic
{


// public methods

CLicenseBasedRightsProviderComp::CLicenseBasedRightsProviderComp()
	:m_updateBridge(this)
{
}


// reimplemented (iauth::IRightsProvider)

bool CLicenseBasedRightsProviderComp::HasRight(
			const QByteArray& operationId,
			bool beQuiet) const
{
	if (m_licenseInfoProviderCompPtr.IsValid() && m_rightsMap.contains(operationId)){
		const ILicenseInstance* licenseInstancePtr = m_licenseInfoProviderCompPtr->GetLicenseInstance(m_rightsMap[operationId]);
		if (licenseInstancePtr != nullptr){
			return licenseInstancePtr->GetExpiration() < QDateTime::currentDateTime();
		}
	}

	if (m_slaveProviderCompPtr.IsValid()){
		return m_slaveProviderCompPtr->HasRight(operationId, beQuiet);
	}

	return false;
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

	if (m_licenseInfoProviderCompPtr.IsValid() && m_licenseInfoProviderModelCompPtr.IsValid()){
		m_licenseInfoProviderModelCompPtr->AttachObserver(&m_updateBridge);
	}

	if (m_slaveProviderCompPtr.IsValid() && m_slaveProviderModelCompPtr.IsValid()){
		m_slaveProviderModelCompPtr->AttachObserver(&m_updateBridge);
	}
}


void CLicenseBasedRightsProviderComp::OnComponentDestroyed()
{
	m_updateBridge.EnsureModelsDetached();

	BaseClass::OnComponentDestroyed();
}


} // namespace imtlic


