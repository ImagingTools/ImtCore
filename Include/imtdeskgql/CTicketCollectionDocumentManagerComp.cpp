// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdeskgql/CTicketCollectionDocumentManagerComp.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <iprm/CParamsSet.h>

// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QUrl>

// ImtCore includes
#include <imtdesk/ISupportTicket.h>
#include <imtchat/IChatMessage.h>
#include <imtchat/IChatService.h>
#include <imtchat/IAttachmentStorage.h>
#include <imtchat/IConversation.h>
#include <imtbase/IObjectCollectionIterator.h>
#include <imtdeskgql/imtdeskgql.h>
#include <imtdoc/CDocumentSavedEvent.h>


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
			response.Version_1_0->comments.Emplace();

			QList<sdl::imtdesk::ImtDesk::CTicketComment::V1_0> itemList;

			while (iteratorPtr->Next()){
				imtbase::IObjectCollection::DataPtr dataPtr;
				if (iteratorPtr->GetObjectData(dataPtr)){
					const imtchat::IChatMessage* msgPtr = dynamic_cast<const imtchat::IChatMessage*>(dataPtr.GetPtr());
					if (msgPtr != nullptr){
						sdl::imtdesk::ImtDesk::CTicketComment::V1_0 itemData;
						itemData.id = msgPtr->GetId();
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

						QByteArrayList attachmentIds = msgPtr->GetAttachmentIds();
						if (!attachmentIds.isEmpty()){
							itemData.attachments.Emplace();
							QStringList attachmentStrings;
							for (const QByteArray& aid : attachmentIds){
								// Resolve attachment ID to data URL via IAttachmentStorage
								if (m_attachmentStorageCompPtr.IsValid()){
									QByteArray data;
									QString fileName;
									QString mimeType;
									if (m_attachmentStorageCompPtr->GetAttachment(aid, data, fileName, mimeType)){
										QString dataUrl = QString("data:%1;base64,%2")
												.arg(mimeType)
												.arg(QString::fromLatin1(data.toBase64()));
										attachmentStrings << dataUrl;
										continue;
									}
								}
								// Fallback: pass the raw attachment ID
								attachmentStrings << QString::fromUtf8(aid);
							}
							itemData.attachments->FromList(attachmentStrings);
						}

						QStringList reactions = msgPtr->GetReactions();
						if (!reactions.isEmpty()){
							itemData.reactions.Emplace();
							itemData.reactions->FromList(reactions);
						}

						itemList << itemData;
					}
				}
			}

			response.Version_1_0->comments->FromList(itemList);

			if (response.Version_1_0->comments->isEmpty()){
				response.Version_1_0->comments.Reset();
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

	// Auto-create a Conversation if the ticket does not have one yet
	if (ticketPtr->GetConversationId().isEmpty() && m_chatServiceCompPtr.IsValid()){
		QByteArray convId = m_chatServiceCompPtr->CreateConversation(
					ticketPtr->GetTitle(),
					imtchat::IConversation::CT_SUPPORT,
					QByteArrayList());
		if (!convId.isEmpty()){
			ticketPtr->SetConversationId(convId);
		}
	}

	if (ticketInfo.comments){
		// Comments are saved as messages via IChatService
		// Messages are NOT part of the ticket document — they don't trigger document change
		QByteArray conversationId = ticketPtr->GetConversationId();
		if (!conversationId.isEmpty() && m_chatServiceCompPtr.IsValid()){
			for (const auto& sdlItem : *ticketInfo.comments){
				if (!sdlItem){
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

				QByteArrayList attachmentIds;
				if (sdlItem->attachments){
					for (const auto& att : *sdlItem->attachments){
						if (att){
							QString attStr = *att;
							// Check if the attachment is a data URL (base64-encoded binary)
							if (attStr.startsWith("data:") && m_attachmentStorageCompPtr.IsValid()){
								// Parse data URL: data:<mimeType>;base64,<data>
								const int dataPrefix = 5; // length of "data:"
								int semiIdx = attStr.indexOf(';');
								int commaIdx = attStr.indexOf(',');
								if (semiIdx > dataPrefix && commaIdx > semiIdx){
									QString mimeType = attStr.mid(dataPrefix, semiIdx - dataPrefix);
									QByteArray base64Data = attStr.mid(commaIdx + 1).toLatin1();
									QByteArray binaryData = QByteArray::fromBase64(base64Data);

									// Derive file extension from mime type
									int slashIdx = mimeType.indexOf('/');
									QString ext = (slashIdx >= 0) ? mimeType.mid(slashIdx + 1) : "bin";
									if (ext == "jpeg") ext = "jpg";
									if (ext == "svg+xml") ext = "svg";
									QString fileName = QString("attachment_%1.%2")
											.arg(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs).replace(':', '-'))
											.arg(ext);

									QByteArray storedId = m_attachmentStorageCompPtr->StoreAttachment(binaryData, fileName, mimeType);
									if (!storedId.isEmpty()){
										attachmentIds << storedId;
										continue;
									}
								}
							}
							// Check if it's a file:// URL (local file — store its data)
							if (attStr.startsWith("file://") && m_attachmentStorageCompPtr.IsValid()){
								QUrl fileUrl(attStr);
								QString localPath = fileUrl.toLocalFile();
								QFile file(localPath);
								if (file.open(QIODevice::ReadOnly)){
									QByteArray binaryData = file.readAll();
									file.close();
									QFileInfo fileInfo(localPath);
									QString fileName = fileInfo.fileName();
									// Determine MIME type from extension
									QString ext = fileInfo.suffix().toLower();
									QString mimeType = "application/octet-stream";
									if (ext == "png") mimeType = "image/png";
									else if (ext == "jpg" || ext == "jpeg") mimeType = "image/jpeg";
									else if (ext == "gif") mimeType = "image/gif";
									else if (ext == "bmp") mimeType = "image/bmp";
									else if (ext == "svg") mimeType = "image/svg+xml";
									else if (ext == "webp") mimeType = "image/webp";

									QByteArray storedId = m_attachmentStorageCompPtr->StoreAttachment(binaryData, fileName, mimeType);
									if (!storedId.isEmpty()){
										attachmentIds << storedId;
										continue;
									}
								}
							}
							// Fallback: treat as raw attachment ID
							attachmentIds << att->toUtf8();
						}
					}
				}

				m_chatServiceCompPtr->SendMessage(conversationId, senderId, content, QByteArrayList() /*entityReferences*/, attachmentIds);
			}
		}
	}

	m_documentManagerCompPtr->SetDocumentData(userId, documentId, *ticketPtr);

	// For new tickets (number == 0), do NOT auto-save — the user will save
	// manually via the Undo/Redo/Save commands panel in the editor.
	// For existing tickets (number > 0), save immediately after each change.
	if (ticketPtr->GetNumber() > 0){
		m_documentManagerCompPtr->SaveDocument(userId, documentId);
	}

	response.Version_1_0->status = sdl::imtbase::CollectionDocumentManager::EDocumentOperationStatus::Success;

	return response;
}


