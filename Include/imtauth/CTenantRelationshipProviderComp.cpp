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
		*isReadPtr = tenantId.isEmpty();
	}

	if (tenantId.isEmpty()){
		return retVal;
	}

	if (tenantNamePtr != nullptr){
		*tenantNamePtr = GetTenantName(tenantId);
	}

	// A partial read would look like ended relationships, and the shares of those are removed
	bool isRead = false;
	QByteArrayList relatedTenantIds;
	for (const QByteArray& relationshipId : m_connectionRequestManagerCompPtr->GetTenantRelationshipIds(tenantId, &isRead)){
		ITenantRelationshipInfoUniquePtr relationshipPtr = m_connectionRequestManagerCompPtr->GetTenantRelationship(relationshipId);
		if (!relationshipPtr.IsValid()){
			isRead = false;
			continue;
		}

		if (relationshipPtr->GetStatus() != ITenantRelationshipInfo::TRS_ACTIVE){
			continue;
		}

		const QByteArray relatedTenantId = relationshipPtr->GetSourceTenantId() == tenantId
					? relationshipPtr->GetTargetTenantId()
					: relationshipPtr->GetSourceTenantId();
		if (relatedTenantId.isEmpty() || relatedTenantId == tenantId){
			continue;
		}

		const bool isEffective = IsTenantRelationshipValidNow(relationshipPtr->GetValidFrom(), relationshipPtr->GetValidUntil());
		const int index = relatedTenantIds.indexOf(relatedTenantId);
		if (index >= 0){
			retVal[index].isEffective = retVal[index].isEffective || isEffective;
			continue;
		}

		relatedTenantIds.append(relatedTenantId);

		RelatedTenantInfo relatedTenantInfo;
		relatedTenantInfo.tenantId = relatedTenantId;
		relatedTenantInfo.tenantName = GetTenantName(relatedTenantId);
		relatedTenantInfo.isEffective = isEffective;
		relatedTenantInfo.relatedSince = m_connectionRequestManagerCompPtr->GetRelatedSince(tenantId, relatedTenantId);

		retVal.append(relatedTenantInfo);
	}

	if (isReadPtr != nullptr){
		*isReadPtr = isRead;
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


