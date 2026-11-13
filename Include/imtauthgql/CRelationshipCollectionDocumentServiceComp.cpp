// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CRelationshipCollectionDocumentServiceComp.h>


// ImtCore includes
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

			// A relationship is visible from inside one of its two organizations only.
			const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
			const QByteArray contextTenantId = (gqlContextPtr != nullptr) ? gqlContextPtr->GetTenantId() : QByteArray();
			if (contextTenantId.isEmpty() || (contextTenantId != sourceTenantId && contextTenantId != targetTenantId)){
				errorMessage = QStringLiteral("Access denied: the relationship does not belong to the current organization");
				return sdl::V1_0::imtauth::CTenantRelationship();
			}

			sdl::V1_0::imtauth::CTenantRelationship response;

			response.id = objectId;
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
			const sdl::V1_0::imtauth::CUpdateRelationshipFromRepresentationGqlRequest& /*updateRelationshipFromRepresentationRequest*/,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CDocumentOperationStatus response;
	response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::Failed;
	errorMessage = QStringLiteral("A relationship comes into force or changes only by a proposal approved by both organizations");
	response.message = errorMessage;

	return response;
}

} // namespace imtauthgql