// reimplemented (imtdoc::IDocumentManagerEventHandler)

bool CTicketCollectionDocumentManagerComp::ProcessEvent(imtdoc::CEventBase* eventPtr)
{
	imtdoc::CDocumentSavedEvent* savedEventPtr = dynamic_cast<imtdoc::CDocumentSavedEvent*>(eventPtr);
	if (savedEventPtr != nullptr){
		QByteArray documentId = savedEventPtr->GetDocumentId();
		QByteArray userId = savedEventPtr->GetUserId();
		QByteArray objectId = savedEventPtr->GetDocumentUrl().toString().toUtf8().replace("collection:///", "");

		imtbase::IObjectCollection::DataPtr collectionDataPtr;
		const imtdesk::ISupportTicket* dbTicketPtr = nullptr;
		if (m_ticketCollectionCompPtr->GetObjectData(objectId, collectionDataPtr)){
			dbTicketPtr = dynamic_cast<const imtdesk::ISupportTicket*>(collectionDataPtr.GetPtr());
		}
	
		istd::IChangeableSharedPtr documentPtr;
		imtdesk::ISupportTicket* documentTicketPtr = nullptr;
		if (m_documentManagerCompPtr->GetDocumentData(userId, documentId, documentPtr) == imtdoc::IDocumentManager::OS_OK){
			documentTicketPtr = dynamic_cast<imtdesk::ISupportTicket*>(documentPtr.GetPtr());
		}

		if (dbTicketPtr != nullptr && documentTicketPtr != nullptr){
			if (documentTicketPtr->GetNumber() == 0){
				documentTicketPtr->SetId(dbTicketPtr->GetId());
				documentTicketPtr->SetNumber(dbTicketPtr->GetNumber());
				documentTicketPtr->SetUpdatedAt(dbTicketPtr->GetUpdatedAt());
				documentTicketPtr->SetCreatedAt(dbTicketPtr->GetCreatedAt());
	
				m_documentManagerCompPtr->SetDocumentData(userId, documentId, *documentPtr);
				m_documentManagerCompPtr->SaveDocument(userId, documentId);
			}
		}
	}

	return true;
}


} // namespace imtdeskgql


