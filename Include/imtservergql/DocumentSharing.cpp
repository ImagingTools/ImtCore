// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/DocumentSharing.h>


// ImtCore includes
#include <iprm/CParamsSet.h>
#include <istd/TDelPtr.h>
#include <imtauth/CTenantFilterParam.h>
#include <imtcol/CDocumentIdFilter.h>
#include <imtauthgql/imtauthgql.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ImtCollection.h>


namespace imtservergql
{


QString DocumentAccessLevelToString(imtauth::IDocumentShareInfo::AccessLevel accessLevel)
{
	switch (accessLevel){
	case imtauth::IDocumentShareInfo::DAL_VIEW:
		return QStringLiteral("View");
	case imtauth::IDocumentShareInfo::DAL_EDIT:
		return QStringLiteral("Edit");
	case imtauth::IDocumentShareInfo::DAL_OWNER:
		return QStringLiteral("Owner");
	case imtauth::IDocumentShareInfo::DAL_NONE:
		break;
	}

	return QStringLiteral("None");
}


imtauth::IDocumentShareInfo::AccessLevel DocumentAccessLevelFromString(const QString& accessLevel)
{
	if (accessLevel == QStringLiteral("View")){
		return imtauth::IDocumentShareInfo::DAL_VIEW;
	}

	if (accessLevel == QStringLiteral("Edit")){
		return imtauth::IDocumentShareInfo::DAL_EDIT;
	}

	return imtauth::IDocumentShareInfo::DAL_NONE;
}


QString DocumentShareStatusToString(imtauth::IDocumentShareInfo::ShareStatus status)
{
	switch (status){
	case imtauth::IDocumentShareInfo::DSS_ACTIVE:
		return QStringLiteral("Active");
	case imtauth::IDocumentShareInfo::DSS_DECLINED:
		return QStringLiteral("Declined");
	case imtauth::IDocumentShareInfo::DSS_PENDING:
		break;
	}

	return QStringLiteral("Pending");
}


imtauth::SharingScope SharingScopeFromString(const QString& sharingScope)
{
	if (sharingScope == QStringLiteral("Own")){
		return imtauth::SS_OWN;
	}

	if (sharingScope == QStringLiteral("Incoming")){
		return imtauth::SS_INCOMING;
	}

	if (sharingScope == QStringLiteral("Outgoing")){
		return imtauth::SS_OUTGOING;
	}

	return imtauth::SS_ALL;
}


sdl::V1_0::imtbase::CDocumentShare CreateDocumentShareRepresentation(const imtauth::IDocumentShareInfo& shareInfo)
{
	sdl::V1_0::imtbase::CDocumentShare retVal;
	retVal.id = shareInfo.GetShareId();
	retVal.collectionId = shareInfo.GetCollectionId();
	retVal.objectId = shareInfo.GetEntityId();
	retVal.objectName = shareInfo.GetEntityName();
	retVal.ownerTenantId = shareInfo.GetOwnerTenantId();
	retVal.ownerTenantName = shareInfo.GetOwnerTenantName();
	retVal.targetTenantId = shareInfo.GetTargetTenantId();
	retVal.targetTenantName = shareInfo.GetTargetTenantName();
	retVal.targetGroupIds.Emplace();
	retVal.targetGroupIds->FromList(shareInfo.GetTargetGroupIds());
	retVal.accessLevel = DocumentAccessLevelToString(shareInfo.GetAccessLevel());
	retVal.status = DocumentShareStatusToString(shareInfo.GetStatus());
	retVal.validUntil = shareInfo.GetValidUntil().toString(Qt::ISODateWithMs);
	retVal.message = shareInfo.GetMessage();
	retVal.createdAt = shareInfo.GetCreatedAt().toString(Qt::ISODateWithMs);
	retVal.updatedAt = shareInfo.GetUpdatedAt().toString(Qt::ISODateWithMs);

	return retVal;
}


bool IsAccessCheckBypassed(const imtgql::CGqlRequest& gqlRequest)
{
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();

	return imtauthgql::IsSuperuserRequest(gqlRequest) && gqlContextPtr != nullptr && gqlContextPtr->GetTenantId().isEmpty();
}


bool AreObjectsInSharingScope(
			const imtbase::IObjectCollection& collection,
			const imtauth::ITenantFilterParam& tenantFilter,
			const QByteArrayList& objectIds,
			imtauth::SharingScope sharingScope,
			imtauth::IDocumentShareInfo::AccessLevel accessLevel)
{
	QByteArrayList uniqueIds;
	for (const QByteArray& objectId : objectIds){
		if (!objectId.isEmpty() && !uniqueIds.contains(objectId)){
			uniqueIds.append(objectId);
		}
	}

	if (uniqueIds.isEmpty()){
		return true;
	}

	istd::TDelPtr<imtauth::CTenantFilterParam> tenantFilterPtr(new imtauth::CTenantFilterParam);
	if (!tenantFilterPtr->CopyFrom(tenantFilter)){
		return false;
	}

	tenantFilterPtr->SetSharingScope(sharingScope);
	tenantFilterPtr->SetMinimumAccessLevel(accessLevel);

	istd::TDelPtr<imtcol::CDocumentIdFilter> documentIdFilterPtr(new imtcol::CDocumentIdFilter);
	documentIdFilterPtr->SetDocumentIds(uniqueIds);
	documentIdFilterPtr->SetConditionType(imtcol::IDocumentIdFilter::CT_IN);

	iprm::CParamsSet filterParams;
	filterParams.SetEditableParameter("TenantFilter", tenantFilterPtr.PopPtr(), true);
	filterParams.SetEditableParameter("DocumentIdFilter", documentIdFilterPtr.PopPtr(), true);

	// The count is never cached, so a revoked share takes effect immediately
	return collection.GetElementsCount(&filterParams) == uniqueIds.size();
}


bool HasObjectAccess(
			const imtbase::IObjectCollection& collection,
			const imtauth::IDocumentShareManager* shareManagerPtr,
			const imtgql::CGqlRequest& gqlRequest,
			const QByteArrayList& objectIds,
			imtauth::IDocumentShareInfo::AccessLevel accessLevel)
{
	if (IsAccessCheckBypassed(gqlRequest)){
		return true;
	}

	istd::TDelPtr<imtauth::CTenantFilterParam> tenantFilterPtr(imtauthgql::CreateTenantFilterParam(gqlRequest));
	if (!tenantFilterPtr.IsValid()){
		return false;
	}

	if (!tenantFilterPtr->GetTenantId().isEmpty()){
		const QByteArrayList shareOwnerTenantIds = shareManagerPtr != nullptr ? shareManagerPtr->GetShareOwnerTenantIds(tenantFilterPtr->GetTenantId()) : QByteArrayList();
		tenantFilterPtr->SetShareOwnerTenantIds(shareOwnerTenantIds, true);
		if (shareManagerPtr != nullptr){
			tenantFilterPtr->SetSharesRevision(shareManagerPtr->GetSharesRevision());
		}
	}

	return AreObjectsInSharingScope(collection, *tenantFilterPtr, objectIds, imtauth::SS_ACCESSIBLE, accessLevel);
}


} // namespace imtservergql


