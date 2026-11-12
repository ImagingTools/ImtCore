// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CConnectedProductCollectionDocumentServiceComp.h>


// ImtCore includes
#include <imtauth/CConnectedProduct.h>
#include <imtauthgql/imtauthgql.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/ConnectedProductCollectionDocumentService.h>


namespace imtauthgql
{


// reimplemented (CConnectedProductCollectionDocumentServiceGqlHandlerCompBase)

sdl::V1_0::imtauth::CConnectedProductData CConnectedProductCollectionDocumentServiceComp::OnGetConnectedProductRepresentation(
			const sdl::V1_0::imtauth::CGetConnectedProductRepresentationGqlRequest& getRepresentationRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CConnectedProductData response;

	if (!IsSuperuserRequest(gqlRequest)){
		errorMessage = QStringLiteral("The catalog of products is available to a superuser only");

		return response;
	}

	const auto arguments = getRepresentationRequest.GetRequestedArguments();
	const QByteArray documentId = arguments.input && arguments.input->id ? QByteArray(*arguments.input->id) : QByteArray();

	istd::IChangeableSharedPtr documentPtr;
	m_documentManagerCompPtr->GetDocumentData(GetUserId(gqlRequest), documentId, documentPtr);
	const imtauth::CIdentifiableConnectedProduct* productPtr = dynamic_cast<const imtauth::CIdentifiableConnectedProduct*>(documentPtr.GetPtr());
	if (productPtr == nullptr){
		errorMessage = QStringLiteral("Document not found");

		return response;
	}

	response.id = productPtr->GetObjectUuid();
	response.productId = productPtr->GetProductId();
	response.name = productPtr->GetProductName();
	response.description = productPtr->GetProductDescription();

	return response;
}


sdl::V1_0::imtbase::CDocumentOperationStatus CConnectedProductCollectionDocumentServiceComp::OnUpdateConnectedProductFromRepresentation(
			const sdl::V1_0::imtauth::CUpdateConnectedProductFromRepresentationGqlRequest& updateRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CDocumentOperationStatus response;
	response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::Failed;

	if (!IsSuperuserRequest(gqlRequest)){
		errorMessage = QStringLiteral("The catalog of products is available to a superuser only");

		return response;
	}

	const auto arguments = updateRequest.GetRequestedArguments();
	if (!arguments.input.has_value() || !arguments.input->documentId){
		response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::InvalidDocumentId;

		return response;
	}

	const QByteArray documentId = *arguments.input->documentId;
	const QByteArray userLogin = GetUserId(gqlRequest);

	istd::IChangeableSharedPtr documentPtr;
	m_documentManagerCompPtr->GetDocumentData(userLogin, documentId, documentPtr);
	imtauth::CIdentifiableConnectedProduct* productPtr = dynamic_cast<imtauth::CIdentifiableConnectedProduct*>(documentPtr.GetPtr());
	if (productPtr == nullptr){
		response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::InvalidDocumentId;

		return response;
	}

	sdl::V1_0::imtauth::CConnectedProductData data;
	if (arguments.input->product){
		data = *arguments.input->product;
	}

	if (data.id && productPtr->GetObjectUuid().isEmpty()){
		productPtr->SetObjectUuid(*data.id);
	}
	if (data.productId){
		// The tokens, the roles and the permission tree refer to a stored product by this ID
		if (productPtr->GetProductId() != *data.productId && IsDocumentStored(userLogin, documentId)){
			errorMessage = QStringLiteral("The product ID cannot be changed");

			return response;
		}
		productPtr->SetProductId(*data.productId);
	}
	if (data.name){
		productPtr->SetProductName(*data.name);
	}
	if (data.description){
		productPtr->SetProductDescription(*data.description);
	}

	if (m_documentManagerCompPtr->SetDocumentData(userLogin, documentId, *documentPtr) != imtdoc::IDocumentService::OS_OK){
		errorMessage = QStringLiteral("Unable to update document data. Error: SetDocumentData failed");

		return response;
	}

	response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::Success;

	return response;
}


// private methods

bool CConnectedProductCollectionDocumentServiceComp::IsDocumentStored(const QByteArray& userId, const QByteArray& documentId) const
{
	const imtdoc::IDocumentService::DocumentList documents = m_documentManagerCompPtr->GetOpenedDocumentList(userId);
	for (const imtdoc::IDocumentService::DocumentListItem& document : documents){
		if (document.documentId == documentId){
			return !document.documentSourceRef.isEmpty();
		}
	}

	return true;
}


} // namespace imtauthgql
