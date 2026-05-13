// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
				ILicenseDefinition::FeatureInfos features = licenseInstancePtr->GetFeatureInfos();
				for (const ILicenseDefinition::FeatureInfo& featureInfo : features){
					if (featureInfo.id == operationId){
						QDateTime expirationDate = licenseInstancePtr->GetExpiration();
						if (!expirationDate.isNull()){
							bool isExpired = (QDateTime::currentDateTime() > licenseInstancePtr->GetExpiration());

							return !isExpired;
						}

						return true;
					}
				}
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


