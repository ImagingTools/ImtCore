// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CTenantRelationshipProviderComp.h>


// ImtCore includes
#include <imtauth/ITenantRelationshipInfo.h>


namespace imtauth
{


// reimplemented (imtauth::ITenantRelationshipProvider)

RelatedTenantInfos CTenantRelationshipProviderComp::GetRelatedTenants(const QByteArray& tenantId, QString* tenantNamePtr, bool* isReadPtr) const
{
	RelatedTenantInfos retVal;

	if (isReadPtr != nullptr){
		*isReadPtr = true;
	}

	if (tenantId.isEmpty()){
		return retVal;
	}

	if (tenantNamePtr != nullptr){
		*tenantNamePtr = GetTenantName(tenantId);
	}

	QByteArrayList relatedTenantIds;
	for (const QByteArray& relationshipId : m_connectionRequestManagerCompPtr->GetTenantRelationshipIds(tenantId)){
		ITenantRelationshipInfoUniquePtr relationshipPtr = m_connectionRequestManagerCompPtr->GetTenantRelationship(relationshipId);
		if (		!relationshipPtr.IsValid() ||
					relationshipPtr->GetStatus() != ITenantRelationshipInfo::TRS_ACTIVE ||
					!IsTenantRelationshipValidNow(relationshipPtr->GetValidFrom(), relationshipPtr->GetValidUntil())){
			continue;
		}

		const QByteArray relatedTenantId = relationshipPtr->GetSourceTenantId() == tenantId
					? relationshipPtr->GetTargetTenantId()
					: relationshipPtr->GetSourceTenantId();
		if (!relatedTenantId.isEmpty() && relatedTenantId != tenantId && !relatedTenantIds.contains(relatedTenantId)){
			relatedTenantIds.append(relatedTenantId);

			RelatedTenantInfo relatedTenantInfo;
			relatedTenantInfo.tenantId = relatedTenantId;
			relatedTenantInfo.tenantName = GetTenantName(relatedTenantId);

			retVal.append(relatedTenantInfo);
		}
	}

	return retVal;
}


// private methods

QString CTenantRelationshipProviderComp::GetTenantName(const QByteArray& tenantId) const
{
	if (!m_tenantManagerCompPtr.IsValid()){
		return QString();
	}

	ITenantInfoUniquePtr tenantInfoPtr = m_tenantManagerCompPtr->GetTenant(tenantId);

	return tenantInfoPtr.IsValid() ? tenantInfoPtr->GetTenantName() : QString();
}


} // namespace imtauth


