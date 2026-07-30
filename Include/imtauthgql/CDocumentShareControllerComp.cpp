// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CDocumentShareControllerComp.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/DocumentShares.h>


// Qt includes
#include <QtCore/QSet>


namespace imtauthgql
{


namespace
{


sdl::V1_0::imtauth::DocumentShareAccessMode ToSdlAccessMode(imtauth::DocumentShareAccessMode accessMode)
{
	switch (accessMode){
	case imtauth::DSAM_FULL_ACCESS:
		return sdl::V1_0::imtauth::DocumentShareAccessMode::FullAccess;
	default:
		return sdl::V1_0::imtauth::DocumentShareAccessMode::ReadOnly;
	}
}


imtauth::DocumentShareAccessMode FromSdlAccessMode(sdl::V1_0::imtauth::DocumentShareAccessMode accessMode)
{
	switch (accessMode){
	case sdl::V1_0::imtauth::DocumentShareAccessMode::FullAccess:
		return imtauth::DSAM_FULL_ACCESS;
	default:
		return imtauth::DSAM_READ_ONLY;
	}
}


sdl::V1_0::imtauth::DocumentShareStatus ToSdlShareStatus(imtauth::DocumentShareStatus status)
{
	switch (status){
	case imtauth::DSS_REVOKED:
		return sdl::V1_0::imtauth::DocumentShareStatus::Revoked;
	case imtauth::DSS_EXPIRED:
		return sdl::V1_0::imtauth::DocumentShareStatus::Expired;
	default:
		return sdl::V1_0::imtauth::DocumentShareStatus::Active;
	}
}


sdl::V1_0::imtauth::CDocumentShare ShareInfoToData(const imtauth::DocumentShareInfo& info)
{
	sdl::V1_0::imtauth::CDocumentShare data;
	data.id = info.shareId;
	data.sourceTenantId = info.sourceTenantId;
	data.targetTenantId = info.targetTenantId;
	data.relationshipId = info.relationshipId;
	data.documentId = info.documentId;
	data.documentType = info.documentType;
	data.documentName = info.documentName;
	data.accessMode = ToSdlAccessMode(info.accessMode);
	data.status = ToSdlShareStatus(info.status);
	data.sharedByUserId = info.sharedByUserId;
	data.createdAt = info.createdAt;
	data.updatedAt = info.updatedAt;
	data.expiresAt = info.expiresAt;
	data.revokedAt = info.revokedAt;
	return data;
}


} // anonymous namespace


// reimplemented (sdl::V1_0::imtauth::CDocumentSharesGqlHandlerCompBase)

sdl::V1_0::imtauth::CGetDocumentSharePayload CDocumentShareControllerComp::OnGetDocumentShare(
		const sdl::V1_0::imtauth::CGetDocumentShareGqlRequest& getDocumentShareRequest,
		const ::imtgql::CGqlRequest& /*gqlRequest*/,
		QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CGetDocumentSharePayload response;

	if (!m_shareManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Document share manager is not configured");
		return response;
	}

	QByteArray shareId;
	sdl::V1_0::imtauth::GetDocumentShareRequestArguments arguments = getDocumentShareRequest.GetRequestedArguments();
	if (arguments.input->shareId){
		shareId = *arguments.input->shareId;
	}

	if (shareId.isEmpty()){
		response.errorMessage = QStringLiteral("Share ID is required");
		return response;
	}

	imtauth::DocumentShareInfo info = m_shareManagerCompPtr->GetShare(shareId);
	if (info.shareId.isEmpty()){
		response.errorMessage = QStringLiteral("Document share not found");
		return response;
	}

	response.share = ShareInfoToData(info);

	return response;
}


sdl::V1_0::imtauth::CGetDocumentSharesPayload CDocumentShareControllerComp::OnGetDocumentShares(
		const sdl::V1_0::imtauth::CGetDocumentSharesGqlRequest& getDocumentSharesRequest,
		const ::imtgql::CGqlRequest& /*gqlRequest*/,
		QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CGetDocumentSharesPayload response;

	if (!m_shareManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Document share manager is not configured");
		return response;
	}

	QByteArray tenantId;
	QByteArray documentId;
	bool incomingOnly = false;
	bool outgoingOnly = false;

	sdl::V1_0::imtauth::GetDocumentSharesRequestArguments arguments = getDocumentSharesRequest.GetRequestedArguments();
	if (arguments.input->tenantId){
		tenantId = *arguments.input->tenantId;
	}
	if (arguments.input->documentId){
		documentId = *arguments.input->documentId;
	}
	if (arguments.input->direction){
		if (*arguments.input->direction == sdl::V1_0::imtauth::DocumentShareDirection::Incoming){
			incomingOnly = true;
		}
		else if (*arguments.input->direction == sdl::V1_0::imtauth::DocumentShareDirection::Outgoing){
			outgoingOnly = true;
		}
	}

	if (tenantId.isEmpty()){
		response.errorMessage = QStringLiteral("Tenant ID is required");
		return response;
	}

	response.shares.Emplace();

	QSet<QByteArray> seenIds;
	if (!outgoingOnly){
		const imtauth::DocumentShares incoming = m_shareManagerCompPtr->GetIncomingShares(tenantId);
		for (const imtauth::DocumentShareInfo& info : incoming){
			if (!documentId.isEmpty() && info.documentId != documentId){
				continue;
			}
			if (!seenIds.contains(info.shareId)){
				seenIds.insert(info.shareId);
				response.shares->push_back(ShareInfoToData(info));
			}
		}
	}
	if (!incomingOnly){
		const imtauth::DocumentShares outgoing = m_shareManagerCompPtr->GetOutgoingShares(tenantId);
		for (const imtauth::DocumentShareInfo& info : outgoing){
			if (!documentId.isEmpty() && info.documentId != documentId){
				continue;
			}
			if (!seenIds.contains(info.shareId)){
				seenIds.insert(info.shareId);
				response.shares->push_back(ShareInfoToData(info));
			}
		}
	}

	return response;
}


sdl::V1_0::imtauth::CGetDocumentAccessPayload CDocumentShareControllerComp::OnGetDocumentAccess(
		const sdl::V1_0::imtauth::CGetDocumentAccessGqlRequest& getDocumentAccessRequest,
		const ::imtgql::CGqlRequest& /*gqlRequest*/,
		QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CGetDocumentAccessPayload response;

	if (!m_shareManagerCompPtr.IsValid()){
		response.hasAccess = false;
		response.errorMessage = QStringLiteral("Document share manager is not configured");
		return response;
	}

	QByteArray tenantId;
	QByteArray documentId;

	sdl::V1_0::imtauth::GetDocumentAccessRequestArguments arguments = getDocumentAccessRequest.GetRequestedArguments();
	if (arguments.input->tenantId){
		tenantId = *arguments.input->tenantId;
	}
	if (arguments.input->documentId){
		documentId = *arguments.input->documentId;
	}

	if (tenantId.isEmpty() || documentId.isEmpty()){
		response.hasAccess = false;
		response.errorMessage = QStringLiteral("Tenant ID and document ID are required");
		return response;
	}

	imtauth::DocumentShareAccessMode accessMode = imtauth::DSAM_READ_ONLY;
	bool hasAccess = m_shareManagerCompPtr->GetDocumentAccess(tenantId, documentId, &accessMode);

	response.hasAccess = hasAccess;
	if (hasAccess){
		response.accessMode = ToSdlAccessMode(accessMode);
	}

	return response;
}


sdl::V1_0::imtauth::CShareDocumentPayload CDocumentShareControllerComp::OnShareDocument(
		const sdl::V1_0::imtauth::CShareDocumentGqlRequest& shareDocumentRequest,
		const ::imtgql::CGqlRequest& /*gqlRequest*/,
		QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CShareDocumentPayload response;

	if (!m_shareManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Document share manager is not configured");
		return response;
	}

	imtauth::DocumentShareInfo info;

	sdl::V1_0::imtauth::ShareDocumentRequestArguments arguments = shareDocumentRequest.GetRequestedArguments();
	if (arguments.input->sourceTenantId){
		info.sourceTenantId = *arguments.input->sourceTenantId;
	}
	if (arguments.input->targetTenantId){
		info.targetTenantId = *arguments.input->targetTenantId;
	}
	if (arguments.input->relationshipId){
		info.relationshipId = *arguments.input->relationshipId;
	}
	if (arguments.input->documentId){
		info.documentId = *arguments.input->documentId;
	}
	if (arguments.input->documentType){
		info.documentType = *arguments.input->documentType;
	}
	if (arguments.input->documentName){
		info.documentName = *arguments.input->documentName;
	}
	if (arguments.input->accessMode){
		info.accessMode = FromSdlAccessMode(*arguments.input->accessMode);
	}
	if (arguments.input->expiresAt){
		info.expiresAt = *arguments.input->expiresAt;
	}

	QByteArray shareId = m_shareManagerCompPtr->CreateShare(info);
	if (shareId.isEmpty()){
		response.errorMessage = QStringLiteral("Failed to share document");
		return response;
	}

	response.shareId = shareId;

	return response;
}


sdl::V1_0::imtauth::CSetDocumentShareAccessModePayload CDocumentShareControllerComp::OnSetDocumentShareAccessMode(
		const sdl::V1_0::imtauth::CSetDocumentShareAccessModeGqlRequest& setDocumentShareAccessModeRequest,
		const ::imtgql::CGqlRequest& /*gqlRequest*/,
		QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CSetDocumentShareAccessModePayload response;

	if (!m_shareManagerCompPtr.IsValid()){
		response.success = false;
		response.errorMessage = QStringLiteral("Document share manager is not configured");
		return response;
	}

	QByteArray shareId;
	imtauth::DocumentShareAccessMode accessMode = imtauth::DSAM_READ_ONLY;

	sdl::V1_0::imtauth::SetDocumentShareAccessModeRequestArguments arguments = setDocumentShareAccessModeRequest.GetRequestedArguments();
	if (arguments.input->shareId){
		shareId = *arguments.input->shareId;
	}
	if (arguments.input->accessMode){
		accessMode = FromSdlAccessMode(*arguments.input->accessMode);
	}

	if (shareId.isEmpty()){
		response.success = false;
		response.errorMessage = QStringLiteral("Share ID is required");
		return response;
	}

	bool success = m_shareManagerCompPtr->SetAccessMode(shareId, accessMode);
	response.success = success;
	if (!success){
		response.errorMessage = QStringLiteral("Failed to update the access mode of the document share");
	}

	return response;
}


sdl::V1_0::imtauth::CRevokeDocumentSharePayload CDocumentShareControllerComp::OnRevokeDocumentShare(
		const sdl::V1_0::imtauth::CRevokeDocumentShareGqlRequest& revokeDocumentShareRequest,
		const ::imtgql::CGqlRequest& /*gqlRequest*/,
		QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CRevokeDocumentSharePayload response;

	if (!m_shareManagerCompPtr.IsValid()){
		response.success = false;
		response.errorMessage = QStringLiteral("Document share manager is not configured");
		return response;
	}

	QByteArray shareId;

	sdl::V1_0::imtauth::RevokeDocumentShareRequestArguments arguments = revokeDocumentShareRequest.GetRequestedArguments();
	if (arguments.input->shareId){
		shareId = *arguments.input->shareId;
	}

	if (shareId.isEmpty()){
		response.success = false;
		response.errorMessage = QStringLiteral("Share ID is required");
		return response;
	}

	bool success = m_shareManagerCompPtr->RevokeShare(shareId);
	response.success = success;
	if (!success){
		response.errorMessage = QStringLiteral("Failed to revoke the document share");
	}

	return response;
}


} // namespace imtauthgql
