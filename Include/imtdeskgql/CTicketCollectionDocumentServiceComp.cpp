// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdeskgql/CTicketCollectionDocumentServiceComp.h>
#include <GeneratedFiles/imtdesksdl/SDL/1.0/CPP/TicketCollectionDocumentService.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <iprm/CParamsSet.h>

// Qt includes
#include <QtCore/QFileInfo>
#include <QtCore/QUuid>

// ImtCore includes
#include <imtdesk/ISupportTicket.h>
#include <imtchat/IChatMessage.h>
#include <imtchat/IChatService.h>
#include <imtchat/IAttachmentStorage.h>
#include <imtchat/IConversation.h>
#include <imtbase/IObjectCollectionIterator.h>
#include <imtbase/CCollectionFilter.h>
#include <imtdeskgql/imtdeskgql.h>
#include <imtdeskgql/TicketPermissions.h>
#include <imtdoc/CDocumentSavedEvent.h>
#include <imtauth/IUserGroupInfoProvider.h>
#include <imtgql/IGqlContext.h>


namespace imtdeskgql
{


// protected methods

// reimplemented (CTicketCollectionDocumentServiceGqlHandlerCompBase)

sdl::V1_0::imtdesk::CTicketData CTicketCollectionDocumentServiceComp::OnGetTicketRepresentation(
			const sdl::V1_0::imtdesk::CGetTicketRepresentationGqlRequest& getTicketRepresentationRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtdesk::GetTicketRepresentationRequestArguments arguments = getTicketRepresentationRequest.GetRequestedArguments();
	QByteArray userId = GetUserId(gqlRequest);

	QByteArray objectId;
	istd::IChangeableSharedPtr documentPtr;
	if (arguments.input->id){
		objectId = *arguments.input->id;

		m_documentManagerCompPtr->GetDocumentData(userId, objectId, documentPtr);
	}

	if (!documentPtr.IsValid()){
		return sdl::V1_0::imtdesk::CTicketData();
	}

	imtdesk::ISupportTicket* ticketPtr = dynamic_cast<imtdesk::ISupportTicket*>(documentPtr.GetPtr());
	if (ticketPtr == nullptr){
		return sdl::V1_0::imtdesk::CTicketData();
	}

	sdl::V1_0::imtdesk::CTicketData response;
	response.id = ticketPtr->GetId();
	response.number = ticketPtr->GetNumber();
	response.title = ticketPtr->GetTitle();
	response.description = ticketPtr->GetDescription();
	response.assigneeIds.Emplace().FromList(ticketPtr->GetAssigneeIds());
	response.reporterId = ticketPtr->GetReporterId();
	response.conversationId = ticketPtr->GetConversationId();
	response.messageId = ticketPtr->GetMessageId();
	response.labelIds.Emplace().FromList(ticketPtr->GetLabelIds());
	response.locked = ticketPtr->IsLocked();
	response.lockReason = ticketPtr->GetLockReason();
	response.createdAt = ticketPtr->GetCreatedAt();
	response.updatedAt = ticketPtr->GetUpdatedAt();
	response.closedAt = ticketPtr->GetClosedAt();
	response.resolvedAt = ticketPtr->GetResolvedAt();
	response.ticketType = imtdeskgql::GetSdlTypeFromTicketType(ticketPtr->GetTicketType());
	response.priority = imtdeskgql::GetSdlTypeFromPriorityType(ticketPtr->GetPriority());
	response.status = imtdeskgql::GetSdlTypeFromStatusType(ticketPtr->GetStatus());
	response.stateReason = imtdeskgql::GetSdlTypeFromStateReason(ticketPtr->GetStateReason());

	// Permissions: visibility (admin / reporter / assignee / same-group as reporter).
	if (!HasTicketVisibility(gqlRequest.GetRequestContext(), ticketPtr, m_userCollectionCompPtr.GetPtr(), m_userGroupInfoProviderCompPtr.GetPtr())){
		errorMessage = QStringLiteral("Permission denied: you do not have access to this ticket");
		return sdl::V1_0::imtdesk::CTicketData();
	}

	response.accessLevel = GetTicketAccessLevel(
			gqlRequest.GetRequestContext(), ticketPtr, m_userCollectionCompPtr.GetPtr(), m_userGroupInfoProviderCompPtr.GetPtr());

	// Load entity references by IDs from the EntityReferences table via IEntityReferenceStorage
	QByteArrayList entityRefIds = ticketPtr->GetEntityReferences();
	if (!entityRefIds.isEmpty() && m_entityReferenceStorageCompPtr.IsValid()){
		response.entityReferences.Emplace();
		QList<sdl::V1_0::imtdesk::CEntityReference> refList;

		QString applicationId;
		if (m_applicationInfoCompPtr.IsValid()){
			applicationId = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID);
		}

		for (const QByteArray& refId : std::as_const(entityRefIds)){
			QString entityType;
			QByteArray entityId;
			QString displayName;
			QString entityUrl;

			if (!m_entityReferenceStorageCompPtr->GetEntityReference(refId, entityType, entityId, displayName, entityUrl)){
				continue;
			}

			sdl::V1_0::imtdesk::CEntityReference ref;
			ref.entityType = entityType;
			ref.entityId = entityId;
			ref.displayName = displayName;

			// Build ObjectLink for navigation
			sdl::V1_0::imtbase::CObjectLink entityLink;
			entityLink.id = ref.entityId;
			entityLink.name = displayName;

			// Extract typeId from path (format: collectionId/typeId/entityId)
			QStringList pathParts = entityUrl.split('/');
			if (pathParts.size() >= 2){
				entityLink.typeId = pathParts[1].toUtf8();
			} else {
				entityLink.typeId = entityType.toUtf8();
			}

			sdl::V1_0::imtbase::CUrlParam urlParam;
			urlParam.scheme = QStringLiteral("applink");
			urlParam.host = applicationId;
			urlParam.path = entityUrl;
			entityLink.url = urlParam;

			ref.entityLink = entityLink;
			refList << ref;
		}
		response.entityReferences->FromList(refList);
		if (response.entityReferences->isEmpty()){
			response.entityReferences.Reset();
		}
	}

