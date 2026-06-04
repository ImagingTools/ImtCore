// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CRelationshipCollectionDocumentServiceComp.h>


// Qt includes
#include <QUuid>

// ImtCore includes
#include <imtauth/ITenantInfo.h>
#include <imtgql/IGqlContext.h>

// Generated includes
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/RelationshipCollectionDocumentService.h>


namespace
{


sdl::V1_0::imtauth::TenantRelationshipRole ToSdlRelationshipRole(imtauth::ITenantInfo::TenantRelationshipRole role)
{
	switch (role){
	case imtauth::ITenantInfo::TRR_PARENT:
		return sdl::V1_0::imtauth::TenantRelationshipRole::Parent;
	case imtauth::ITenantInfo::TRR_CHILD:
		return sdl::V1_0::imtauth::TenantRelationshipRole::Child;
	case imtauth::ITenantInfo::TRR_SUPPLIER:
		return sdl::V1_0::imtauth::TenantRelationshipRole::Supplier;
	case imtauth::ITenantInfo::TRR_CUSTOMER:
		return sdl::V1_0::imtauth::TenantRelationshipRole::Customer;
	case imtauth::ITenantInfo::TRR_AFFILIATE:
		return sdl::V1_0::imtauth::TenantRelationshipRole::Affiliate;
	default:
		return sdl::V1_0::imtauth::TenantRelationshipRole::Partner;
	}
}


imtauth::ITenantInfo::TenantRelationshipRole FromSdlRelationshipRole(sdl::V1_0::imtauth::TenantRelationshipRole role)
{
	switch (role){
	case sdl::V1_0::imtauth::TenantRelationshipRole::Parent:
		return imtauth::ITenantInfo::TRR_PARENT;
	case sdl::V1_0::imtauth::TenantRelationshipRole::Child:
		return imtauth::ITenantInfo::TRR_CHILD;
	case sdl::V1_0::imtauth::TenantRelationshipRole::Supplier:
		return imtauth::ITenantInfo::TRR_SUPPLIER;
	case sdl::V1_0::imtauth::TenantRelationshipRole::Customer:
		return imtauth::ITenantInfo::TRR_CUSTOMER;
	case sdl::V1_0::imtauth::TenantRelationshipRole::Affiliate:
		return imtauth::ITenantInfo::TRR_AFFILIATE;
	default:
		return imtauth::ITenantInfo::TRR_PARTNER;
	}
}


sdl::V1_0::imtauth::RelationshipStatus ToSdlRelationshipStatus(imtauth::ITenantInfo::TenantRelationshipStatus status)
{
	switch (status){
	case imtauth::ITenantInfo::TRS_ARCHIVED:
		return sdl::V1_0::imtauth::RelationshipStatus::Archived;
	case imtauth::ITenantInfo::TRS_PENDING_APPROVED:
		return sdl::V1_0::imtauth::RelationshipStatus::PendingApproval;
	default:
		return sdl::V1_0::imtauth::RelationshipStatus::Active;
	}
}


} // anonymous namespace


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
	QByteArray userId = GetUserId(gqlRequest);

	QByteArray objectId;
	if (arguments.input && arguments.input->id){
		objectId = *arguments.input->id;
	}

	if (objectId.isEmpty()){
		errorMessage = QStringLiteral("Missing document ID");
		return sdl::V1_0::imtauth::CTenantRelationship();
	}

	istd::IChangeableSharedPtr documentPtr;
	m_documentManagerCompPtr->GetDocumentData(userId, objectId, documentPtr);
	if (!documentPtr.IsValid()){
		errorMessage = QStringLiteral("Document not found");
		return sdl::V1_0::imtauth::CTenantRelationship();
	}

	const imtauth::ITenantInfo* tenantPtr = dynamic_cast<const imtauth::ITenantInfo*>(documentPtr.GetPtr());
	if (tenantPtr == nullptr){
		errorMessage = QStringLiteral("Invalid document type");
		return sdl::V1_0::imtauth::CTenantRelationship();
	}

	// Find the relationship within the tenant's relationships list.
	// The objectId here is the relationship ID.
	QByteArray tenantId = tenantPtr->GetTenantId();
	imtauth::ITenantInfo::TenantRelationships relationships = tenantPtr->GetRelationships();

	for (const imtauth::ITenantInfo::TenantRelationship& rel : relationships){
		if (rel.relationshipId == objectId){
			sdl::V1_0::imtauth::CTenantRelationship response;
			response.id = rel.relationshipId;
			response.connectionId = rel.connectionId;
			response.sourceTenantId = rel.sourceTenantId;
			response.targetTenantId = rel.targetTenantId;
			response.sourceRole = ToSdlRelationshipRole(rel.sourceRole);
			response.targetRole = ToSdlRelationshipRole(rel.targetRole);
			response.scope = rel.scope;
			response.validFrom = rel.validFrom;
			response.validUntil = rel.validUntil;
			response.status = ToSdlRelationshipStatus(rel.status);
			response.description = rel.description;
			response.createdAt = rel.createdAt;
			response.updatedAt = rel.updatedAt;
			return response;
		}
	}

	// If not found in local tenant relationships, try via the connection request manager
	if (m_connectionRequestManagerCompPtr.IsValid()){
		imtauth::ITenantInfo::TenantRelationships allRelationships =
			m_connectionRequestManagerCompPtr->GetTenantRelationships(tenantId);
		for (const imtauth::ITenantInfo::TenantRelationship& rel : allRelationships){
			if (rel.relationshipId == objectId){
				sdl::V1_0::imtauth::CTenantRelationship response;
				response.id = rel.relationshipId;
				response.connectionId = rel.connectionId;
				response.sourceTenantId = rel.sourceTenantId;
				response.targetTenantId = rel.targetTenantId;
				response.sourceRole = ToSdlRelationshipRole(rel.sourceRole);
				response.targetRole = ToSdlRelationshipRole(rel.targetRole);
				response.scope = rel.scope;
				response.validFrom = rel.validFrom;
				response.validUntil = rel.validUntil;
				response.status = ToSdlRelationshipStatus(rel.status);
				response.description = rel.description;
				response.createdAt = rel.createdAt;
				response.updatedAt = rel.updatedAt;
				return response;
			}
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

	QByteArray userLogin = GetUserId(gqlRequest);

	istd::IChangeableSharedPtr documentPtr;
	m_documentManagerCompPtr->GetDocumentData(userLogin, documentId, documentPtr);
	if (!documentPtr.IsValid()){
		response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	imtauth::ITenantInfo* tenantPtr = dynamic_cast<imtauth::ITenantInfo*>(documentPtr.GetPtr());
	if (tenantPtr == nullptr){
		response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	sdl::V1_0::imtauth::CTenantRelationship relData;
	if (arguments.input->relationship){
		relData = *arguments.input->relationship;
	}

	// Build the domain relationship struct from the SDL data
	imtauth::ITenantInfo::TenantRelationship relationship;

	if (relData.id){
		relationship.relationshipId = *relData.id;
	}
	if (relData.connectionId){
		relationship.connectionId = *relData.connectionId;
	}
	if (relData.sourceTenantId){
		relationship.sourceTenantId = *relData.sourceTenantId;
	}
	if (relData.targetTenantId){
		relationship.targetTenantId = *relData.targetTenantId;
	}
	if (relData.sourceRole){
		relationship.sourceRole = FromSdlRelationshipRole(*relData.sourceRole);
	}
	if (relData.targetRole){
		relationship.targetRole = FromSdlRelationshipRole(*relData.targetRole);
	}
	if (relData.scope){
		relationship.scope = *relData.scope;
	}
	if (relData.validFrom){
		relationship.validFrom = *relData.validFrom;
	}
	if (relData.validUntil){
		relationship.validUntil = *relData.validUntil;
	}
	if (relData.description){
		relationship.description = *relData.description;
	}
	if (relData.createdAt){
		relationship.createdAt = *relData.createdAt;
	}
	if (relData.updatedAt){
		relationship.updatedAt = *relData.updatedAt;
	}

	// Find and update the relationship in the tenant's list
	QByteArray tenantId = tenantPtr->GetTenantId();
	imtauth::ITenantInfo::TenantRelationships relationships = tenantPtr->GetRelationships();
	bool found = false;

	for (int i = 0; i < relationships.size(); ++i){
		if (relationships[i].relationshipId == relationship.relationshipId){
			relationships[i] = relationship;
			found = true;
			break;
		}
	}

	if (!found){
		// If the relationship is new (not found), add it
		relationships.append(relationship);
	}

	tenantPtr->SetRelationships(relationships);

	m_documentManagerCompPtr->SetDocumentData(userLogin, documentId, *documentPtr);

	response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::Success;

	return response;
}


} // namespace imtauthgql
