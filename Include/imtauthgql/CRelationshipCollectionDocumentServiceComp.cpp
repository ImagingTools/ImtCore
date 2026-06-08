// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CRelationshipCollectionDocumentServiceComp.h>


// ImtCore includes
#include <imtauth/ITenantConnectionInfo.h>
#include <imtauth/ITenantRelationshipInfo.h>
#include <imtauthgql/imtauthgql.h>
#include <imtbase/ICollectionInfo.h>
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
			QByteArray sourceTenantId = relPtr->GetSourceTenantId();
			QByteArray targetTenantId = relPtr->GetTargetTenantId();

			sdl::V1_0::imtauth::CTenantRelationship response;
			response.id = relPtr->GetRelationshipId();
			response.targetTenantId = targetTenantId;
			response.sourceTenantId = sourceTenantId;
			response.connectionId = relPtr->GetConnectionId();
			response.sourceRole = ToSdlRelationshipRole(relPtr->GetSourceRole());
			response.targetRole = ToSdlRelationshipRole(relPtr->GetTargetRole());
			response.scope = relPtr->GetScope();
			response.validFrom = relPtr->GetValidFrom();
			response.validUntil = relPtr->GetValidUntil();
			response.status = ToSdlRelationshipStatus(relPtr->GetStatus());
			response.description = relPtr->GetDescription();
			response.createdAt = relPtr->GetCreatedAt();
			response.updatedAt = relPtr->GetUpdatedAt();

			// Resolve target tenant name
			if (m_tenantCollectionCompPtr.IsValid() && !targetTenantId.isEmpty()){
				QString tenantName = m_tenantCollectionCompPtr->GetElementInfo(targetTenantId, imtbase::ICollectionInfo::EIT_NAME).toString();
				if (!tenantName.isEmpty()){
					response.targetTenantName = tenantName;
				}
			}

			if (m_tenantCollectionCompPtr.IsValid() && !sourceTenantId.isEmpty()){
				QString tenantName = m_tenantCollectionCompPtr->GetElementInfo(sourceTenantId, imtbase::ICollectionInfo::EIT_NAME).toString();
				if (!tenantName.isEmpty()){
					response.sourceTenantName = tenantName;
				}
			}

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

	imtauth::ITenantRelationshipInfo* relationshipPtr = dynamic_cast<imtauth::ITenantRelationshipInfo*>(documentPtr.GetPtr());
	if (relationshipPtr == nullptr){
		response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	sdl::V1_0::imtauth::CTenantRelationship relData;
	if (arguments.input->relationship){
		relData = *arguments.input->relationship;
	} else {
		errorMessage = QStringLiteral("Missing relationship representation");
		return response;
	}

	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	QByteArray contextTenantId;
	if (gqlContextPtr != nullptr){
		contextTenantId = gqlContextPtr->GetTenantId();
	}

	QByteArray sourceTenantId = relationshipPtr->GetSourceTenantId();
	QByteArray targetTenantId = relationshipPtr->GetTargetTenantId();
	const bool hasPersistentPair = !sourceTenantId.isEmpty() && !targetTenantId.isEmpty();

	if (hasPersistentPair){
		if (!contextTenantId.isEmpty() && sourceTenantId != contextTenantId && targetTenantId != contextTenantId){
			errorMessage = QStringLiteral("Tenant context is not a participant of the relationship");
			return response;
		}

		if (relData.sourceTenantId && !relData.sourceTenantId->isEmpty() && *relData.sourceTenantId != sourceTenantId){
			errorMessage = QStringLiteral("Relationship partner cannot be changed after creation");
			return response;
		}

		if (relData.targetTenantId && !relData.targetTenantId->isEmpty() && *relData.targetTenantId != targetTenantId){
			errorMessage = QStringLiteral("Relationship partner cannot be changed after creation");
			return response;
		}
	} else {
		if (relData.sourceTenantId && !relData.sourceTenantId->isEmpty()){
			sourceTenantId = *relData.sourceTenantId;
		}
		if (relData.targetTenantId && !relData.targetTenantId->isEmpty()){
			targetTenantId = *relData.targetTenantId;
		}

		if (!contextTenantId.isEmpty()){
			if (sourceTenantId.isEmpty()){
				sourceTenantId = contextTenantId;
			}
			if (sourceTenantId == contextTenantId && targetTenantId.isEmpty() && relData.sourceTenantId && *relData.sourceTenantId != contextTenantId){
				targetTenantId = *relData.sourceTenantId;
			}
		}
	}

	if (!contextTenantId.isEmpty() && sourceTenantId != contextTenantId && targetTenantId != contextTenantId){
		errorMessage = QStringLiteral("Tenant context is not a participant of the relationship");
		return response;
	}

	if (!sourceTenantId.isEmpty() && sourceTenantId == targetTenantId){
		errorMessage = QStringLiteral("Source and target tenants must be different");
		return response;
	}

	if (sourceTenantId.isEmpty()){
		errorMessage = QStringLiteral("Missing source tenant ID");
		return response;
	}

	if (targetTenantId.isEmpty()){
		errorMessage = QStringLiteral("Missing target tenant ID");
		return response;
	}

	if (!m_connectionCollectionCompPtr.IsValid()){
		errorMessage = QStringLiteral("Connection collection is not configured");
		return response;
	}

	QByteArray requestedConnectionId;
	if (relData.connectionId){
		requestedConnectionId = *relData.connectionId;
	}

	QByteArray connectionId;
	for (const QByteArray& objectId : m_connectionCollectionCompPtr->GetElementIds()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!m_connectionCollectionCompPtr->GetObjectData(objectId, dataPtr)){
			continue;
		}

		const imtauth::ITenantConnectionInfo* connectionPtr = dynamic_cast<const imtauth::ITenantConnectionInfo*>(dataPtr.GetPtr());
		if (connectionPtr == nullptr){
			continue;
		}

		if (connectionPtr->GetStatus() != imtauth::ITenantConnectionInfo::CS_ACTIVE){
			continue;
		}

		const bool isDirectPair =
						(connectionPtr->GetTenantAId() == sourceTenantId && connectionPtr->GetTenantBId() == targetTenantId) ||
						(connectionPtr->GetTenantAId() == targetTenantId && connectionPtr->GetTenantBId() == sourceTenantId);
		if (!isDirectPair){
			continue;
		}

		QByteArray currentConnectionId = connectionPtr->GetConnectionId();
		if (currentConnectionId.isEmpty()){
			currentConnectionId = objectId;
		}

		if (!requestedConnectionId.isEmpty() && requestedConnectionId != currentConnectionId && requestedConnectionId != objectId){
			continue;
		}

		connectionId = currentConnectionId;
		break;
	}

	if (connectionId.isEmpty()){
		if (requestedConnectionId.isEmpty()){
			errorMessage = QStringLiteral("No active connection between tenants");
		} else {
			errorMessage = QStringLiteral("Invalid connectionId for the specified tenants");
		}
		return response;
	}

	if (relData.id){
		relationshipPtr->SetRelationshipId(*relData.id);
	}
	relationshipPtr->SetConnectionId(connectionId);
	relationshipPtr->SetSourceTenantId(sourceTenantId);
	relationshipPtr->SetTargetTenantId(targetTenantId);
	if (relData.sourceRole){
		relationshipPtr->SetSourceRole(imtauthgql::FromSdlRelationshipRole(*relData.sourceRole));
	}
	if (relData.targetRole){
		relationshipPtr->SetTargetRole(imtauthgql::FromSdlRelationshipRole(*relData.targetRole));
	}
	if (relData.scope){
		relationshipPtr->SetScope(*relData.scope);
	}
	if (relData.validFrom){
		relationshipPtr->SetValidFrom(*relData.validFrom);
	}
	if (relData.validUntil){
		relationshipPtr->SetValidUntil(*relData.validUntil);
	}
	if (relData.description){
		relationshipPtr->SetDescription(*relData.description);
	}
	if (relData.createdAt){
		relationshipPtr->SetCreatedAt(*relData.createdAt);
	}
	if (relData.updatedAt){
		relationshipPtr->SetUpdatedAt(*relData.updatedAt);
	}
	if (relData.status){
		switch (*relData.status){
		case sdl::V1_0::imtauth::RelationshipStatus::Archived:
			relationshipPtr->SetStatus(imtauth::ITenantRelationshipInfo::TRS_ARCHIVED);
			break;
		case sdl::V1_0::imtauth::RelationshipStatus::PendingApproval:
			relationshipPtr->SetStatus(imtauth::ITenantRelationshipInfo::TRS_PENDING_APPROVED);
			break;
		default:
			relationshipPtr->SetStatus(imtauth::ITenantRelationshipInfo::TRS_ACTIVE);
			break;
		}
	}

	m_documentManagerCompPtr->SetDocumentData(userLogin, documentId, *documentPtr);
	response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::Success;

	return response;
}


} // namespace imtauthgql
