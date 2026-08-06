// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0
import imtdocgui 1.0
import imtchatImtChatSdl 1.0
import imtchatConversationCollectionDocumentServiceSdl 1.0
import imtbaseCollectionDocumentServiceSdl 1.0
import imtbaseUndoManagerSdl 1.0
import imtchatgui 1.0

RemoteCollectionView {
	id: container

	collectionId: "Conversations"
	gqlGetListCommandId: ImtchatImtChatSdlCommandIds.s_conversationsList

	Component.onCompleted: {
		console.log("ConversationCollectionView.qml onCompleted")
		table.setSortingInfo(ConversationItemDataTypeMetaInfo.s_updatedAt, "DESC")
	}

	commandsDelegateComp: Component {
		DocCollectionViewDelegate {
			collectionView: container

			Component.onCompleted: {
				registerDocumentType("Conversation", qsTr("Conversation"))
				addDocumentView("Conversation", "ConversationEditor", conversationEditorComp, conversationDataControllerFactory)
			}

			Component {
				id: conversationEditorComp

				ConversationEditor {
					id: conversationEditor
					commandsControllerComp: Component {
						GqlBasedCommandsController {
							typeId: "Conversation"
						}
					}
				}
			}

			Component {
				id: conversationDataControllerFactory

				DocumentRepresentationController {
					id: root

					representationModel: ConversationData {}

					function updateRepresentationFromDocument(){
						startUpdateRepresentation(documentId, representationModel)

						documentIdInput.m_id = documentId
						getConversationRequest.send(documentIdInput)
					}

					function updateDocumentFromRepresentation(){
						startUpdateDocument(documentId)

						updateConversationInput.m_documentId = documentId
						updateConversationInput.m_conversation = representationModel

						updateConversationRequest.send(updateConversationInput)
					}

					property DocumentId documentIdInput: DocumentId {}
					property GqlSdlRequestSender getConversationRequest: GqlSdlRequestSender {
						gqlCommandId: ImtchatConversationCollectionDocumentServiceSdlCommandIds.s_getConversationRepresentation
						sdlObjectComp: Component {
							ConversationData {
								onFinished: {
									root.representationModel.copyFrom(this)
									root.representationUpdated(root.documentId, root.representationModel)
								}
							}
						}

						function onError(message, type){
							root.updateRepresentationFailed(root.documentId, message)
						}
					}

					property UpdateConversationInput updateConversationInput: UpdateConversationInput {}
					property GqlSdlRequestSender updateConversationRequest: GqlSdlRequestSender {
						gqlCommandId: ImtchatConversationCollectionDocumentServiceSdlCommandIds.s_updateConversationFromRepresentation
						requestType: 1
						sdlObjectComp: Component {
							DocumentOperationStatus {
								onFinished: {
									if (m_status === "Success"){
										root.documentUpdated(root.documentId)
									}
								}
							}
						}

						function onError(message, type){
							root.updateDocumentFailed(root.documentId, message)
						}
					}
				}
			}
		}
	}
}
