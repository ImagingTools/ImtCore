// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtdocgui 1.0
import imtdeskImtDeskSdl 1.0
import imtdeskTicketCollectionDocumentManagerSdl 1.0
import imtbaseCollectionDocumentManagerSdl 1.0
import imtbaseUndoManagerSdl 1.0

RemoteCollectionView {
	id: container

	collectionId: "Tickets"
	gqlGetListCommandId: ImtdeskImtDeskSdlCommandIds.s_ticketsList

	Component.onCompleted: {
		table.setSortingInfo(TicketItemDataTypeMetaInfo.s_createdAt, "DESC")
	}

	commandsDelegateComp: Component {
		DocCollectionViewDelegate {
			collectionView: container

			Component.onCompleted: {
				registerDocumentType("Ticket", qsTr("Ticket"))
				addDocumentView("Ticket", "TicketEditor", ticketEditorComp, ticketDataControllerFactory)
			}

			Component {
				id: ticketEditorComp

				TicketEditor {
					id: ticketEditor
					commandsControllerComp: null
				}
			}

			Component {
				id: ticketDataControllerFactory

				DocumentRepresentationController {
					id: root

					representationModel: TicketData {
						m_status: "Open"
						m_ticketType: "SupportRequest"
						m_priority: "Medium"
						m_environment: "Production"
						m_stateReason: "None"
					}

					function updateRepresentationFromDocument(){
						startUpdateRepresentation(documentId, representationModel)

						documentIdInput.m_id = documentId
						getTicketRequest.send(documentIdInput)
					}

					function updateDocumentFromRepresentation(){
						startUpdateDocument(documentId)

						updateTicketInput.m_documentId = documentId
						updateTicketInput.m_ticket = representationModel

						updateTicketRequest.send(updateTicketInput)
					}

					// Subscription: listen for document changes from other clients
					property SubscriptionClient ticketSubscription: SubscriptionClient {
						gqlCommandId: "OnTicketsDocumentChanged"

						onMessageReceived: {
							let changedDocId = data.getData("documentId")
							if (changedDocId === root.documentId){
								root.updateRepresentationFromDocument()
							}
						}

						function getHeaders(){
							return {}
						}
					}

					// Polling timer: messages are saved via IChatService::SendMessage() which does
					// not trigger ticket document change notifications (to prevent infinite loops
					// when multiple clients have the same ticket open). Polling provides
					// near-real-time message visibility across clients.
					property Timer messagePollingTimer: Timer {
						interval: 5000
						repeat: true
						running: root.documentId !== ""

						onTriggered: {
							root.updateRepresentationFromDocument()
						}
					}

					property DocumentId documentIdInput: DocumentId {}
					property GqlSdlRequestSender getTicketRequest: GqlSdlRequestSender {
						gqlCommandId: ImtdeskTicketCollectionDocumentManagerSdlCommandIds.s_getTicketRepresentation
						sdlObjectComp: Component {
							TicketData {
								onFinished: {
									root.representationModel.copyFrom(this)
									root.representationUpdated(root.documentId, root.representationModel)
								}
							}
						}
					}

					property UpdateTicketInput updateTicketInput: UpdateTicketInput {}
					property GqlSdlRequestSender updateTicketRequest: GqlSdlRequestSender {
						gqlCommandId: ImtdeskTicketCollectionDocumentManagerSdlCommandIds.s_updateTicketFromRepresentation
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
					}
				}
			}
		}
	}
}
