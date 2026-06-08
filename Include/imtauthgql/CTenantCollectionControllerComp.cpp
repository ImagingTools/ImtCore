// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantCollectionControllerComp.h>
#include <imtauthsdl/SDL/1.0/CPP/Tenants.h>


// ACF includes
#include <iprm/CParamsSet.h>
#include <iprm/CIdParam.h>

// ImtCore includes
#include <imtauth/ITenantInfo.h>
#include <imtauth/ITenantInvitation.h>
#include <imtauth/imtauth.h>
#include <imtgql/IGqlContext.h>


namespace imtauthgql
{


// reimplemented (sdl::V1_0::imtauth::CTenantCollectionControllerCompBase)

bool CTenantCollectionControllerComp::CreateRepresentationFromObject(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const sdl::V1_0::imtauth::CGetTenantListGqlRequest& getTenantListRequest,
			sdl::V1_0::imtauth::CTenantItemData& representationObject,
			QString& errorMessage) const
{
	QByteArray objectId = objectCollectionIterator.GetObjectId();

	const imtauth::ITenantInfo* tenantInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr tenantDataPtr;
	if (objectCollectionIterator.GetObjectData(tenantDataPtr)){
		tenantInfoPtr = dynamic_cast<const imtauth::ITenantInfo*>(tenantDataPtr.GetPtr());
	}

	if (tenantInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object '%1'").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "CTenantCollectionControllerComp");

		return false;
	}

	sdl::V1_0::imtauth::GetTenantListRequestInfo requestInfo = getTenantListRequest.GetRequestInfo();

	if (requestInfo.items.isIdRequested){
		representationObject.id = QByteArray(objectId);
	}

	if (requestInfo.items.isTypeIdRequested){
		representationObject.typeId = objectCollectionIterator.GetObjectTypeId();
	}

	if (requestInfo.items.isNameRequested){
		representationObject.name = QString(tenantInfoPtr->GetTenantName());
	}

	if (requestInfo.items.isDescriptionRequested){
		representationObject.description = QString(tenantInfoPtr->GetTenantDescription());
	}

	if (requestInfo.items.isOwnerIdRequested && m_userCollectionCompPtr.IsValid()){
		representationObject.ownerId = imtauth::GetUserName(*m_userCollectionCompPtr, tenantInfoPtr->GetOwnerId()).toUtf8();
	}

	if (requestInfo.items.isIsActiveRequested){
		representationObject.isActive = tenantInfoPtr->IsActive();
	}

	if (requestInfo.items.isCreatedAtRequested){
		representationObject.createdAt = tenantInfoPtr->GetCreatedAt();
	}

	if (requestInfo.items.isUpdatedAtRequested){
		representationObject.updatedAt = tenantInfoPtr->GetUpdatedAt();
	}

	if (requestInfo.items.isMembersCountRequested && m_membershipManagerCompPtr.IsValid()){
		QByteArrayList membershipIds = m_membershipManagerCompPtr->GetMembershipsByTenant(objectId);
		int activeMembersCount = 0;
		for (const QByteArray& membershipId : membershipIds){
			imtauth::ITenantMembershipUniquePtr membershipPtr = m_membershipManagerCompPtr->GetMembership(membershipId);
			if (membershipPtr.IsValid() && membershipPtr->IsActive()){
				++activeMembersCount;
			}
		}
		representationObject.membersCount = activeMembersCount;
	}

	if (requestInfo.items.isTenantRelationScopeRequested){
		QVariant scope = objectCollectionIterator.GetElementInfo("TenantRelationScope");
		if (scope.isValid() && !scope.isNull()){
			representationObject.tenantRelationScope = scope.toString();
		}
	}

	if ((requestInfo.items.isInvitationIdRequested || requestInfo.items.isInvitedByNameRequested)
		&& representationObject.tenantRelationScope == "Invited"
		&& m_invitationManagerCompPtr.IsValid()){
		const imtgql::IGqlContext* gqlContextPtr = getTenantListRequest.GetRequestContext();
		if (gqlContextPtr != nullptr){
			QByteArray userId = gqlContextPtr->GetUserId();
			if (!userId.isEmpty()){
				imtauth::ITenantInvitationUniquePtr invitationPtr = m_invitationManagerCompPtr->FindPendingInvitation(userId, objectId);
				if (invitationPtr.IsValid()){
					if (requestInfo.items.isInvitationIdRequested){
						representationObject.invitationId = invitationPtr->GetInvitationId();
					}
					if (requestInfo.items.isInvitedByNameRequested && m_userCollectionCompPtr.IsValid()){
						representationObject.invitedByName = imtauth::GetUserName(*m_userCollectionCompPtr, invitationPtr->GetInvitedByUserId());
					}
				}
			}
		}
	}

	if (requestInfo.items.isParentTenantIdRequested){
		representationObject.parentTenantId = tenantInfoPtr->GetParentTenantId();
	}

	if (requestInfo.items.isIsSystemTenantRequested){
		representationObject.isSystemTenant = (tenantInfoPtr->GetTenantId() == imtauth::GetSystemTenantId());
	}

	return true;
}


void CTenantCollectionControllerComp::SetAdditionalFilters(
			const imtgql::CGqlRequest& gqlRequest,
			const imtgql::CGqlParamObject& /*viewParamsGql*/,
			iprm::CParamsSet* filterParamsPtr) const
{
	if (filterParamsPtr == nullptr){
		return;
	}

	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr == nullptr){
		return;
	}

	const imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
	if (userInfoPtr == nullptr){
		return;
	}

	if (userInfoPtr->IsAdmin()){
		// return;
	}

	QByteArray userId = gqlContextPtr->GetUserId();
	if (userId.isEmpty()){
		return;
	}

	iprm::CIdParam* userIdParamPtr = new iprm::CIdParam();
	userIdParamPtr->SetId(userId);
	filterParamsPtr->SetEditableParameter("UserId", userIdParamPtr, true);
}


} // namespace imtauthgql


