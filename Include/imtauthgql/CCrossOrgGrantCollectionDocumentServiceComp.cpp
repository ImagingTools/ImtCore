// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CCrossOrgGrantCollectionDocumentServiceComp.h>


// Qt includes
#include <QtCore/QStringList>

// ImtCore includes
#include <imtauth/ICrossOrgGrantData.h>
#include <imtgql/IGqlContext.h>
#include <imtbase/ICollectionInfo.h>

// Generated includes
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/CrossOrgGrantCollectionDocumentService.h>


namespace imtauthgql
{


QString CCrossOrgGrantCollectionDocumentServiceComp::ComposeDefaultGrantName(const imtauth::CrossOrgGrantInfo& info) const
{
	QString targetName;
	if (m_tenantCollectionCompPtr.IsValid() && !info.targetTenantId.isEmpty()){
		targetName = m_tenantCollectionCompPtr->GetElementInfo(info.targetTenantId, imtbase::ICollectionInfo::EIT_NAME).toString();
	}
	if (targetName.isEmpty()){
		targetName = QString::fromUtf8(info.targetTenantId);
	}

	QStringList roleNames;
	for (const QByteArray& roleId : info.roleIds){
		if (!roleId.isEmpty()){
			roleNames << QString::fromUtf8(roleId);
		}
	}

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

			// Enforce tenant isolation: grant must belong to the context tenant
			const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
			QByteArray contextTenantId;
			if (gqlContextPtr != nullptr){
				contextTenantId = gqlContextPtr->GetTenantId();
			}
			if (!contextTenantId.isEmpty()
					&& info.sourceTenantId != contextTenantId
					&& info.targetTenantId != contextTenantId){
				errorMessage = QStringLiteral("Access denied: grant does not belong to the current tenant");
				return sdl::V1_0::imtauth::CCrossOrgGrant();
			}

			sdl::V1_0::imtauth::CCrossOrgGrant response;
			response.id = info.grantId;
			response.sourceTenantId = info.sourceTenantId;
			response.targetTenantId = info.targetTenantId;
			response.name = info.name;
			response.roleIds.Emplace().FromList(info.roleIds);
			response.description = info.description;
			response.createdAt = info.createdAt;
			response.expiresAt = info.expiresAt;
			response.isActive = info.isActive;

			// Mark the grant as read-only when the current tenant is the grantee (target),
			// not the grantor (source). Only the source tenant may edit the grant.
			if (!contextTenantId.isEmpty()){
				response.isReadOnly = (info.sourceTenantId != contextTenantId);
			} else {
				response.isReadOnly = false;
			}

			// Resolve target tenant name
			if (m_tenantCollectionCompPtr.IsValid() && !info.targetTenantId.isEmpty()){
				response.targetTenantName = m_tenantCollectionCompPtr->GetElementInfo(info.targetTenantId, imtbase::ICollectionInfo::EIT_NAME).toString();
			}

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

	// Enforce tenant isolation: only the source (grantor) tenant may edit the grant.
	// The target (grantee) tenant can view it but must not modify it.
	if (!contextTenantId.isEmpty() && !info.grantId.isEmpty()){
		if (info.sourceTenantId != contextTenantId){
			errorMessage = QStringLiteral("Access denied: only the grantor tenant can modify this grant");
			return response;
		}
	}

	if (grantRepresentation.id){
		info.grantId = *grantRepresentation.id;
	}
	// The source tenant is the tenant currently acting (owner of the grant).
	// The client representation does not provide it, so prefer the request
	// context and never persist an empty source tenant id.
	if (grantRepresentation.sourceTenantId && !grantRepresentation.sourceTenantId->isEmpty()){
		info.sourceTenantId = *grantRepresentation.sourceTenantId;
	}
	if (info.sourceTenantId.isEmpty()){
		info.sourceTenantId = contextTenantId;
	}
	if (grantRepresentation.targetTenantId){
		info.targetTenantId = *grantRepresentation.targetTenantId;
	}
	if (grantRepresentation.name && !grantRepresentation.name->isEmpty()){
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

	// Auto-generate a human-readable name when one was not provided, based on the
	// target tenant name and the delegated roles.
	if (info.name.isEmpty()){
		info.name = ComposeDefaultGrantName(info);
	}

	grantDataPtr->SetGrantInfo(info);

	m_documentManagerCompPtr->SetDocumentData(userLogin, documentId, *documentPtr);
	response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::Success;

	return response;
}


} // namespace imtauthgql
