// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantCollectionControllerComp.h>


// ImtCore includes
#include <imtauth/ITenantInfo.h>

// ACF includes
#include <iprm/CParamsSet.h>
#include <istd/TDelPtr.h>


namespace imtauthgql
{

namespace
{

static const QByteArray kCurrentUserFilterValue("__CURRENT_USER__");

void ReplaceCurrentUserSentinel(imtbase::IComplexCollectionFilter::FilterExpression& expression, const QByteArray& userId)
{
	for (imtbase::IComplexCollectionFilter::FieldFilter& fieldFilter : expression.fieldFilters){
		if (fieldFilter.fieldId == "OwnerId" && fieldFilter.filterValue.toByteArray() == kCurrentUserFilterValue){
			fieldFilter.filterValue = userId;
		}
	}

	for (imtbase::IComplexCollectionFilter::FilterExpression& childExpression : expression.filterExpressions){
		ReplaceCurrentUserSentinel(childExpression, userId);
	}
}

void AddVisibilityFilter(
		imtbase::CComplexCollectionFilter& complexFilter,
		const QByteArray& userId,
		const imtauth::ITenantMembershipManager* membershipManagerPtr)
{
	if (userId.isEmpty()){
		complexFilter.AddFieldFilter(imtbase::IComplexCollectionFilter::FieldFilter("Id", QByteArray()));
		return;
	}

	QVector<imtbase::IComplexCollectionFilter::FieldFilter> visibilityFieldFilters;
	visibilityFieldFilters.append(imtbase::IComplexCollectionFilter::FieldFilter("OwnerId", userId));

	if (membershipManagerPtr != nullptr){
		QByteArrayList membershipIds = membershipManagerPtr->GetMembershipsByUser(userId);
		for (const QByteArray& membershipId : membershipIds){
			const imtauth::ITenantMembership* membershipPtr = membershipManagerPtr->GetMembership(membershipId);
			if (membershipPtr != nullptr && membershipPtr->IsActive()){
				visibilityFieldFilters.append(imtbase::IComplexCollectionFilter::FieldFilter("Id", membershipPtr->GetTenantId()));
			}
		}
	}

	complexFilter.AddFilterExpression(imtbase::IComplexCollectionFilter::FilterExpression(
		visibilityFieldFilters,
		QVector<imtbase::IComplexCollectionFilter::FilterExpression>(),
		imtbase::IComplexCollectionFilter::LO_OR));
}

} // namespace


// reimplemented (sdl::imtauth::Tenants::CTenantCollectionControllerCompBase)

bool CTenantCollectionControllerComp::CreateRepresentationFromObject(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const sdl::imtauth::Tenants::CGetTenantListGqlRequest& getTenantListRequest,
			sdl::imtauth::Tenants::CTenantItemData::V1_0& representationObject,
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

	sdl::imtauth::Tenants::GetTenantListRequestInfo requestInfo = getTenantListRequest.GetRequestInfo();

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

	if (requestInfo.items.isOwnerIdRequested){
		representationObject.ownerId = tenantInfoPtr->GetOwnerId();
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
			const imtauth::ITenantMembership* membershipPtr = m_membershipManagerCompPtr->GetMembership(membershipId);
			if (membershipPtr != nullptr && membershipPtr->IsActive()){
				++activeMembersCount;
			}
		}
		representationObject.membersCount = activeMembersCount;
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

	istd::TDelPtr<imtbase::CComplexCollectionFilter> complexFilterPtr = new imtbase::CComplexCollectionFilter();
	AddVisibilityFilter(*complexFilterPtr, GetUserId(gqlRequest), m_membershipManagerCompPtr.GetPtr());
	filterParamsPtr->SetEditableParameter("ComplexFilter", complexFilterPtr.PopPtr(), true);
}


void CTenantCollectionControllerComp::SetAdditionalFilters(
			const imtgql::CGqlRequest& gqlRequest,
			imtbase::CComplexCollectionFilter& complexFilter) const
{
	QByteArray userId = GetUserId(gqlRequest);
	imtbase::IComplexCollectionFilter::FilterExpression expression = complexFilter.GetFilterExpression();
	ReplaceCurrentUserSentinel(expression, userId);
	complexFilter.SetFilterExpression(expression);
	AddVisibilityFilter(complexFilter, userId, m_membershipManagerCompPtr.GetPtr());
}


} // namespace imtauthgql
