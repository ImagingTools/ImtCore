// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdeskgql/CTicketCollectionDocumentManagerComp.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtdesk/ISupportTicket.h>
#include <imtchat/IChatMessage.h>
#include <imtchat/IChatService.h>
#include <imtchat/IConversation.h>
#include <imtbase/IObjectCollectionIterator.h>
#include <imtdeskgql/imtdeskgql.h>


namespace imtdeskgql
{


// protected methods

// reimplemented (CGraphQlHandlerCompBase)

sdl::imtdesk::ImtDesk::CTicketData CTicketCollectionDocumentManagerComp::OnGetTicketRepresentation(
			const sdl::imtdesk::TicketCollectionDocumentManager::CGetTicketRepresentationGqlRequest& getTicketRepresentationRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtdesk::TicketCollectionDocumentManager::GetTicketRepresentationRequestArguments arguments = getTicketRepresentationRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		Q_ASSERT(false);
		return sdl::imtdesk::ImtDesk::CTicketData();
	}

	QByteArray userId = GetUserId(gqlRequest);

	QByteArray objectId;
	istd::IChangeableSharedPtr documentPtr;
	if (arguments.input.Version_1_0->id){
		objectId = *arguments.input.Version_1_0->id;

		m_documentManagerCompPtr->GetDocumentData(userId, objectId, documentPtr);
	}

	if (!documentPtr.IsValid()){
		return sdl::imtdesk::ImtDesk::CTicketData();
	}

	imtdesk::ISupportTicket* ticketPtr = dynamic_cast<imtdesk::ISupportTicket*>(documentPtr.GetPtr());
	if (ticketPtr == nullptr){
		return sdl::imtdesk::ImtDesk::CTicketData();
	}

	sdl::imtdesk::ImtDesk::CTicketData response;
	response.Version_1_0.Emplace();

	response.Version_1_0->id = ticketPtr->GetId();
	response.Version_1_0->number = ticketPtr->GetNumber();
	response.Version_1_0->title = ticketPtr->GetTitle();
	response.Version_1_0->description = ticketPtr->GetDescription();
	response.Version_1_0->assigneeIds.Emplace().FromList(ticketPtr->GetAssigneeIds());
	response.Version_1_0->reporterId = ticketPtr->GetReporterId();
	response.Version_1_0->conversationId = ticketPtr->GetConversationId();
	response.Version_1_0->messageId = ticketPtr->GetMessageId();
	response.Version_1_0->labelIds.Emplace().FromList(ticketPtr->GetLabelIds());
	response.Version_1_0->locked = ticketPtr->IsLocked();
	response.Version_1_0->lockReason = ticketPtr->GetLockReason();
	response.Version_1_0->createdAt = ticketPtr->GetCreatedAt();
	response.Version_1_0->updatedAt = ticketPtr->GetUpdatedAt();
	response.Version_1_0->closedAt = ticketPtr->GetClosedAt();
	response.Version_1_0->resolvedAt = ticketPtr->GetResolvedAt();
	response.Version_1_0->ticketType = imtdeskgql::GetSdlTypeFromTicketType(ticketPtr->GetTicketType());
	response.Version_1_0->priority = imtdeskgql::GetSdlTypeFromPriorityType(ticketPtr->GetPriority());
	response.Version_1_0->status = imtdeskgql::GetSdlTypeFromStatusType(ticketPtr->GetStatus());
	response.Version_1_0->stateReason = imtdeskgql::GetSdlTypeFromStateReason(ticketPtr->GetStateReason());

