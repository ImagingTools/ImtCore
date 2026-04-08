// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdeskgql/CTicketCollectionDocumentManagerComp.h>


// ACF includes
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtdesk/ISupportTicket.h>
#include <imtauth/CUserRecentAction.h>
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

	// Populate activity items from recent user actions (imtauth::CUserRecentAction)
	// Messages/comments come from the Conversation (via ConversationId) and are
	// merged at the QML/presentation level, not stored in the Ticket model.
	{
		const QList<imtauth::CUserRecentAction> actions = ticketPtr->GetRecentActions();
		for (const imtauth::CUserRecentAction& action : actions){
			sdl::imtdesk::ImtDesk::CTicketActivityItem sdlItem;
			sdlItem.Version_1_0.Emplace();
			sdlItem.Version_1_0->itemType = sdl::imtdesk::ImtDesk::ActivityItemType::Action;
			sdlItem.Version_1_0->userId = action.GetUserInfo().id;
			sdlItem.Version_1_0->userName = action.GetUserInfo().name;
			sdlItem.Version_1_0->timestamp = action.GetTimestamp().toString(Qt::ISODate);
			sdlItem.Version_1_0->actionType = action.GetActionTypeInfo().name;
			sdlItem.Version_1_0->actionDescription = action.GetActionTypeInfo().description;
			if (!response.Version_1_0->activityItems.has_value()){
				response.Version_1_0->activityItems.Emplace();
			}
			response.Version_1_0->activityItems->Append(sdlItem);
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

	istd::CChangeGroup changeGroup(ticketPtr);

	if (ticketInfo.title){
		ticketPtr->SetTitle(*ticketInfo.title);
	}

	if (ticketInfo.description){
		ticketPtr->SetDescription(*ticketInfo.description);
	}

	if (ticketInfo.assigneeIds){
		ticketPtr->SetAssigneeIds(ticketInfo.assigneeIds->ToList());
	}

	if (ticketInfo.reporterId){
		ticketPtr->SetReporterId(*ticketInfo.reporterId);
	}

	if (ticketInfo.conversationId){
		ticketPtr->SetConversationId(*ticketInfo.conversationId);
	}

	if (ticketInfo.messageId){
		ticketPtr->SetMessageId(*ticketInfo.messageId);
	}

	if (ticketInfo.labelIds){
		ticketPtr->SetLabelIds(ticketInfo.labelIds->ToList());
	}

	if (ticketInfo.locked){
		ticketPtr->SetLocked(*ticketInfo.locked);
	}

	if (ticketInfo.lockReason){
		ticketPtr->SetLockReason(*ticketInfo.lockReason);
	}

	if (ticketInfo.number){
		ticketPtr->SetNumber(*ticketInfo.number);
	}

	if (ticketInfo.createdAt){
		ticketPtr->SetCreatedAt(*ticketInfo.createdAt);
	}

	if (ticketInfo.updatedAt){
		ticketPtr->SetUpdatedAt(*ticketInfo.updatedAt);
	}

	if (ticketInfo.resolvedAt){
		ticketPtr->SetResolvedAt(*ticketInfo.resolvedAt);
	}

	if (ticketInfo.closedAt){
		ticketPtr->SetClosedAt(*ticketInfo.closedAt);
	}

	if (ticketInfo.ticketType){
		ticketPtr->SetTicketType(imtdeskgql::GetTicketTypeFromSdlType(*ticketInfo.ticketType));
	}

	if (ticketInfo.status){
		ticketPtr->SetStatus(imtdeskgql::GetStatusTypeFromSdlType(*ticketInfo.status));
	}

	if (ticketInfo.stateReason){
		ticketPtr->SetStateReason(imtdeskgql::GetStateReasonFromSdlType(*ticketInfo.stateReason));
	}

	if (ticketInfo.priority){
		ticketPtr->SetPriority(imtdeskgql::GetPriorityTypeFromSdlType(*ticketInfo.priority));
	}

	if (ticketInfo.activityItems){
		QList<imtauth::CUserRecentAction> actionItems;
		for (int i = 0; i < ticketInfo.activityItems->GetSize(); ++i){
			const sdl::imtdesk::ImtDesk::CTicketActivityItem& sdlItem = ticketInfo.activityItems->At(i);
			if (!sdlItem.Version_1_0.has_value()){
				continue;
			}
			// Only process Action items — comments are managed through the Conversation
			if (sdlItem.Version_1_0->itemType && *sdlItem.Version_1_0->itemType == sdl::imtdesk::ImtDesk::ActivityItemType::Action){
				imtauth::CUserRecentAction action;
				imtauth::IUserRecentAction::UserInfo userInfo;
				if (sdlItem.Version_1_0->userId){
					userInfo.id = *sdlItem.Version_1_0->userId;
				}
				if (sdlItem.Version_1_0->userName){
					userInfo.name = *sdlItem.Version_1_0->userName;
				}
				action.SetUserInfo(userInfo);
				if (sdlItem.Version_1_0->timestamp){
					action.SetTimestamp(QDateTime::fromString(*sdlItem.Version_1_0->timestamp, Qt::ISODate));
				}
				imtauth::IUserRecentAction::ActionTypeInfo actionTypeInfo;
				if (sdlItem.Version_1_0->actionType){
					actionTypeInfo.name = *sdlItem.Version_1_0->actionType;
				}
				if (sdlItem.Version_1_0->actionDescription){
					actionTypeInfo.description = *sdlItem.Version_1_0->actionDescription;
				}
				action.SetActionTypeInfo(actionTypeInfo);
				actionItems.append(action);
			}
		}
		ticketPtr->SetRecentActions(actionItems);
	}

	m_documentManagerCompPtr->SetDocumentData(userId, documentId, *ticketPtr);

	response.Version_1_0->status = sdl::imtbase::CollectionDocumentManager::EDocumentOperationStatus::Success;

	return response;
}


} // namespace imtdeskgql
