// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantRelationshipItemInfoProviderComp.h>


// ACF includes
#include <iprm/CParamsSet.h>
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


// reimplemented (IObjectParamsFiller)

bool CTenantRelationshipItemInfoProviderComp::FillParams(
			const QByteArray& objectId,
			iprm::IParamsSet& paramsSet,
			const QByteArray& contextTenantId) const
{
	iprm::CParamsSet* paramsSetPtr = dynamic_cast<iprm::CParamsSet*>(&paramsSet);
	if (paramsSetPtr == nullptr){
		return false;
	}

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

	// Determine partner: if current tenant is source, partner is target and vice versa
	QByteArray partnerTenantId;
	imtauth::ITenantRelationshipInfo::TenantRelationshipRole myRole;
	imtauth::ITenantRelationshipInfo::TenantRelationshipRole partnerRole;

	if (!contextTenantId.isEmpty() && relationshipPtr->GetTargetTenantId() == contextTenantId){
		// Current tenant is the target — partner is the source
		partnerTenantId = relationshipPtr->GetSourceTenantId();
		myRole = relationshipPtr->GetTargetRole();
		partnerRole = relationshipPtr->GetSourceRole();
	}
	else{
		// Current tenant is the source (or context unknown — fallback to old behavior)
		partnerTenantId = relationshipPtr->GetTargetTenantId();
		myRole = relationshipPtr->GetSourceRole();
		partnerRole = relationshipPtr->GetTargetRole();
	}

	// Partner tenant name
	if (!partnerTenantId.isEmpty()){
		QString partnerName;
		if (m_tenantCollectionCompPtr.IsValid()){
			partnerName = m_tenantCollectionCompPtr->GetElementInfo(partnerTenantId, imtbase::ICollectionInfo::EIT_NAME).toString();
		}

		iprm::CTextParam* partnerParamPtr = new iprm::CTextParam;
		partnerParamPtr->SetText(partnerName.isEmpty() ? QString::fromUtf8(partnerTenantId) : partnerName);
		paramsSetPtr->SetEditableParameter("partnerTenant", partnerParamPtr, true);
	}

	// My tenant name
	if (!contextTenantId.isEmpty()){
		QString myTenantName;
		if (m_tenantCollectionCompPtr.IsValid()){
			myTenantName = m_tenantCollectionCompPtr->GetElementInfo(contextTenantId, imtbase::ICollectionInfo::EIT_NAME).toString();
		}

		iprm::CTextParam* myTenantParamPtr = new iprm::CTextParam;
		myTenantParamPtr->SetText(myTenantName.isEmpty() ? QString::fromUtf8(contextTenantId) : myTenantName);
		paramsSetPtr->SetEditableParameter("myTenant", myTenantParamPtr, true);
	}

	// My role in this relationship
	{
		iprm::CTextParam* myRoleParamPtr = new iprm::CTextParam;
		myRoleParamPtr->SetText(RelationshipRoleToString(myRole));
		paramsSetPtr->SetEditableParameter("myRole", myRoleParamPtr, true);
	}

	// Partner role
	{
		iprm::CTextParam* roleParamPtr = new iprm::CTextParam;
		roleParamPtr->SetText(RelationshipRoleToString(partnerRole));
		paramsSetPtr->SetEditableParameter("role", roleParamPtr, true);
	}

	// Status
	{
		iprm::CTextParam* statusParamPtr = new iprm::CTextParam;
		statusParamPtr->SetText(RelationshipStatusToString(relationshipPtr->GetStatus()));
		paramsSetPtr->SetEditableParameter("status", statusParamPtr, true);
	}

	return true;
}


} // namespace imtauthgql
