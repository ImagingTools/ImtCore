// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantCollectionControllerComp.h>
#include <imtauthgql/TenantAccess.h>
#include <imtauthgql/imtauthgql.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Tenants.h>


// ACF includes
#include <iprm/CParamsSet.h>
#include <iprm/CIdParam.h>
#include <iprm/CEnableableParam.h>

// ImtCore includes
#include <imtauth/ITenantInfo.h>
#include <imtauth/ITenantInvitation.h>
#include <imtauth/imtauth.h>
#include <imtgql/IGqlContext.h>
#include <imtgql/CGqlParamObject.h>
#include <imtbase/imtbase.h>
#include <imtbase/CComplexCollectionFilter.h>


namespace imtauthgql
{


namespace
{


// Stands for the missing user of an anonymous request: no organization is related to it
const QByteArray s_noUserId = QByteArrayLiteral("00000000-0000-0000-0000-000000000000");


} // anonymous namespace


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
		errorMessage = QStringLiteral("Unable to create representation from object '%1'").arg(objectId);
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

	QString tenantRelationScope;
	if (requestInfo.items.isTenantRelationScopeRequested){
		QVariant scope = objectCollectionIterator.GetElementInfo("TenantRelationScope");
		if (scope.isValid() && !scope.isNull()){
			tenantRelationScope = scope.toString();
			representationObject.tenantRelationScope = tenantRelationScope;
		}
	}

	if ((requestInfo.items.isInvitationIdRequested || requestInfo.items.isInvitedByNameRequested) &&
				tenantRelationScope== "Invited" &&
				m_invitationManagerCompPtr.IsValid()){
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

	if (requestInfo.items.isParentTenantNameRequested && !tenantInfoPtr->GetParentTenantId().isEmpty() && m_objectCollectionCompPtr.IsValid()){
		representationObject.parentTenantName = m_objectCollectionCompPtr->GetElementInfo(tenantInfoPtr->GetParentTenantId(), imtbase::ICollectionInfo::EIT_NAME).toString();
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
	const imtauth::IUserInfo* userInfoPtr = gqlContextPtr != nullptr ? gqlContextPtr->GetUserInfo() : nullptr;
	QByteArray userId = gqlContextPtr != nullptr ? gqlContextPtr->GetUserId() : QByteArray();

	// Nobody is related to an organization without a user, so an anonymous request selects nothing
	if (userInfoPtr == nullptr || userId.isEmpty()){
		userId = s_noUserId;
	}

	iprm::CIdParam* userIdParamPtr = new iprm::CIdParam();
	userIdParamPtr->SetId(userId);
	filterParamsPtr->SetEditableParameter("UserId", userIdParamPtr, true);

	// System administrators get a global view, the others see only the tenants they are related to.
	if (userInfoPtr == nullptr || !userInfoPtr->IsAdmin()){
		filterParamsPtr->SetEditableParameter("RelatedTenantsOnly", new iprm::CEnableableParam(true), true);
	}
}


// reimplemented (imtservergql::CObjectCollectionControllerCompBase)

bool CTenantCollectionControllerComp::CheckObjectAccess(
			const imtgql::CGqlRequest& gqlRequest,
			const QByteArrayList& objectIds,
			imtauth::IDocumentShareInfo::AccessLevel accessLevel,
			QString& errorMessage) const
{
	if (!IsSuperuserRequest(gqlRequest) && m_objectCollectionCompPtr.IsValid()){
		// An organization the user is not related to is reported as missing, like a document of another organization
		for (const QByteArray& objectId : objectIds){
			imtbase::IComplexCollectionFilter::FilterExpression filterExpr;
			filterExpr.fieldFilters << imtbase::IComplexCollectionFilter::FieldFilter("Id", QString::fromUtf8(objectId));

			imtbase::CComplexCollectionFilter complexFilter;
			complexFilter.SetFilterExpression(filterExpr);

			iprm::CParamsSet filterParams;
			filterParams.SetEditableParameter("ComplexFilter", &complexFilter);
			SetAdditionalFilters(gqlRequest, imtgql::CGqlParamObject(), &filterParams);

			if (!imtbase::isValidUuid(objectId) || !m_objectCollectionCompPtr->GetElementIds(0, -1, &filterParams).contains(objectId)){
				errorMessage = QStringLiteral("Object does not exist");
				return false;
			}
		}
	}

	return BaseClass::CheckObjectAccess(gqlRequest, objectIds, accessLevel, errorMessage);
}


bool CTenantCollectionControllerComp::OnBeforeRemoveElements(const QByteArrayList& elementIds, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	// RemoveElementSet selects by the filter of the request without the access check of RemoveElements
	if (!CheckObjectAccess(gqlRequest, elementIds, imtauth::IDocumentShareInfo::DAL_OWNER, errorMessage)){
		return false;
	}

	if (m_objectCollectionCompPtr.IsValid() && !CheckTenantRemoval(*m_objectCollectionCompPtr, elementIds, errorMessage)){
		return false;
	}

	return BaseClass::OnBeforeRemoveElements(elementIds, gqlRequest, errorMessage);
}


} // namespace imtauthgql


