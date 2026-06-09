// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CCrossOrgGrantCollectionDocumentServiceComp.h>


// ImtCore includes
#include <imtauth/ICrossOrgGrantData.h>
#include <imtgql/IGqlContext.h>
#include <imtbase/ICollectionInfo.h>

// Generated includes
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/CrossOrgGrantCollectionDocumentService.h>


namespace imtauthgql
{


// protected methods

// reimplemented (CCrossOrgGrantCollectionDocumentServiceGqlHandlerCompBase)

sdl::V1_0::imtauth::CCrossOrgGrant CCrossOrgGrantCollectionDocumentServiceComp::OnGetGrantRepresentation(
			const sdl::V1_0::imtauth::CGetGrantRepresentationGqlRequest& getGrantRepresentationRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::GetGrantRepresentationRequestArguments arguments = getGrantRepresentationRequest.GetRequestedArguments();
	Q_UNUSED(gqlRequest);

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
			sdl::V1_0::imtauth::CCrossOrgGrant response;
			response.id = info.grantId;
			response.sourceTenantId = info.sourceTenantId;
			response.targetTenantId = info.targetTenantId;
			response.roleIds.Emplace().FromList(info.roleIds);
			response.description = info.description;
			response.createdAt = info.createdAt;
			response.expiresAt = info.expiresAt;
			response.isActive = info.isActive;

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
	} else if (!info.isActive){
		info.isActive = true;
	}

	grantDataPtr->SetGrantInfo(info);

	m_documentManagerCompPtr->SetDocumentData(userLogin, documentId, *documentPtr);
	response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::Success;

	return response;
}


} // namespace imtauthgql
