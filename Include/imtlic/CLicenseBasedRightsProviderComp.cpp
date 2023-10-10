#include <imtlic/CLicenseBasedRightsProviderComp.h>


// ImtCore includes
#include <imtbase/ICollectionInfo.h>
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
	if (m_licenseInfoProviderCompPtr.IsValid()){
		const imtbase::ICollectionInfo& licenseList = m_licenseInfoProviderCompPtr->GetLicenseInstances();
		
		imtbase::ICollectionInfo::Ids licenseIds = licenseList.GetElementIds();
		for (const QByteArray& licenseId : licenseIds){
			const ILicenseInstance* licenseInstancePtr = m_licenseInfoProviderCompPtr->GetLicenseInstance(licenseId);
			if (licenseInstancePtr != nullptr){
//				ILicenseInfo::FeatureInfos features = licenseInstancePtr->GetFeatureInfos();
//				for (const ILicenseInfo::FeatureInfo& featureInfo : features){
//					if (featureInfo.id == operationId){
//						bool isExpired = (licenseInstancePtr->GetExpiration() >= QDateTime::currentDateTime());

//						return !isExpired;
//					}
//				}
			}
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


