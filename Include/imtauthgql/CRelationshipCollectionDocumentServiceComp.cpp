// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CRelationshipCollectionDocumentServiceComp.h>
#include <imtauthgql/imtauthgql.h>


// Qt includes
#include <QUuid>

// ImtCore includes
#include <imtauth/ITenantRelationshipInfo.h>
#include <imtgql/IGqlContext.h>

// Generated includes
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/RelationshipCollectionDocumentService.h>


namespace imtauthgql
{


// protected methods

// reimplemented (CRelationshipCollectionDocumentServiceGqlHandlerCompBase)

sdl::V1_0::imtauth::CTenantRelationship CRelationshipCollectionDocumentServiceComp::OnGetRelationshipRepresentation(
		const sdl::V1_0::imtauth::CGetRelationshipRepresentationGqlRequest& getRelationshipRepresentationRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	sdl::V1_0::imtauth::GetRelationshipRepresentationRequestArguments arguments = getRelationshipRepresentationRequest.GetRequestedArguments();
	Q_UNUSED(gqlRequest);

	QByteArray objectId;
	if (arguments.input && arguments.input->id){
		objectId = *arguments.input->id;
	}

	if (objectId.isEmpty()){
		errorMessage = QStringLiteral("Missing relationship ID");
		return sdl::V1_0::imtauth::CTenantRelationship();
	}

	if (!m_documentManagerCompPtr.IsValid()){
		errorMessage = QStringLiteral("Document service not configured");
		return sdl::V1_0::imtauth::CTenantRelationship();
	}

	QByteArray userLogin = GetUserId(gqlRequest);
	istd::IChangeableSharedPtr documentPtr;
	m_documentManagerCompPtr->GetDocumentData(userLogin, objectId, documentPtr);

	if (documentPtr.IsValid()){
		const imtauth::ITenantRelationshipInfo* relPtr = dynamic_cast<const imtauth::ITenantRelationshipInfo*>(documentPtr.GetPtr());
		if (relPtr != nullptr){
			sdl::V1_0::imtauth::CTenantRelationship response;
			response.id = relPtr->GetRelationshipId();
			response.connectionId = relPtr->GetConnectionId();
			response.sourceTenantId = relPtr->GetSourceTenantId();
			response.targetTenantId = relPtr->GetTargetTenantId();
			response.sourceRole = ToSdlRelationshipRole(relPtr->GetSourceRole());
			response.targetRole = ToSdlRelationshipRole(relPtr->GetTargetRole());
			response.scope = relPtr->GetScope();
			response.validFrom = relPtr->GetValidFrom();
			response.validUntil = relPtr->GetValidUntil();
			response.status = ToSdlRelationshipStatus(relPtr->GetStatus());
			response.description = relPtr->GetDescription();
			response.createdAt = relPtr->GetCreatedAt();
			response.updatedAt = relPtr->GetUpdatedAt();
			return response;
		}
	}

	errorMessage = QStringLiteral("Relationship not found");
	return sdl::V1_0::imtauth::CTenantRelationship();
}


sdl::V1_0::imtbase::CDocumentOperationStatus CRelationshipCollectionDocumentServiceComp::OnUpdateRelationshipFromRepresentation(
		const sdl::V1_0::imtauth::CUpdateRelationshipFromRepresentationGqlRequest& updateRelationshipFromRepresentationRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	sdl::V1_0::imtauth::UpdateRelationshipFromRepresentationRequestArguments arguments = updateRelationshipFromRepresentationRequest.GetRequestedArguments();
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

	// The update is handled via bilateral proposal mechanism
	// through the connection request manager, not direct modification
	if (!m_connectionRequestManagerCompPtr.IsValid()){
		errorMessage = QStringLiteral("Connection request manager not configured");
		return response;
	}

	// Extract tenant IDs from the relationship SDL input to create a proposal
	sdl::V1_0::imtauth::CTenantRelationship relData;
	if (arguments.input->relationship){
		relData = *arguments.input->relationship;
	}

	// Build a proposal info from the relationship data
	imtauth::ITenantRelationshipProposalInfoUniquePtr proposalInfo = m_proposalFactoryCompPtr.CreateInstance();
	if (!proposalInfo.IsValid()){
		errorMessage = QStringLiteral("Failed to create proposal info instance");
		return response;
	}

	if (relData.connectionId){
		proposalInfo->SetConnectionId(*relData.connectionId);
	}
	if (relData.sourceTenantId){
		proposalInfo->SetInitiatorTenantId(*relData.sourceTenantId);
	}
	if (relData.targetTenantId){
		proposalInfo->SetCounterpartyTenantId(*relData.targetTenantId);
	}
	if (relData.sourceRole){
		proposalInfo->SetProposedSourceRole(imtauthgql::FromSdlRelationshipRole(*relData.sourceRole));
	}
	if (relData.targetRole){
		proposalInfo->SetProposedTargetRole(imtauthgql::FromSdlRelationshipRole(*relData.targetRole));
	}
	if (relData.scope){
		proposalInfo->SetProposedScope(*relData.scope);
	}
	if (relData.description){
		proposalInfo->SetProposedDescription(*relData.description);
	}
	if (relData.validFrom){
		proposalInfo->SetProposedValidFrom(*relData.validFrom);
	}
	if (relData.validUntil){
		proposalInfo->SetProposedValidUntil(*relData.validUntil);
	}
	// Determine proposal type: if the document is a new/empty relationship, use RPT_CREATE;
	// if it has an existing relationship ID, use RPT_UPDATE
	const imtauth::ITenantRelationshipInfo* existingRelPtr = dynamic_cast<const imtauth::ITenantRelationshipInfo*>(documentPtr.GetPtr());
	bool isNewRelationship = (existingRelPtr == nullptr || existingRelPtr->GetRelationshipId().isEmpty());

	if (isNewRelationship){
		proposalInfo->SetProposalType(imtauth::ITenantRelationshipProposalInfo::RPT_CREATE);
	} else {
		proposalInfo->SetProposalType(imtauth::ITenantRelationshipProposalInfo::RPT_UPDATE);
		proposalInfo->SetExistingRelationshipId(existingRelPtr->GetRelationshipId());
	}

	QByteArray proposalId = m_connectionRequestManagerCompPtr->CreateRelationshipProposal(*proposalInfo);
	if (proposalId.isEmpty()){
		errorMessage = QStringLiteral("Failed to create relationship proposal");
		return response;
	}

	m_documentManagerCompPtr->SetDocumentData(userLogin, documentId, *documentPtr);
	response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::Success;

	return response;
}


} // namespace imtauthgql
