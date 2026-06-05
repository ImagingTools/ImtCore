// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CRelationshipCollectionDocumentServiceComp.h>


// Qt includes
#include <QUuid>

// ImtCore includes
#include <imtauth/ITenantRelationshipInfo.h>
#include <imtgql/IGqlContext.h>

// Generated includes
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/RelationshipCollectionDocumentService.h>


namespace
{


sdl::V1_0::imtauth::TenantRelationshipRole ToSdlRelationshipRole(imtauth::ITenantRelationshipInfo::TenantRelationshipRole role)
{
switch (role){
case imtauth::ITenantRelationshipInfo::TRR_PARENT:
return sdl::V1_0::imtauth::TenantRelationshipRole::Parent;
case imtauth::ITenantRelationshipInfo::TRR_CHILD:
return sdl::V1_0::imtauth::TenantRelationshipRole::Child;
case imtauth::ITenantRelationshipInfo::TRR_SUPPLIER:
return sdl::V1_0::imtauth::TenantRelationshipRole::Supplier;
case imtauth::ITenantRelationshipInfo::TRR_CUSTOMER:
return sdl::V1_0::imtauth::TenantRelationshipRole::Customer;
case imtauth::ITenantRelationshipInfo::TRR_AFFILIATE:
return sdl::V1_0::imtauth::TenantRelationshipRole::Affiliate;
default:
return sdl::V1_0::imtauth::TenantRelationshipRole::Partner;
}
}


sdl::V1_0::imtauth::RelationshipStatus ToSdlRelationshipStatus(imtauth::ITenantRelationshipInfo::TenantRelationshipStatus status)
{
switch (status){
case imtauth::ITenantRelationshipInfo::TRS_ARCHIVED:
return sdl::V1_0::imtauth::RelationshipStatus::Archived;
case imtauth::ITenantRelationshipInfo::TRS_PENDING_APPROVED:
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

QByteArray connectionId;
QByteArray initiatorTenantId;
QByteArray counterpartyTenantId;
if (relData.connectionId){
connectionId = *relData.connectionId;
}
if (relData.sourceTenantId){
initiatorTenantId = *relData.sourceTenantId;
}
if (relData.targetTenantId){
counterpartyTenantId = *relData.targetTenantId;
}

QByteArray proposalId = m_connectionRequestManagerCompPtr->CreateRelationshipProposal(
QByteArray(), connectionId, initiatorTenantId, counterpartyTenantId);
if (proposalId.isEmpty()){
errorMessage = QStringLiteral("Failed to create relationship proposal");
return response;
}

m_documentManagerCompPtr->SetDocumentData(userLogin, documentId, *documentPtr);
response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::Success;

return response;
}


} // namespace imtauthgql
