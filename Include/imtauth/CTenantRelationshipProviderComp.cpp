// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CTenantRelationshipProviderComp.h>


// STL includes
#include <algorithm>

// ImtCore includes
#include <imtauth/ITenantRelationshipInfo.h>


namespace imtauth
{


namespace
{


// Reads of the relationships while they keep changing, after which the read counts as incomplete
constexpr int s_maxReadAttempts = 3;


} // anonymous namespace



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

	// A partial read would look like ended relationships, and the shares of those are removed. A relationship created
	// while the others are read is missing from the ids read before, and the one it replaces may already be archived:
	// the read counts only when the ids did not change meanwhile.
	bool isRead = false;
	QByteArrayList relationshipIds = m_connectionRequestManagerCompPtr->GetTenantRelationshipIds(tenantId, &isRead);
	for (int attempt = 0; isRead && attempt < s_maxReadAttempts; ++attempt){
		retVal = ReadRelatedTenants(tenantId, relationshipIds, isRead);

		bool isReread = false;
		QByteArrayList rereadIds = m_connectionRequestManagerCompPtr->GetTenantRelationshipIds(tenantId, &isReread);
		std::sort(relationshipIds.begin(), relationshipIds.end());
		std::sort(rereadIds.begin(), rereadIds.end());
		if (isRead && isReread && rereadIds == relationshipIds){
			if (isReadPtr != nullptr){
				*isReadPtr = true;
			}

			return retVal;
		}

		isRead = isReread;
		relationshipIds = rereadIds;
	}

	if (isReadPtr != nullptr){
		*isReadPtr = false;
	}

	return retVal;
}


// private methods

RelatedTenantInfos CTenantRelationshipProviderComp::ReadRelatedTenants(const QByteArray& tenantId, const QByteArrayList& relationshipIds, bool& isRead) const
{
	RelatedTenantInfos retVal;
	QByteArrayList relatedTenantIds;
	for (const QByteArray& relationshipId : relationshipIds){
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
		bool isHistoryRead = false;
		relatedTenantInfo.relatedSince = m_connectionRequestManagerCompPtr->GetRelatedSince(tenantId, relatedTenantId, &isHistoryRead);
		isRead = isRead && isHistoryRead;

		retVal.append(relatedTenantInfo);
	}

	return retVal;
}


QString CTenantRelationshipProviderComp::GetTenantName(const QByteArray& tenantId) const
{
	if (!m_tenantManagerCompPtr.IsValid()){
		return QString();
	}

	ITenantInfoUniquePtr tenantInfoPtr = m_tenantManagerCompPtr->GetTenant(tenantId);

	return tenantInfoPtr.IsValid() ? tenantInfoPtr->GetTenantName() : QString();
}


} // namespace imtauth


