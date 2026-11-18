// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlicgql/CProductCollectionDocumentServiceComp.h>


// ImtCore includes
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/CollectionDocumentService.h>
#include <GeneratedFiles/imtlicsdl/SDL/1.0/CPP/ProductCollectionDocumentService.h>
#include <imtdoc/imtdoc.h>
#include <imtlicgql/ProductRepresentation.h>


namespace imtlicgql
{


// protected methods

// reimplemented (imtservergql::CGqlRequestHandlerCompBase)

bool CProductCollectionDocumentServiceComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	if (!BaseClass::IsRequestSupported(gqlRequest)){
		return false;
	}

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		return false;
	}

	return inputParamPtr->GetParamArgumentValue("collectionId").toByteArray() == *m_collectionIdAttrPtr;
}


// reimplemented (sdl::V1_0::imtlic::CProductCollectionDocumentServiceGqlHandlerCompBase)

sdl::V1_0::imtlic::CProductData CProductCollectionDocumentServiceComp::OnGetProductRepresentation(
			const sdl::V1_0::imtlic::CGetProductRepresentationGqlRequest& getRepresentationRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	auto arguments = getRepresentationRequest.GetRequestedArguments();
	if (!arguments.input || !arguments.input->id){
		errorMessage = QStringLiteral("Unable to get the product. Error: Document-ID is missing");

		return sdl::V1_0::imtlic::CProductData();
	}

	const QByteArray userId = GetUserId(gqlRequest);
	const QByteArray documentId = *arguments.input->id;

	istd::IChangeableSharedPtr documentPtr;
	m_documentServiceCompPtr->GetDocumentData(userId, documentId, documentPtr);

	imtlic::IProductInfo* productInfoPtr = dynamic_cast<imtlic::IProductInfo*>(documentPtr.GetPtr());
	if (productInfoPtr == nullptr){
		errorMessage = QStringLiteral("Unable to get the product. Error: Document is not opened");

		return sdl::V1_0::imtlic::CProductData();
	}

	sdl::V1_0::imtlic::CProductData response;
	FillRepresentationFromProduct(*productInfoPtr, response);

	// The ID of the stored product, empty until the document is saved.
	response.id = imtdoc::GetDocumentObjectId(*m_documentServiceCompPtr, userId, documentId);

	return response;
}


sdl::V1_0::imtbase::CDocumentOperationStatus CProductCollectionDocumentServiceComp::OnUpdateProductFromRepresentation(
			const sdl::V1_0::imtlic::CUpdateProductFromRepresentationGqlRequest& updateRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtbase::CDocumentOperationStatus response;
	response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::Failed;

	auto arguments = updateRequest.GetRequestedArguments();
	if (!arguments.input || !arguments.input->documentId || !arguments.input->productData){
		return response;
	}

	const QByteArray userId = GetUserId(gqlRequest);
	const QByteArray documentId = *arguments.input->documentId;

	istd::IChangeableSharedPtr documentPtr;
	m_documentServiceCompPtr->GetDocumentData(userId, documentId, documentPtr);

	imtlic::IProductInfo* productInfoPtr = dynamic_cast<imtlic::IProductInfo*>(documentPtr.GetPtr());
	if (productInfoPtr == nullptr){
		response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::InvalidDocumentId;

		return response;
	}

	FillProductFromRepresentation(*arguments.input->productData, *m_featureCollectionCompPtr, *productInfoPtr);

	m_documentServiceCompPtr->SetDocumentData(userId, documentId, *productInfoPtr);

	response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::Success;

	return response;
}


} // namespace imtlicgql