	if (m_entityTypeProvidersCompPtr.IsValid()){
		// Populate available entity types from registered providers (I_MULTIREF)
		// EntityTypeId also serves as the collectionId for browsing entities
		response.entityTypes.Emplace();
		QList<sdl::V1_0::imtdesk::CEntityType> typeList;
		for (int i = 0; i < m_entityTypeProvidersCompPtr.GetCount(); ++i){
			sdl::V1_0::imtdesk::CEntityType et;
			et.id = m_entityTypeProvidersCompPtr[i]->GetEntityTypeId();
			et.name = m_entityTypeProvidersCompPtr[i]->GetEntityTypeName();
			typeList << et;
		}
		response.entityTypes->FromList(typeList);
	}

	// Load messages from the Messages collection filtered by ConversationId
	QByteArray conversationId = ticketPtr->GetConversationId();
	if (!conversationId.isEmpty() && m_messageCollectionCompPtr.IsValid()){
		CIdParam conversationIdParam(conversationId);

		imtbase::CCollectionFilter sortFilter;
		sortFilter.SetSortingOrder(imtbase::ICollectionFilter::SO_ASC);
		sortFilter.SetSortingInfoIds(QByteArrayList() << "CreatedAt");

		iprm::CParamsSet paramsSet;
		paramsSet.SetEditableParameter("ConversationId", &conversationIdParam);
		paramsSet.SetEditableParameter("Filter", &sortFilter);

		istd::TDelPtr<imtbase::IObjectCollectionIterator> iteratorPtr(
					m_messageCollectionCompPtr->CreateObjectCollectionIterator(QByteArray(), 0, -1, &paramsSet));

		if (iteratorPtr.IsValid()){
			response.comments.Emplace();

			QList<sdl::V1_0::imtdesk::CTicketComment> itemList;

			while (iteratorPtr->Next()){
				imtbase::IObjectCollection::DataPtr dataPtr;
				if (iteratorPtr->GetObjectData(dataPtr)){
					const imtchat::IChatMessage* msgPtr = dynamic_cast<const imtchat::IChatMessage*>(dataPtr.GetPtr());
					if (msgPtr != nullptr){
						sdl::V1_0::imtdesk::CTicketComment itemData;
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

						// Mark the message as edited when its updatedAt timestamp
						// was set by IChatService::EditMessage (SendMessage does
						// not populate updatedAt, so a non-empty value reliably
						// indicates a subsequent edit).
						QString updatedAt = msgPtr->GetUpdatedAt();
						if (!updatedAt.isEmpty() && updatedAt != msgPtr->GetCreatedAt()){
							itemData.edited = true;
						}

						QByteArrayList attachmentIds = msgPtr->GetAttachmentIds();
						if (!attachmentIds.isEmpty()){
							itemData.attachments.Emplace();
							QList<sdl::V1_0::imtdesk::CAttachment> attachmentList;
							for (const QByteArray& aid : std::as_const(attachmentIds)){
								sdl::V1_0::imtdesk::CAttachment att;
								att.id = aid;
								// Build HTTP download URL — aid is a pure UUID from the DB.
								// Append file extension so the GET endpoint returns the correct MIME type.
								QString httpUrl = QStringLiteral("../../files/%1").arg(aid);
								if (m_attachmentStorageCompPtr.IsValid()){
									QByteArray data;
									QString fileName;
									QString mimeType;
									if (m_attachmentStorageCompPtr->GetAttachment(aid, data, fileName, mimeType)){
										att.fileName = fileName;
										att.mimeType = mimeType;
										QString ext = QFileInfo(fileName).suffix().toLower();
										if (!ext.isEmpty()){
											httpUrl = QStringLiteral("../../files/%1.%2").arg(QString::fromUtf8(aid), ext);
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

						// Reply-to context
						QByteArray replyToId = msgPtr->GetReplyToId();
						if (!replyToId.isEmpty()){
							itemData.replyToId = replyToId;
							// Resolve reply-to content and user name from the referenced message
							imtbase::IObjectCollection::DataPtr replyDataPtr;
							if (m_messageCollectionCompPtr.IsValid() && m_messageCollectionCompPtr->GetObjectData(replyToId, replyDataPtr)){
								const imtchat::IChatMessage* replyMsgPtr = dynamic_cast<const imtchat::IChatMessage*>(replyDataPtr.GetPtr());
								if (replyMsgPtr != nullptr){
									itemData.replyToContent = replyMsgPtr->GetContent();
									QByteArray replySenderId = replyMsgPtr->GetSenderId();
									if (m_userCollectionCompPtr.IsValid()){
										imtbase::IObjectCollection::DataPtr replyUserDataPtr;
										if (m_userCollectionCompPtr->GetObjectData(replySenderId, replyUserDataPtr)){
											const imtauth::IUserInfo* replyUserInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(replyUserDataPtr.GetPtr());
											if (replyUserInfoPtr != nullptr){
												itemData.replyToUserName = replyUserInfoPtr->GetName();
											}
										}
									}
								}
							}
						}

						itemList << itemData;
					}
				}
			}

			response.comments->FromList(itemList);

			if (response.comments->isEmpty()){
				response.comments.Reset();
			}
		}
	}

	return response;
}


sdl::V1_0::imtbase::CDocumentOperationStatus CTicketCollectionDocumentServiceComp::OnUpdateTicketFromRepresentation(
			const sdl::V1_0::imtdesk::CUpdateTicketFromRepresentationGqlRequest& updateTicketFromRepresentationRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtdesk::UpdateTicketFromRepresentationRequestArguments arguments = updateTicketFromRepresentationRequest.GetRequestedArguments();
	sdl::V1_0::imtbase::CDocumentOperationStatus response;
	response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::Failed;

	QByteArray documentId;
	if (arguments.input->documentId){
		documentId = *arguments.input->documentId;
	}

	sdl::V1_0::imtdesk::CTicketData ticketInfo;
	if (arguments.input->ticket){
		ticketInfo = *arguments.input->ticket;
	}

	QByteArray userId;
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		userId = gqlContextPtr->GetUserId();
	}

	QByteArray userLogin = GetUserId(gqlRequest);

	istd::IChangeableSharedPtr documentPtr;
	m_documentManagerCompPtr->GetDocumentData(userLogin, documentId, documentPtr);
	if (!documentPtr.IsValid()){
		response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	imtdesk::ISupportTicket* ticketPtr = dynamic_cast<imtdesk::ISupportTicket*>(documentPtr.GetPtr());
	if (ticketPtr == nullptr){
		response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	// Permission check: only admin, reporter, or assignee can edit
	if (!CanEditTicket(gqlRequest.GetRequestContext(), ticketPtr)){
		errorMessage = QStringLiteral("Permission denied: you cannot edit this ticket");
		response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::Failed;
		return response;
	}

	// Lock permission: only admin or reporter can change lock state
	if (ticketInfo.locked){
		if (*ticketInfo.locked != ticketPtr->IsLocked() && !CanLockTicket(gqlRequest.GetRequestContext(), ticketPtr)){
			errorMessage = QStringLiteral("Permission denied: only the reporter can lock/unlock this ticket");
			response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::Failed;
			return response;
		}
	}

	const bool canEditCoreFields = CanLockTicket(gqlRequest.GetRequestContext(), ticketPtr)
			|| ticketPtr->GetReporterId().isEmpty();
	const bool titleChanged = ticketInfo.title && *ticketInfo.title != ticketPtr->GetTitle();
	const bool descriptionChanged = ticketInfo.description && *ticketInfo.description != ticketPtr->GetDescription();
	const bool assigneesChanged = ticketInfo.assigneeIds && ticketInfo.assigneeIds->ToList() != ticketPtr->GetAssigneeIds();
	const bool ticketTypeChanged = ticketInfo.ticketType
			&& imtdeskgql::GetTicketTypeFromSdlType(*ticketInfo.ticketType) != ticketPtr->GetTicketType();
	const bool priorityChanged = ticketInfo.priority
			&& imtdeskgql::GetPriorityTypeFromSdlType(*ticketInfo.priority) != ticketPtr->GetPriority();
	if ((titleChanged || descriptionChanged || assigneesChanged || ticketTypeChanged || priorityChanged) && !canEditCoreFields){
		errorMessage = QStringLiteral("Permission denied: only reporter or admin can edit ticket fields");
		response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::Failed;
		return response;
	}

	if (ticketInfo.title){
		ticketPtr->SetTitle(*ticketInfo.title);
	}

	if (ticketInfo.description){
		ticketPtr->SetDescription(*ticketInfo.description);
	}

	// Reporter can be set only for new tickets.
	// For existing tickets keep original reporter unchanged.
	if (ticketPtr->GetReporterId().isEmpty()){
		if (ticketInfo.reporterId && !ticketInfo.reporterId->isEmpty()){
			ticketPtr->SetReporterId(*ticketInfo.reporterId);
		}
		else{
			ticketPtr->SetReporterId(userId);
		}
	}

	if (ticketInfo.assigneeIds){
		ticketPtr->SetAssigneeIds(ticketInfo.assigneeIds->ToList());
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

	// Write entity references via IEntityReferenceStorage (by analogy with Attachments)
	if (ticketInfo.entityReferences && m_entityReferenceStorageCompPtr.IsValid()){
		QByteArrayList newRefIds;
		for (const auto& ref : *ticketInfo.entityReferences){
			if (!ref){
				continue;
			}

			QString entityType = ref->entityType ? *ref->entityType : QString();
			QByteArray entityId = ref->entityId ? *ref->entityId : QByteArray();
			QString displayName = ref->displayName ? *ref->displayName : QString();

			// Extract navigation path from ObjectLink if available
			QString entityUrl;
			if (ref->entityLink && ref->entityLink->url && ref->entityLink->url->path){
				entityUrl = *ref->entityLink->url->path;
			}

			QByteArray refId = m_entityReferenceStorageCompPtr->CreateEntityReference(entityType, entityId, displayName, entityUrl);
			if (!refId.isEmpty()){
				newRefIds << refId;
			}
		}
		ticketPtr->SetEntityReferences(newRefIds);
	}

	// NOTE: Conversation is NOT created here — it is created in ProcessEvent
	// after the ticket is saved and assigned a number in the database.

	if (ticketInfo.comments){
		// Comments are saved as messages via IChatService
		// Messages are NOT part of the ticket document — they don't trigger document change
		QByteArray conversationId = ticketPtr->GetConversationId();
		if (!conversationId.isEmpty() && m_chatServiceCompPtr.IsValid()){
			for (const auto& sdlItem : *ticketInfo.comments){
				if (!sdlItem){
					continue;
				}
				// Existing messages (with id) may be edited or deleted.
				if (sdlItem->id && !sdlItem->id->isEmpty()){
					QByteArray messageId = *sdlItem->id;

					if (QUuid::fromString(QString::fromUtf8(messageId)).isNull()){
						continue;
					}

					// Delete request (explicit `deleted: true` flag from client).
					if (sdlItem->deleted && *sdlItem->deleted){
						m_chatServiceCompPtr->DeleteMessage(messageId, userId);
						continue;
					}

					// Edit request: when content or attachments have changed.
					if (sdlItem->content){
						imtbase::IObjectCollection::DataPtr msgDataPtr;
						if (m_messageCollectionCompPtr.IsValid()
								&& m_messageCollectionCompPtr->GetObjectData(messageId, msgDataPtr)){
							const imtchat::IChatMessage* existingMsgPtr =
									dynamic_cast<const imtchat::IChatMessage*>(msgDataPtr.GetPtr());
							if (existingMsgPtr != nullptr){
								// Collect updated attachment IDs from the SDL item.
								QByteArrayList editAttachmentIds;
								if (sdlItem->attachments){
									for (const auto& att : *sdlItem->attachments){
										if (!att){
											continue;
										}
										if (att->id && !att->id->isEmpty()){
											QByteArray attId = *att->id;
											int dotIdx = attId.lastIndexOf('.');
											if (dotIdx > 0){
												attId = attId.left(dotIdx);
											}
											editAttachmentIds << attId;
										}
									}
								}

								bool contentChanged = existingMsgPtr->GetContent() != *sdlItem->content;
								bool attachmentsChanged = existingMsgPtr->GetAttachmentIds() != editAttachmentIds;
								if (contentChanged || attachmentsChanged){
									m_chatServiceCompPtr->EditMessage(messageId, userId, *sdlItem->content, editAttachmentIds);
								}
							}
						}
					}
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

				QByteArray replyToId;
				if (sdlItem->replyToId && !sdlItem->replyToId->isEmpty()){
					replyToId = *sdlItem->replyToId;
				}

				m_chatServiceCompPtr->SendMessage(conversationId, senderId, content, QByteArrayList() /*entityReferences*/, attachmentIds, replyToId);
			}
		}
	}

	m_documentManagerCompPtr->SetDocumentData(userLogin, documentId, *ticketPtr);

	// For new tickets (number == 0), do NOT auto-save — the user will save
	// manually via the Undo/Redo/Save commands panel in the editor.
	// For existing tickets (number > 0), save immediately after each change.
	if (ticketPtr->GetNumber() > 0){
		imtdoc::IDocumentService::TaskParams saveParams;
		saveParams.userId = userLogin;
		saveParams.documentId = documentId;
		QByteArray saveTaskId = m_documentManagerCompPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_SAVE, saveParams);
		m_documentManagerCompPtr->WaitForTaskFinished(saveTaskId);
	}

	response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::Success;

	return response;
}


// reimplemented (imtdoc::IDocumentServiceEventHandler)

bool CTicketCollectionDocumentServiceComp::ProcessEvent(imtdoc::CEventBase* eventPtr)
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
		if (m_documentManagerCompPtr->GetDocumentData(userId, documentId, documentPtr) == imtdoc::IDocumentService::OS_OK){
			documentTicketPtr = dynamic_cast<imtdesk::ISupportTicket*>(documentPtr.GetPtr());
		}

		if (dbTicketPtr != nullptr && documentTicketPtr != nullptr){
			if (documentTicketPtr->GetNumber() == 0){
				documentTicketPtr->SetId(dbTicketPtr->GetId());
				documentTicketPtr->SetNumber(dbTicketPtr->GetNumber());
				documentTicketPtr->SetUpdatedAt(dbTicketPtr->GetUpdatedAt());
				documentTicketPtr->SetCreatedAt(dbTicketPtr->GetCreatedAt());

				// Create a Conversation now that the ticket has been saved and assigned a number
				if (documentTicketPtr->GetConversationId().isEmpty() && m_chatServiceCompPtr.IsValid()){
					QByteArray convId = m_chatServiceCompPtr->CreateConversation(
								documentTicketPtr->GetTitle(),
								imtchat::IConversation::CT_SUPPORT,
								QByteArrayList());
					if (!convId.isEmpty()){
						documentTicketPtr->SetConversationId(convId);
					}
				}
	
				m_documentManagerCompPtr->SetDocumentData(userId, documentId, *documentPtr);

				imtdoc::IDocumentService::TaskParams saveParams;
				saveParams.userId = userId;
				saveParams.documentId = documentId;
				QByteArray saveTaskId = m_documentManagerCompPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_SAVE, saveParams);
				m_documentManagerCompPtr->WaitForTaskFinished(saveTaskId);
			}
		}
	}

	return true;
}




} // namespace imtdeskgql
