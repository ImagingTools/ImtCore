// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtchatgql/CConversationCollectionDocumentServiceComp.h>


// ACF includes
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtchat/IConversation.h>


namespace imtchatgql
{


// protected methods

// reimplemented (CGraphQlHandlerCompBase)

sdl::imtchat::ImtChat::CConversationData CConversationCollectionDocumentServiceComp::OnGetConversationRepresentation(
			const sdl::imtchat::ConversationCollectionDocumentService::CGetConversationRepresentationGqlRequest& getConversationRepresentationRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtchat::ConversationCollectionDocumentService::GetConversationRepresentationRequestArguments arguments = getConversationRepresentationRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		Q_ASSERT(false);
		return sdl::imtchat::ImtChat::CConversationData();
	}

	QByteArray userId = GetUserId(gqlRequest);

	QByteArray objectId;
	istd::IChangeableSharedPtr documentPtr;
	if (arguments.input.Version_1_0->id){
		objectId = *arguments.input.Version_1_0->id;

		m_documentManagerCompPtr->GetDocumentData(userId, objectId, documentPtr);
	}

	if (!documentPtr.IsValid()){
		return sdl::imtchat::ImtChat::CConversationData();
	}

	imtchat::IConversation* convPtr = dynamic_cast<imtchat::IConversation*>(documentPtr.GetPtr());
	if (convPtr == nullptr){
		return sdl::imtchat::ImtChat::CConversationData();
	}

	sdl::imtchat::ImtChat::CConversationData response;
	response.Version_1_0.Emplace();

	response.Version_1_0->id = convPtr->GetId();
	response.Version_1_0->name = convPtr->GetName();
	response.Version_1_0->createdAt = convPtr->GetCreatedAt();
	response.Version_1_0->updatedAt = convPtr->GetUpdatedAt();
	response.Version_1_0->metadata = convPtr->GetMetadata();

	return response;
}


sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus CConversationCollectionDocumentServiceComp::OnUpdateConversationFromRepresentation(
			const sdl::imtchat::ConversationCollectionDocumentService::CUpdateConversationFromRepresentationGqlRequest& updateConversationFromRepresentationRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtchat::ConversationCollectionDocumentService::UpdateConversationFromRepresentationRequestArguments arguments = updateConversationFromRepresentationRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		Q_ASSERT(false);
		return sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus();
	}

	sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus response;
	response.Version_1_0.Emplace();
	response.Version_1_0->status = sdl::imtbase::CollectionDocumentService::EDocumentOperationStatus::Failed;

	QByteArray documentId;
	if (arguments.input.Version_1_0->documentId){
		documentId = *arguments.input.Version_1_0->documentId;
	}

	sdl::imtchat::ImtChat::CConversationData::V1_0 convInfo;
	if (arguments.input.Version_1_0->conversation){
		convInfo = *arguments.input.Version_1_0->conversation;
	}

	QByteArray userId = GetUserId(gqlRequest);

	istd::IChangeableSharedPtr documentPtr;
	m_documentManagerCompPtr->GetDocumentData(userId, documentId, documentPtr);
	if (!documentPtr.IsValid()){
		response.Version_1_0->status = sdl::imtbase::CollectionDocumentService::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	imtchat::IConversation* convPtr = dynamic_cast<imtchat::IConversation*>(documentPtr.GetPtr());
	if (convPtr == nullptr){
		response.Version_1_0->status = sdl::imtbase::CollectionDocumentService::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	istd::CChangeGroup changeGroup(convPtr);

	if (convInfo.name){
		convPtr->SetName(*convInfo.name);
	}

	if (convInfo.metadata){
		convPtr->SetMetadata(*convInfo.metadata);
	}

	if (convInfo.createdAt){
		convPtr->SetCreatedAt(*convInfo.createdAt);
	}

	if (convInfo.updatedAt){
		convPtr->SetUpdatedAt(*convInfo.updatedAt);
	}

	m_documentManagerCompPtr->SetDocumentData(userId, documentId, *convPtr);

	response.Version_1_0->status = sdl::imtbase::CollectionDocumentService::EDocumentOperationStatus::Success;

	return response;
}


} // namespace imtchatgql
