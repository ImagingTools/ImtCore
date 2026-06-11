// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantRelationshipItemInfoProviderComp.h>


// ImtCore includes
#include <imtauth/ITenantRelationshipInfo.h>
#include <imtbase/ICollectionInfo.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ImtBaseTypes.h>


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
			imtsdl::TElementList<sdl::V1_0::imtbase::CParameter>& parameters) const
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
		sdl::V1_0::imtbase::CParameter partnerParam;
		partnerParam.id = "partnerTenant";
		partnerParam.name = QStringLiteral("Partner");

		QString partnerName;
		if (m_tenantCollectionCompPtr.IsValid()){
			partnerName = m_tenantCollectionCompPtr->GetElementInfo(targetTenantId, imtbase::ICollectionInfo::EIT_NAME).toString();
		}
		partnerParam.data = partnerName.isEmpty() ? QString::fromUtf8(targetTenantId) : partnerName;
		parameters << partnerParam;
	}

	// Relationship role (target role describes what the partner is to us)
	{
		sdl::V1_0::imtbase::CParameter roleParam;
		roleParam.id = "role";
		roleParam.name = QStringLiteral("Role");
		roleParam.data = RelationshipRoleToString(relationshipPtr->GetTargetRole());
		parameters << roleParam;
	}

	// Status
	{
		sdl::V1_0::imtbase::CParameter statusParam;
		statusParam.id = "status";
		statusParam.name = QStringLiteral("Status");
		statusParam.data = RelationshipStatusToString(relationshipPtr->GetStatus());
		parameters << statusParam;
	}

	return true;
}


} // namespace imtauthgql
