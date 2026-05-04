// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CTenantInfoProviderComp.h>


// ImtCore includes
#include <imtbase/CCollectionInfo.h>
#include <imtauth/ITenantInfo.h>


namespace imtauth
{


// public methods

// reimplemented (ITenantInfoProvider)

const imtbase::ICollectionInfo& CTenantInfoProviderComp::GetTenantList() const
{
	static imtbase::CCollectionInfo emptyCollection;

	if (m_tenantCollectionCompPtr.IsValid()){
		return *m_tenantCollectionCompPtr;
	}

	return emptyCollection;
}


imtauth::ITenantInfoSharedPtr CTenantInfoProviderComp::GetTenant(const QByteArray& tenantId, const iprm::IParamsSet* paramsPtr) const
{
	if (m_tenantCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_tenantCollectionCompPtr->GetObjectData(tenantId, dataPtr, paramsPtr)){
			ITenantInfoSharedPtr retVal;
			retVal.SetCastedPtr(dataPtr);

			return retVal;
		}
	}

	return nullptr;
}


} // namespace imtauth