	// Load messages from the Messages collection filtered by ConversationId
	QByteArray conversationId = ticketPtr->GetConversationId();
	if (!conversationId.isEmpty() && m_messageCollectionCompPtr.IsValid()){
		CIdParam conversationIdParam(conversationId);

		iprm::CParamsSet paramsSet;
		paramsSet.SetEditableParameter("ConversationId", &conversationIdParam);

		istd::TDelPtr<imtbase::IObjectCollectionIterator> iteratorPtr(
					m_messageCollectionCompPtr->CreateObjectCollectionIterator(QByteArray(), 0, -1, &paramsSet));

		if (iteratorPtr.IsValid()){
			response.Version_1_0->activityItems.Emplace();

			QList<sdl::imtdesk::ImtDesk::CTicketActivityItem::V1_0> itemList;

			while (iteratorPtr->Next()){
				imtbase::IObjectCollection::DataPtr dataPtr;
				if (iteratorPtr->GetObjectData(dataPtr)){
					const imtchat::IChatMessage* msgPtr = dynamic_cast<const imtchat::IChatMessage*>(dataPtr.GetPtr());
					if (msgPtr != nullptr){
						sdl::imtdesk::ImtDesk::CTicketActivityItem::V1_0 itemData;
						itemData.id = msgPtr->GetId();
						itemData.itemType = sdl::imtdesk::ImtDesk::ActivityItemType::Comment;
						itemData.userId = msgPtr->GetSenderId();

						itemData.userName = "";
						imtbase::IObjectCollection::DataPtr dataPtr;
						if (m_userCollectionCompPtr.IsValid()){
							if (m_userCollectionCompPtr->GetObjectData(*itemData.userId, dataPtr)){
								const imtauth::IUserInfo* userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(dataPtr.GetPtr());
								if (userInfoPtr != nullptr){
									itemData.userName = userInfoPtr->GetName();
								}
							}
						}

						itemData.timestamp = msgPtr->GetCreatedAt();
						itemData.content = msgPtr->GetContent();

						QStringList reactions = msgPtr->GetReactions();
						if (!reactions.isEmpty()){
							itemData.reactions.Emplace();
							itemData.reactions->FromList(reactions);
						}

						itemList << itemData;
					}
				}
			}

			response.Version_1_0->activityItems->FromList(itemList);

			if (response.Version_1_0->activityItems->isEmpty()){
				response.Version_1_0->activityItems.Reset();
			}
		}
	}

	return response;
}


