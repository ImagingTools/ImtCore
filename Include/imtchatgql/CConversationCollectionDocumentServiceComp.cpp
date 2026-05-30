// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtchatgql/CConversationCollectionDocumentServiceComp.h>
#include <imtbasesdl/SDL/1.0/CPP/CollectionDocumentService.h>
#include <GeneratedFiles/imtchatsdl/SDL/1.0/CPP/ConversationCollectionDocumentService.h>


// ACF includes
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtchat/IConversation.h>


namespace imtchatgql
{


// protected methods

// reimplemented (CConversationCollectionDocumentServiceGqlHandlerCompBase)

sdl::V1_0::imtchat::CConversationData CConversationCollectionDocumentServiceComp::OnGetConversationRepresentation(
			const sdl::V1_0::imtchat::CGetConversationRepresentationGqlRequest& getConversationRepresentationRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtchat::GetConversationRepresentationRequestArguments arguments = getConversationRepresentationRequest.GetRequestedArguments();
	QByteArray userId = GetUserId(gqlRequest);

	QByteArray objectId;
	istd::IChangeableSharedPtr documentPtr;
	if (arguments.input.id){
		objectId = *arguments.input.id;

		m_documentManagerCompPtr->GetDocumentData(userId, objectId, documentPtr);
	}

	if (!documentPtr.IsValid()){
		return sdl::V1_0::imtchat::CConversationData();
	}

	imtchat::IConversation* convPtr = dynamic_cast<imtchat::IConversation*>(documentPtr.GetPtr());
	if (convPtr == nullptr){
		return sdl::V1_0::imtchat::CConversationData();
	}

	sdl::V1_0::imtchat::CConversationData response;
	response.id = convPtr->GetId();
	response.name = convPtr->GetName();
	response.createdAt = convPtr->GetCreatedAt();
	response.updatedAt = convPtr->GetUpdatedAt();
	response.metadata = convPtr->GetMetadata();

	return response;
}


sdl::V1_0::imtbase::CDocumentOperationStatus CConversationCollectionDocumentServiceComp::OnUpdateConversationFromRepresentation(
			const sdl::V1_0::imtchat::CUpdateConversationFromRepresentationGqlRequest& updateConversationFromRepresentationRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtchat::UpdateConversationFromRepresentationRequestArguments arguments = updateConversationFromRepresentationRequest.GetRequestedArguments();
	sdl::V1_0::imtbase::CDocumentOperationStatus response;
	response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::Failed;

	QByteArray documentId;
	if (arguments.input.documentId){
		documentId = *arguments.input.documentId;
	}

	sdl::V1_0::imtchat::CConversationData convInfo;
	if (arguments.input.conversation){
		convInfo = *arguments.input.conversation;
	}

	QByteArray userId = GetUserId(gqlRequest);

	istd::IChangeableSharedPtr documentPtr;
	m_documentManagerCompPtr->GetDocumentData(userId, documentId, documentPtr);
	if (!documentPtr.IsValid()){
		response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	imtchat::IConversation* convPtr = dynamic_cast<imtchat::IConversation*>(documentPtr.GetPtr());
	if (convPtr == nullptr){
		response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::InvalidDocumentId;
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

	response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::Success;

	return response;
}


} // namespace imtchatgql
