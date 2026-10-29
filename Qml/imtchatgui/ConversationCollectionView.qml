// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0
import imtdocgui 1.0
import imtchatImtChatSdl 1.0
import imtchatConversationCollectionDocumentServiceSdl 1.0
import imtbaseCollectionDocumentServiceSdl 1.0
import imtbaseUndoManagerSdl 1.0

RemoteCollectionView {
	id: container

	property string permissionPath: ""
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

					// A refused change stays in the editor
					readBackOnRefusal: false

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
					property DocumentRepresentationRequestSender getConversationRequest: DocumentRepresentationRequestSender {
						representationController: root
						permissionPath: container.permissionPath
						gqlCommandId: ImtchatConversationCollectionDocumentServiceSdlCommandIds.s_getConversationRepresentation
						sdlObjectComp: Component {
							ConversationData {}
						}
					}

					property UpdateConversationInput updateConversationInput: UpdateConversationInput {}
					property DocumentUpdateRequestSender updateConversationRequest: DocumentUpdateRequestSender {
						representationController: root
						permissionPath: container.permissionPath
						gqlCommandId: ImtchatConversationCollectionDocumentServiceSdlCommandIds.s_updateConversationFromRepresentation
					}
				}
			}
		}
	}
}