sdl::imtbase::CollectionDocumentManager::CDocumentOperationStatus CTicketCollectionDocumentManagerComp::OnUpdateTicketFromRepresentation(
			const sdl::imtdesk::TicketCollectionDocumentManager::CUpdateTicketFromRepresentationGqlRequest& updateTicketFromRepresentationRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtdesk::TicketCollectionDocumentManager::UpdateTicketFromRepresentationRequestArguments arguments = updateTicketFromRepresentationRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		Q_ASSERT(false);
		return sdl::imtbase::CollectionDocumentManager::CDocumentOperationStatus();
	}

	sdl::imtbase::CollectionDocumentManager::CDocumentOperationStatus response;
	response.Version_1_0.Emplace();
	response.Version_1_0->status = sdl::imtbase::CollectionDocumentManager::EDocumentOperationStatus::Failed;

	QByteArray documentId;
	if (arguments.input.Version_1_0->documentId){
		documentId = *arguments.input.Version_1_0->documentId;
	}

	sdl::imtdesk::ImtDesk::CTicketData::V1_0 ticketInfo;
	if (arguments.input.Version_1_0->ticket){
		ticketInfo = *arguments.input.Version_1_0->ticket;
	}

	QByteArray userId = GetUserId(gqlRequest);

	istd::IChangeableSharedPtr documentPtr;
	m_documentManagerCompPtr->GetDocumentData(userId, documentId, documentPtr);
	if (!documentPtr.IsValid()){
		response.Version_1_0->status = sdl::imtbase::CollectionDocumentManager::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	imtdesk::ISupportTicket* ticketPtr = dynamic_cast<imtdesk::ISupportTicket*>(documentPtr.GetPtr());
	if (ticketPtr == nullptr){
		response.Version_1_0->status = sdl::imtbase::CollectionDocumentManager::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	bool ticketChanged = false;

	if (ticketInfo.title){
		ticketPtr->SetTitle(*ticketInfo.title);
		ticketChanged = true;
	}

	if (ticketInfo.description){
		ticketPtr->SetDescription(*ticketInfo.description);
		ticketChanged = true;
	}

	if (ticketInfo.assigneeIds){
		ticketPtr->SetAssigneeIds(ticketInfo.assigneeIds->ToList());
		ticketChanged = true;
	}

	if (ticketInfo.reporterId){
		ticketPtr->SetReporterId(*ticketInfo.reporterId);
		ticketChanged = true;
	}

	if (ticketInfo.conversationId){
		ticketPtr->SetConversationId(*ticketInfo.conversationId);
		ticketChanged = true;
	}

	if (ticketInfo.messageId){
		ticketPtr->SetMessageId(*ticketInfo.messageId);
		ticketChanged = true;
	}

	if (ticketInfo.labelIds){
		ticketPtr->SetLabelIds(ticketInfo.labelIds->ToList());
		ticketChanged = true;
	}

	if (ticketInfo.locked){
		ticketPtr->SetLocked(*ticketInfo.locked);
		ticketChanged = true;
	}

	if (ticketInfo.lockReason){
		ticketPtr->SetLockReason(*ticketInfo.lockReason);
		ticketChanged = true;
	}

	if (ticketInfo.number){
		ticketPtr->SetNumber(*ticketInfo.number);
		ticketChanged = true;
	}

	if (ticketInfo.createdAt){
		ticketPtr->SetCreatedAt(*ticketInfo.createdAt);
		ticketChanged = true;
	}

	if (ticketInfo.updatedAt){
		ticketPtr->SetUpdatedAt(*ticketInfo.updatedAt);
		ticketChanged = true;
	}

	if (ticketInfo.resolvedAt){
		ticketPtr->SetResolvedAt(*ticketInfo.resolvedAt);
		ticketChanged = true;
	}

	if (ticketInfo.closedAt){
		ticketPtr->SetClosedAt(*ticketInfo.closedAt);
		ticketChanged = true;
	}

	if (ticketInfo.ticketType){
		ticketPtr->SetTicketType(imtdeskgql::GetTicketTypeFromSdlType(*ticketInfo.ticketType));
		ticketChanged = true;
	}

	if (ticketInfo.status){
		ticketPtr->SetStatus(imtdeskgql::GetStatusTypeFromSdlType(*ticketInfo.status));
		ticketChanged = true;
	}

	if (ticketInfo.stateReason){
		ticketPtr->SetStateReason(imtdeskgql::GetStateReasonFromSdlType(*ticketInfo.stateReason));
		ticketChanged = true;
	}

	if (ticketInfo.priority){
		ticketPtr->SetPriority(imtdeskgql::GetPriorityTypeFromSdlType(*ticketInfo.priority));
		ticketChanged = true;
	}

	// Auto-create a Conversation if the ticket does not have one yet
	if (ticketPtr->GetConversationId().isEmpty() && m_chatServiceCompPtr.IsValid()){
		QByteArray convId = m_chatServiceCompPtr->CreateConversation(
					ticketPtr->GetTitle(),
					imtchat::IConversation::CT_SUPPORT,
					QByteArrayList());
		if (!convId.isEmpty()){
			ticketPtr->SetConversationId(convId);
			ticketChanged = true;
		}
	}

	if (ticketInfo.activityItems){
		// Comment-type activity items are saved as messages via IChatService
		// Messages are NOT part of the ticket document — they don't trigger document change
		QByteArray conversationId = ticketPtr->GetConversationId();
		if (!conversationId.isEmpty() && m_chatServiceCompPtr.IsValid()){
			for (const auto& sdlItem : *ticketInfo.activityItems){
				if (!sdlItem){
					continue;
				}
				if (sdlItem->itemType != sdl::imtdesk::ImtDesk::ActivityItemType::Comment){
					continue;
				}
				// Skip items that already have an id — they are existing messages loaded from DB
				if (sdlItem->id && !sdlItem->id->isEmpty()){
					continue;
				}

				QString content;
				if (sdlItem->content){
					content = *sdlItem->content;
				}

				QByteArray senderId = userId;
				if (sdlItem->userId && !sdlItem->userId->isEmpty()){
					senderId = *sdlItem->userId;
				}

				m_chatServiceCompPtr->SendMessage(conversationId, senderId, content);
			}
		}
	}

	// Only save and notify if actual ticket fields changed.
	// Messages are saved via IChatService above and do not affect the ticket document.
	// This prevents false change notifications that cause infinite re-fetch loops
	// when multiple clients have the same ticket open.
	if (ticketChanged){
		m_documentManagerCompPtr->SetDocumentData(userId, documentId, *ticketPtr);
		m_documentManagerCompPtr->SaveDocument(userId, documentId);
	}

	response.Version_1_0->status = sdl::imtbase::CollectionDocumentManager::EDocumentOperationStatus::Success;

	return response;
}


} // namespace imtdeskgql
