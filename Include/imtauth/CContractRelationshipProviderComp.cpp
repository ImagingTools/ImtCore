// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CContractRelationshipProviderComp.h>


// Qt includes
#include <QtCore/QMap>

// ImtCore includes
#include <imtauth/ContractState.h>


namespace imtauth
{


// reimplemented (imtauth::ITenantRelationshipProvider)

RelatedTenantInfos CContractRelationshipProviderComp::GetRelatedTenants(const QByteArray& tenantId, QString* tenantNamePtr, bool* isReadPtr) const
{
	struct PartnerLinks
	{
		RelatedTenantInfo info;
		QList<RelationshipPeriod> periods;
		bool isRelated = false;
	};

	bool isRead = false;
	const ContractInfoList contracts = m_contractProviderCompPtr->GetContractsOfParty(tenantId, &isRead);
	if (isReadPtr != nullptr){
		*isReadPtr = isRead;
	}

	const QDateTime now = QDateTime::currentDateTimeUtc();
	QMap<QByteArray, PartnerLinks> partners;
	for (const IContractInfoSharedPtr& contractPtr : contracts){
		const IContractInfo::Parties parties = contractPtr->GetParties();
		for (const IContractInfo::Link& link : contractPtr->GetLinks()){
			if (link.tenantAId != tenantId && link.tenantBId != tenantId){
				continue;
			}

			RelationshipPeriod period;
			if (!GetLinkPeriod(*contractPtr, link, now, period)){
				continue;
			}

			const QByteArray partnerId = (link.tenantAId == tenantId) ? link.tenantBId : link.tenantAId;
			PartnerLinks& partner = partners[partnerId];
			if (partner.info.tenantId.isEmpty()){
				partner.info.tenantId = partnerId;
				partner.info.isEffective = false;
				const int partyIndex = FindContractParty(parties, partnerId);
				partner.info.tenantName = (partyIndex >= 0) ? parties[partyIndex].tenantName : QString();
			}

			partner.periods.append(period);
			if (!period.end.isValid()){
				partner.isRelated = true;
				partner.info.relatedContractIds.append(contractPtr->GetContractId());
			}

			if (IsLinkInForce(*contractPtr, link, now)){
				partner.info.isEffective = true;
				if (link.isSharingAllowed){
					partner.info.sharingContractIds.append(contractPtr->GetContractId());
				}
			}
		}
	}

	if (tenantNamePtr != nullptr && m_tenantManagerCompPtr.IsValid()){
		ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(tenantId);
		if (tenantPtr.IsValid()){
			*tenantNamePtr = tenantPtr->GetTenantName();
		}
	}

	RelatedTenantInfos retVal;
	for (PartnerLinks& partner : partners){
		if (!partner.isRelated){
			continue;
		}

		if (m_tenantManagerCompPtr.IsValid()){
			ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(partner.info.tenantId);
			if (tenantPtr.IsValid()){
				partner.info.tenantName = tenantPtr->GetTenantName();
			}
		}

		partner.info.relatedSince = GetUninterruptedStart(partner.periods);
		retVal.append(partner.info);
	}

	return retVal;
}


} // namespace imtauth


