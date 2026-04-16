// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdeskgql/CTicketCollectionDocumentManagerComp.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <iprm/CParamsSet.h>

// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QFileInfo>
#include <QtCore/QUuid>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>

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

	// Load entity references by IDs from the EntityReferences table
	QByteArrayList entityRefIds = ticketPtr->GetEntityReferences();
	if (!entityRefIds.isEmpty() && m_databaseEngineCompPtr.IsValid()){
		response.Version_1_0->entityReferences.Emplace();
		QList<sdl::imtdesk::ImtDesk::CEntityReference::V1_0> refList;

		QString applicationId;
		if (m_applicationInfoCompPtr.IsValid()){
			applicationId = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID);
		}

		for (const QByteArray& refId : entityRefIds){
			QString escaped = QString::fromUtf8(refId);
			escaped.replace('\'', "''");
			QByteArray refQuery = QString(
				"SELECT \"EntityType\", \"EntityId\", \"DisplayName\", \"EntityUrl\" "
				"FROM \"EntityReferences\" WHERE \"Id\"='%1';")
				.arg(escaped).toUtf8();

			QSqlError sqlError;
			QSqlQuery sqlQuery = m_databaseEngineCompPtr->ExecSqlQuery(refQuery, &sqlError);
			if (sqlError.type() == QSqlError::NoError && sqlQuery.next()){
				QSqlRecord r = sqlQuery.record();
				sdl::imtdesk::ImtDesk::CEntityReference::V1_0 ref;
				ref.entityType = r.value("EntityType").toString();
				ref.entityId = r.value("EntityId").toString().toUtf8();
				ref.displayName = r.value("DisplayName").toString();

				// Build ObjectLink for navigation
				sdl::imtbase::ImtBaseTypes::CObjectLink::V1_0 entityLink;
				entityLink.id = ref.entityId;
				entityLink.typeId = ref.entityType ? QByteArray(ref.entityType->toUtf8()) : QByteArray();
				entityLink.name = ref.displayName ? *ref.displayName : QString();

				QString entityUrl = r.value("EntityUrl").toString();
				sdl::imtbase::ImtBaseTypes::CUrlParam::V1_0 urlParam;
				urlParam.scheme = QStringLiteral("applink");
				urlParam.host = applicationId;
				urlParam.path = entityUrl;
				entityLink.url = urlParam;

				ref.entityLink = entityLink;
				refList << ref;
			}
		}
		response.Version_1_0->entityReferences->FromList(refList);
		if (response.Version_1_0->entityReferences->isEmpty()){
			response.Version_1_0->entityReferences.Reset();
		}
	}

	// Populate available entity types from registered providers (I_MULTIREF)
	response.Version_1_0->entityTypes.Emplace();
	QList<sdl::imtdesk::ImtDesk::CEntityType::V1_0> typeList;
	for (int i = 0; i < m_entityTypeProvidersCompPtr.GetCount(); ++i){
		sdl::imtdesk::ImtDesk::CEntityType::V1_0 et;
		et.id = m_entityTypeProvidersCompPtr[i]->GetEntityTypeId();
		et.name = m_entityTypeProvidersCompPtr[i]->GetEntityTypeName();
		et.collectionId = m_entityTypeProvidersCompPtr[i]->GetCollectionId();
		typeList << et;
	}
	response.Version_1_0->entityTypes->FromList(typeList);

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
							QList<sdl::imtdesk::ImtDesk::CAttachment::V1_0> attachmentList;
							for (const QByteArray& aid : attachmentIds){
								sdl::imtdesk::ImtDesk::CAttachment::V1_0 att;
								att.id = aid;
								// Build HTTP download URL — aid is a pure UUID from the DB.
								// Append file extension so the GET endpoint returns the correct MIME type.
								QString httpUrl = QString("../../files/%1").arg(QString::fromUtf8(aid));
								if (m_attachmentStorageCompPtr.IsValid()){
									QByteArray data;
									QString fileName;
									QString mimeType;
									if (m_attachmentStorageCompPtr->GetAttachment(aid, data, fileName, mimeType)){
										att.fileName = fileName;
										att.mimeType = mimeType;
										QString ext = QFileInfo(fileName).suffix().toLower();
										if (!ext.isEmpty()){
											httpUrl = QString("../../files/%1.%2").arg(QString::fromUtf8(aid), ext);
										}
									}
								}
								att.preview = httpUrl;
								attachmentList << att;
							}
							itemData.attachments->FromList(attachmentList);
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

	// Write entity references directly to EntityReferences + TicketEntityReferences tables
	if (ticketInfo.entityReferences && m_databaseEngineCompPtr.IsValid()){
		QByteArrayList newRefIds;
		QString nowUtc = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
		for (const auto& ref : *ticketInfo.entityReferences){
			if (!ref){
				continue;
			}
			QString refId = QUuid::createUuid().toString(QUuid::WithoutBraces);

			auto sqlEscape = [](const QString& s) -> QString {
				QString escaped = s;
				escaped.replace('\'', "''");
				return escaped;
			};

			QString entityType = ref->entityType ? sqlEscape(*ref->entityType) : QString();
			QString entityId = ref->entityId ? sqlEscape(QString::fromUtf8(*ref->entityId)) : QString();
			QString displayName = ref->displayName ? sqlEscape(*ref->displayName) : QString();

			// Extract navigation path from ObjectLink if available
			QString entityUrl;
			if (ref->entityLink && ref->entityLink->url && ref->entityLink->url->path){
				entityUrl = sqlEscape(*ref->entityLink->url->path);
			}

			QByteArray insertQuery = QString(
				"INSERT INTO \"EntityReferences\" "
				"(\"Id\", \"EntityType\", \"EntityId\", \"DisplayName\", \"EntityUrl\", \"CreatedAt\") "
				"VALUES('%1', '%2', '%3', '%4', '%5', '%6');")
				.arg(refId, entityType, entityId, displayName, entityUrl, nowUtc).toUtf8();

			QSqlError sqlError;
			m_databaseEngineCompPtr->ExecSqlQuery(insertQuery, &sqlError);
			if (sqlError.type() == QSqlError::NoError){
				newRefIds << refId.toUtf8();
			}
		}
		ticketPtr->SetEntityReferences(newRefIds);
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
						if (!att){
							continue;
						}
						// Attachments are pre-uploaded via HTTP — the client sends
						// the attachment ID returned by the upload endpoint (uuid.ext).
						// Strip the file extension so the DB stores pure UUIDs
						// matching the Attachments table Id column.
						if (att->id && !att->id->isEmpty()){
							QByteArray attId = *att->id;
							int dotIdx = attId.lastIndexOf('.');
							if (dotIdx > 0){
								attId = attId.left(dotIdx);
							}
							attachmentIds << attId;
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


