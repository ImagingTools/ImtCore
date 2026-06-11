// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantRelationshipItemInfoProviderComp.h>


// ACF includes
#include <iprm/CTextParam.h>

// ImtCore includes
#include <imtauth/ITenantRelationshipInfo.h>
#include <imtbase/ICollectionInfo.h>


namespace imtauthgql
{


namespace
{


QString RelationshipRoleToString(imtauth::ITenantRelationshipInfo::TenantRelationshipRole role)
{
	switch (role){
		case imtauth::ITenantRelationshipInfo::TRR_PARENT: return QStringLiteral("Parent");
		case imtauth::ITenantRelationshipInfo::TRR_CHILD: return QStringLiteral("Child");
		case imtauth::ITenantRelationshipInfo::TRR_PARTNER: return QStringLiteral("Partner");
		case imtauth::ITenantRelationshipInfo::TRR_SUPPLIER: return QStringLiteral("Supplier");
		case imtauth::ITenantRelationshipInfo::TRR_CUSTOMER: return QStringLiteral("Customer");
		case imtauth::ITenantRelationshipInfo::TRR_AFFILIATE: return QStringLiteral("Affiliate");
		default: return QStringLiteral("Unknown");
	}
}


QString RelationshipStatusToString(imtauth::ITenantRelationshipInfo::TenantRelationshipStatus status)
{
	switch (status){
		case imtauth::ITenantRelationshipInfo::TRS_ACTIVE: return QStringLiteral("Active");
		case imtauth::ITenantRelationshipInfo::TRS_ARCHIVED: return QStringLiteral("Archived");
		case imtauth::ITenantRelationshipInfo::TRS_PENDING_APPROVED: return QStringLiteral("Pending");
		default: return QStringLiteral("Unknown");
	}
}


} // anonymous namespace


// reimplemented (ISelectableItemInfoProvider)

bool CTenantRelationshipItemInfoProviderComp::GetItemParameters(
			const QByteArray& objectId,
			iprm::IParamsSet& paramsSet) const
{
	if (!m_relationshipCollectionCompPtr.IsValid()){
		return false;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_relationshipCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		return false;
	}

	const imtauth::ITenantRelationshipInfo* relationshipPtr = dynamic_cast<const imtauth::ITenantRelationshipInfo*>(dataPtr.GetPtr());
	if (relationshipPtr == nullptr){
		return false;
	}

	// Partner tenant (target)
	QByteArray targetTenantId = relationshipPtr->GetTargetTenantId();
	if (!targetTenantId.isEmpty()){
		QString partnerName;
		if (m_tenantCollectionCompPtr.IsValid()){
			partnerName = m_tenantCollectionCompPtr->GetElementInfo(targetTenantId, imtbase::ICollectionInfo::EIT_NAME).toString();
		}

		iprm::CTextParam* partnerParamPtr = new iprm::CTextParam;
		partnerParamPtr->SetText(partnerName.isEmpty() ? QString::fromUtf8(targetTenantId) : partnerName);
		paramsSet.SetEditableParameter("partnerTenant", partnerParamPtr, true);
	}

	// Relationship role
	{
		iprm::CTextParam* roleParamPtr = new iprm::CTextParam;
		roleParamPtr->SetText(RelationshipRoleToString(relationshipPtr->GetTargetRole()));
		paramsSet.SetEditableParameter("role", roleParamPtr, true);
	}

	// Status
	{
		iprm::CTextParam* statusParamPtr = new iprm::CTextParam;
		statusParamPtr->SetText(RelationshipStatusToString(relationshipPtr->GetStatus()));
		paramsSet.SetEditableParameter("status", statusParamPtr, true);
	}

	return true;
}


} // namespace imtauthgql
