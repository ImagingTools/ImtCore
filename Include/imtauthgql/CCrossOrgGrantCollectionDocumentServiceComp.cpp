// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CCrossOrgGrantCollectionDocumentServiceComp.h>


// Qt includes
#include <QtCore/QStringList>

// ImtCore includes
#include <imtauth/ICrossOrgGrantData.h>
#include <imtgql/IGqlContext.h>
#include <imtbase/ICollectionInfo.h>
#include <imtauthgql/TenantAccess.h>

// Generated includes
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/CrossOrgGrantCollectionDocumentService.h>


namespace imtauthgql
{


QString CCrossOrgGrantCollectionDocumentServiceComp::GetTenantName(const QByteArray& tenantId) const
{
	QString tenantName;
	if (m_tenantCollectionCompPtr.IsValid() && !tenantId.isEmpty()){
		tenantName = m_tenantCollectionCompPtr->GetElementInfo(tenantId, imtbase::ICollectionInfo::EIT_NAME).toString();
	}

	return tenantName.isEmpty() ? QString::fromUtf8(tenantId) : tenantName;
}


QStringList CCrossOrgGrantCollectionDocumentServiceComp::GetRoleNames(const QByteArrayList& roleIds) const
{
	QStringList roleNames;
	for (const QByteArray& roleId : roleIds){
		if (!roleId.isEmpty()){
			QString roleName;
			if (m_roleCollectionCompPtr.IsValid()){
				roleName = m_roleCollectionCompPtr->GetElementInfo(roleId, imtbase::ICollectionInfo::EIT_NAME).toString();
			}
			roleNames << (roleName.isEmpty() ? QString::fromUtf8(roleId) : roleName);
		}
	}

	return roleNames;
}


QString CCrossOrgGrantCollectionDocumentServiceComp::ComposeDefaultGrantName(const imtauth::CrossOrgGrantInfo& info) const
{
	const QString targetName = GetTenantName(info.targetTenantId);
	const QStringList roleNames = GetRoleNames(info.roleIds);
	if (roleNames.isEmpty()){
		return targetName;
	}

	return QStringLiteral("%1 (%2)").arg(targetName, roleNames.join(QStringLiteral(", ")));
}


// protected methods

// reimplemented (CCrossOrgGrantCollectionDocumentServiceGqlHandlerCompBase)

sdl::V1_0::imtauth::CCrossOrgGrant CCrossOrgGrantCollectionDocumentServiceComp::OnGetGrantRepresentation(
			const sdl::V1_0::imtauth::CGetGrantRepresentationGqlRequest& getGrantRepresentationRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::GetGrantRepresentationRequestArguments arguments = getGrantRepresentationRequest.GetRequestedArguments();

	QByteArray objectId;
	if (arguments.input && arguments.input->id){
		objectId = *arguments.input->id;
	}

	if (objectId.isEmpty()){
		errorMessage = QStringLiteral("Missing grant ID");
		return sdl::V1_0::imtauth::CCrossOrgGrant();
	}

	if (!m_documentManagerCompPtr.IsValid()){
		errorMessage = QStringLiteral("Document service not configured");
		return sdl::V1_0::imtauth::CCrossOrgGrant();
	}

	QByteArray userLogin = GetUserId(gqlRequest);
	istd::IChangeableSharedPtr documentPtr;
	m_documentManagerCompPtr->GetDocumentData(userLogin, objectId, documentPtr);

	if (documentPtr.IsValid()){
		const imtauth::ICrossOrgGrantData* grantPtr = dynamic_cast<const imtauth::ICrossOrgGrantData*>(documentPtr.GetPtr());
		if (grantPtr != nullptr){
			imtauth::CrossOrgGrantInfo info = grantPtr->GetGrantInfo();

			// Only the grantor and the grantee see a stored grant.
			const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
			QByteArray contextTenantId;
			if (gqlContextPtr != nullptr){
				contextTenantId = gqlContextPtr->GetTenantId();
			}
			const bool isNewGrant = info.grantId.isEmpty() && info.sourceTenantId.isEmpty();
			if (contextTenantId.isEmpty()
					|| (!isNewGrant && info.sourceTenantId != contextTenantId && info.targetTenantId != contextTenantId)){
				errorMessage = QStringLiteral("Access denied: grant does not belong to the current tenant");
				return sdl::V1_0::imtauth::CCrossOrgGrant();
			}

			sdl::V1_0::imtauth::CCrossOrgGrant response;
			response.id = info.grantId;
			response.sourceTenantId = info.sourceTenantId;
			response.targetTenantId = info.targetTenantId;
			if (!info.sourceTenantId.isEmpty()){
				response.sourceTenantName = GetTenantName(info.sourceTenantId);
			}
			if (!info.targetTenantId.isEmpty()){
				response.targetTenantName = GetTenantName(info.targetTenantId);
			}
			response.name = info.name;
			response.roleIds.Emplace().FromList(info.roleIds);
			imtsdl::TElementList<QString> roleNames;
			for (const QString& roleName : GetRoleNames(info.roleIds)){
				roleNames << roleName;
			}
			response.roleNames = roleNames;
			response.description = info.description;
			response.createdAt = info.createdAt;
			response.expiresAt = info.expiresAt;
			response.isActive = info.isActive;

			// The grantee only views the grant.
			response.isReadOnly = (info.sourceTenantId != contextTenantId) && !isNewGrant;

			return response;
		}
	}

	errorMessage = QStringLiteral("Grant not found");
	return sdl::V1_0::imtauth::CCrossOrgGrant();
}


sdl::V1_0::imtbase::CDocumentOperationStatus CCrossOrgGrantCollectionDocumentServiceComp::OnUpdateGrantFromRepresentation(
			const sdl::V1_0::imtauth::CUpdateGrantFromRepresentationGqlRequest& updateGrantFromRepresentationRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::UpdateGrantFromRepresentationRequestArguments arguments = updateGrantFromRepresentationRequest.GetRequestedArguments();
	sdl::V1_0::imtbase::CDocumentOperationStatus response;
	response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::Failed;

	QByteArray documentId;
	if (arguments.input->documentId){
		documentId = *arguments.input->documentId;
	}

	if (documentId.isEmpty()){
		errorMessage = QStringLiteral("Missing document ID");
		return response;
	}

	if (!m_documentManagerCompPtr.IsValid()){
		errorMessage = QStringLiteral("Document service not configured");
		return response;
	}

	QByteArray userLogin = GetUserId(gqlRequest);

	istd::IChangeableSharedPtr documentPtr;
	m_documentManagerCompPtr->GetDocumentData(userLogin, documentId, documentPtr);
	if (!documentPtr.IsValid()){
		response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	imtauth::ICrossOrgGrantData* grantDataPtr = dynamic_cast<imtauth::ICrossOrgGrantData*>(documentPtr.GetPtr());
	if (grantDataPtr == nullptr){
		response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	sdl::V1_0::imtauth::CCrossOrgGrant grantRepresentation;
	if (arguments.input->grant){
		grantRepresentation = *arguments.input->grant;
	} else {
		errorMessage = QStringLiteral("Missing grant representation");
		return response;
	}

	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	QByteArray contextTenantId;
	if (gqlContextPtr != nullptr){
		contextTenantId = gqlContextPtr->GetTenantId();
	}

	imtauth::CrossOrgGrantInfo info = grantDataPtr->GetGrantInfo();

	// The organization of the request context grants its own roles; the grantee cannot modify the grant.
	if (contextTenantId.isEmpty() || (!info.sourceTenantId.isEmpty() && info.sourceTenantId != contextTenantId)){
		errorMessage = QStringLiteral("Access denied: only the grantor tenant can modify this grant");
		return response;
	}
	if (!HasTenantManageAccess(
				*m_tenantManagerCompPtr.GetPtr(),
				m_membershipManagerCompPtr.GetPtr(),
				gqlRequest,
				contextTenantId,
				QByteArrayLiteral("ConnectOrganization"),
				errorMessage)){
		return response;
	}
	info.sourceTenantId = contextTenantId;

	// A name equal to the generated one follows the target and the roles.
	const QString previousDefaultName = ComposeDefaultGrantName(info);

	if (grantRepresentation.id){
		info.grantId = *grantRepresentation.id;
	}
	if (grantRepresentation.targetTenantId){
		info.targetTenantId = *grantRepresentation.targetTenantId;
	}
	if (info.targetTenantId == info.sourceTenantId){
		errorMessage = QStringLiteral("An organization cannot grant access to itself");
		return response;
	}
	if (grantRepresentation.name){
		info.name = *grantRepresentation.name;
	}
	if (grantRepresentation.roleIds){
		info.roleIds = grantRepresentation.roleIds->ToList();
	}
	if (grantRepresentation.description){
		info.description = *grantRepresentation.description;
	}
	if (grantRepresentation.expiresAt){
		info.expiresAt = *grantRepresentation.expiresAt;
	}
	if (grantRepresentation.isActive){
		info.isActive = *grantRepresentation.isActive;
	} else if (info.grantId.isEmpty()){
		// Default new grants to active only when the client omitted the flag.
		info.isActive = true;
	}

	if (info.name.isEmpty() || info.name == previousDefaultName){
		info.name = ComposeDefaultGrantName(info);
	}

	grantDataPtr->SetGrantInfo(info);

	m_documentManagerCompPtr->SetDocumentData(userLogin, documentId, *documentPtr);
	response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::Success;

	return response;
}


} // namespace imtauthgql
